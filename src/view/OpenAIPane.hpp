#ifndef OPENAI_PANE_HPP
#define OPENAI_PANE_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QProcess>
#include <QScrollBar>
#include <QProcessEnvironment>
#include <QToolButton>
#include <QRegularExpression>
#include <QTimer>
#include <QComboBox>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include "../service/PersistenceService.hpp"

namespace Parcel::View {

    class OpenAIPane : public QWidget {
        Q_OBJECT
    public:
        explicit OpenAIPane(QWidget *parent = nullptr) : QWidget(parent) {
            auto* mainLayout = new QVBoxLayout(this);
            mainLayout->setContentsMargins(0, 0, 0, 0);
            mainLayout->setSpacing(0);
            setStyleSheet("background-color: #1e1f22; border-left: 1px solid #3c3f41;");

            // Header
            auto* header = new QWidget(this);
            header->setFixedHeight(40);
            header->setStyleSheet("background-color: #2b2d30; border-bottom: 1px solid #3c3f41;");
            auto* headerLayout = new QHBoxLayout(header);
            headerLayout->setContentsMargins(10, 0, 10, 0);

            auto* title = new QLabel("🤖 OpenAI", this);
            title->setStyleSheet("font-size: 13px; font-weight: bold; color: #dfe1e5;");

            modelSelector = new QComboBox(this);
            modelSelector->addItems({
                "gpt-4o",
                "gpt-4-turbo",
                "gpt-3.5-turbo",
                "gpt-5.6"
            });
            modelSelector->setCurrentIndex(0);
            modelSelector->setStyleSheet("QComboBox { background-color: #3c3f41; color: #dfe1e5; border: none; padding: 2px 5px; font-size: 11px; }");

            auto* clearBtn = new QToolButton(this);
            clearBtn->setIcon(QIcon::fromTheme("edit-clear-all"));
            connect(clearBtn, &QToolButton::clicked, [this]() {
                chatHistory->clear();
            });

            headerLayout->addWidget(title);
            headerLayout->addStretch();
            headerLayout->addWidget(modelSelector);
            headerLayout->addSpacing(10);
            headerLayout->addWidget(clearBtn);
            mainLayout->addWidget(header);

            chatHistory = new QTextEdit(this);
            chatHistory->setReadOnly(true);
            chatHistory->setStyleSheet("QTextEdit { background-color: #1e1f22; border: none; padding: 10px; color: #bcbec4; font-size: 13px; }");
            mainLayout->addWidget(chatHistory);

            statusBanner = new QLabel("🤖 <b>Status:</b> OpenAI Pronto", this);
            statusBanner->setWordWrap(true);
            statusBanner->setStyleSheet("QLabel { background-color: #2b2d30; color: #aaa; border-top: 1px solid #3c3f41; padding: 8px 12px; font-size: 10px; font-family: 'Monospace'; }");
            mainLayout->addWidget(statusBanner);

            auto* inputLayout = new QVBoxLayout();
            inputLayout->setContentsMargins(10, 10, 10, 10);
            userInput = new QLineEdit(this);
            userInput->setPlaceholderText("Pergunte ao ChatGPT...");
            userInput->setFixedHeight(35);
            userInput->setStyleSheet("QLineEdit { background-color: #2b2d30; color: #bcbec4; border: 1px solid #4e5157; border-radius: 6px; padding-left: 10px; }");

            inputLayout->addWidget(userInput);
            mainLayout->addLayout(inputLayout);

            process = new QProcess(this);
            process->setProcessChannelMode(QProcess::MergedChannels);

            connect(userInput, &QLineEdit::returnPressed, this, &OpenAIPane::handleSend);
            connect(process, &QProcess::readyReadStandardOutput, this, &OpenAIPane::readOutput);
            connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &OpenAIPane::onFinished);
        }

        ~OpenAIPane() {
            if (process && process->state() != QProcess::NotRunning) {
                process->terminate();
                if (!process->waitForFinished(500)) {
                    process->kill();
                }
            }
        }

    private slots:
        void handleSend() {
            QString text = userInput->text().trimmed();
            if (text.isEmpty() || process->state() != QProcess::NotRunning) return;

            chatHistory->append(QString("<div style='margin-top: 10px;'><b style='color: #bcbec4;'>Você</b><br>%1</div>").arg(text));

            userInput->clear();
            userInput->setEnabled(false);

            QString model = modelSelector->currentText();
            statusBanner->setText("🤖 <b>Status:</b> Chamando OpenAI...");

            QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
            // Tenta buscar no .bashrc se nao estiver no env
            if (!env.contains("OPENAI_API_KEY")) {
                QFile bashrc(QDir::homePath() + "/.bashrc");
                if (bashrc.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QTextStream in(&bashrc);
                    while (!in.atEnd()) {
                        QString line = in.readLine().trimmed();
                        if (line.startsWith("export OPENAI_API_KEY=")) {
                            QString key = line.section('=', 1).trimmed().remove('"').remove('\'');
                            env.insert("OPENAI_API_KEY", key);
                            break;
                        }
                    }
                }
            }

            QString pythonScript = QString(
                "import sys\n"
                "from openai import OpenAI\n"
                "try:\n"
                "    client = OpenAI(api_key='%1')\n"
                "    user_input = sys.stdin.read()\n"
                "    try:\n"
                "        # Usando o padrao solicitado responses.create\n"
                "        response = client.responses.create(model='%2', input=user_input)\n"
                "        print(response.output_text)\n"
                "    except:\n"
                "        # Fallback para chat.completions\n"
                "        response = client.chat.completions.create(\n"
                "            model='gpt-4o',\n"
                "            messages=[{\"role\": \"user\", \"content\": user_input}]\n"
                "        )\n"
                "        print(response.choices[0].message.content)\n"
                "except Exception as e:\n"
                "    print(f'Erro: {e}', file=sys.stderr)\n"
            ).arg(env.value("OPENAI_API_KEY"), model);

            process->setProcessEnvironment(env);
            process->start("/home/marcel1237/venvs/gemini/bin/python3", QStringList() << "-c" << pythonScript);
            process->write(text.toUtf8());
            process->closeWriteChannel();

            chatHistory->append(QString("<b style='color: #10a37f;'>✨ OpenAI (%1)</b>").arg(model));
            hasOutput = false;
        }

        void readOutput() {
            QString output = QString::fromUtf8(process->readAllStandardOutput());
            if (output.isEmpty()) return;
            hasOutput = true;
            chatHistory->insertPlainText(output);
            chatHistory->verticalScrollBar()->setValue(chatHistory->verticalScrollBar()->maximum());
        }

        void onFinished(int exitCode) {
            userInput->setEnabled(true);
            userInput->setFocus();
            statusBanner->setText("🤖 <b>Status:</b> OpenAI Pronto");
            if (exitCode != 0 && !hasOutput) {
                chatHistory->append("<i style='color: #ff4d4d;'>[Erro na execução do script OpenAI]</i>");
            }
        }

    private:
        QTextEdit* chatHistory;
        QLabel* statusBanner;
        QLineEdit* userInput;
        QComboBox* modelSelector;
        QProcess* process;
        bool hasOutput = false;
    };
}

#endif

#ifndef GEMINI_PANE_HPP
#define GEMINI_PANE_HPP

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

    class GeminiPane : public QWidget {
        Q_OBJECT
    public:
        explicit GeminiPane(QWidget *parent = nullptr) : QWidget(parent) {
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

            auto* title = new QLabel("✨ Gemini", this);
            title->setStyleSheet("font-size: 13px; font-weight: bold; color: #dfe1e5;");

            modelSelector = new QComboBox(this);
            // Modelos Locais (Ollama) e Cloud (Gemini)
            modelSelector->addItems({
                "qwen2.5-coder:1.5b",
                "gemini-1.5-flash",
                "gemini-1.5-pro",
                "gemini-3.5-flash",
                "gemini-pro"
            });
            modelSelector->setCurrentIndex(0); // Qwen como DEFAULT
            modelSelector->setStyleSheet("QComboBox { background-color: #3c3f41; color: #dfe1e5; border: none; padding: 2px 5px; font-size: 11px; }");

            auto* clearBtn = new QToolButton(this);
            clearBtn->setIcon(QIcon::fromTheme("edit-clear-all"));
            connect(clearBtn, &QToolButton::clicked, [this]() {
                chatHistory->clear();
                lastContextKey.clear();
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

            statusBanner = new QLabel(this);
            statusBanner->setWordWrap(true);
            statusBanner->setStyleSheet("QLabel { background-color: #2b2d30; color: #aaa; border-top: 1px solid #3c3f41; padding: 8px 12px; font-size: 10px; font-family: 'Monospace'; }");
            mainLayout->addWidget(statusBanner);
            updateUsageDisplay();

            auto* inputLayout = new QVBoxLayout();
            inputLayout->setContentsMargins(10, 10, 10, 10);
            userInput = new QLineEdit(this);
            userInput->setPlaceholderText("Pergunte ao Gemini...");
            userInput->setFixedHeight(35);
            userInput->setStyleSheet("QLineEdit { background-color: #2b2d30; color: #bcbec4; border: 1px solid #4e5157; border-radius: 6px; padding-left: 10px; }");

            inputLayout->addWidget(userInput);
            mainLayout->addLayout(inputLayout);

            process = new QProcess(this);
            process->setProcessChannelMode(QProcess::MergedChannels);

            countdownTimer = new QTimer(this);
            countdownTimer->setInterval(100);

            connect(userInput, &QLineEdit::returnPressed, this, &GeminiPane::handleSend);
            connect(process, &QProcess::readyReadStandardOutput, this, &GeminiPane::readOutput);
            connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &GeminiPane::onFinished);
            connect(countdownTimer, &QTimer::timeout, this, &GeminiPane::updateCountdown);
        }

        ~GeminiPane() {
            if (process && process->state() != QProcess::NotRunning) {
                process->terminate();
                if (!process->waitForFinished(500)) {
                    process->kill();
                }
            }
        }

        void setContext(const QString& fileName, const QString& content) {
            currentFileName = fileName;
            // Pega o início do arquivo para contexto (máx 5000 chars)
            currentFileContent = content.left(5000);
        }

        void setProjectPath(const QString& path) {
            projectRoot = path;
        }

        void ask(const QString& prompt) {
            userInput->setText(prompt);
            handleSend();
        }

    private slots:
        void handleSend() {
            QString text = userInput->text().trimmed();
            if (text.isEmpty() || process->state() != QProcess::NotRunning) return;

            chatHistory->append(QString("<div style='margin-top: 10px;'><b style='color: #bcbec4;'>Você</b><br>%1</div>").arg(text));

            userInput->clear();
            userInput->setEnabled(false);

            QString model = modelSelector->currentText();
            bool isLocal = model.contains("qwen");

            statusBanner->setText(isLocal ? "🤖 <b>Status:</b> Chamando Ollama (Local)..." : "🤖 <b>Status:</b> Chamando Gemini CLI...");

            QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

            if (!isLocal) {
                // 1. TENTA BUSCAR NA CONFIGURAÇÃO (CRIPTOGRAFADA) PARA GEMINI
                Service::PersistenceService persistence;
                std::string savedKey = persistence.getGeminiApiKey();

                if (!savedKey.empty()) {
                    env.insert("GEMINI_API_KEY", QString::fromStdString(savedKey));
                } else if (!env.contains("GEMINI_API_KEY")) {
                    // 2. BUSCA NO .BASHRC COMO FALLBACK
                    QFile bashrc(QDir::homePath() + "/.bashrc");
                    if (bashrc.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        QTextStream in(&bashrc);
                        while (!in.atEnd()) {
                            QString line = in.readLine().trimmed();
                            if (line.startsWith("export GEMINI_API_KEY=")) {
                                QString key = line.section('=', 1).trimmed().remove('"').remove('\'');
                                env.insert("GEMINI_API_KEY", key);
                                break;
                            }
                        }
                    }
                }
            }

            QString fullPrompt = text;

            // Persona: Autonomous Reality Check: Qwen 2.5 Coder (1.5B)
            QString systemInstruction =
                "Você é o agente 'Autonomous Reality Check', baseado no modelo Qwen 2.5 Coder. "
                "Suas capacidades autônomas incluem: Refatoração de arquivo único, Reparo automático de código (corrigindo erros de sintaxe) "
                "e Completagem preditiva de boilerplate. "
                "Sempre responda de forma técnica, direta e focada em C++, Java ou Kotlin.";

            if (!currentFileName.isEmpty()) {
                fullPrompt = QString("%1\n\nContexto: Arquivo %2\n```\n%3\n```\nPergunta: %4")
                                .arg(systemInstruction, currentFileName, currentFileContent, text);
            } else {
                fullPrompt = QString("%1\n\nPergunta: %2").arg(systemInstruction, text);
            }

            process->setProcessEnvironment(env);
            if (!projectRoot.isEmpty()) process->setWorkingDirectory(projectRoot);

            if (isLocal) {
                // Comando para Ollama
                QStringList args;
                args << "run" << model << fullPrompt;
                process->start("ollama", args);
                chatHistory->append("<b style='color: #f1c40f;'>✨ Qwen (Local)</b>");
            } else {
                // Usando SDK Python oficial recomendado (2.0 Flash para evitar Erro 500)
                QString model = modelSelector->currentText();
                if (model == "gemini-3.5-flash") model = "gemini-2.0-flash"; // Redireciona o instável

                QString pythonScript = QString(
                    "from google import genai\n"
                    "import sys\n"
                    "try:\n"
                    "    client = genai.Client(api_key='%1')\n"
                    "    user_input = sys.stdin.read()\n"
                    "    response = client.interactions.create(model='%2', input=user_input)\n"
                    "    \n"
                    "    out = getattr(response, 'output_text', '')\n"
                    "    if not out and hasattr(response, 'outputs') and response.outputs:\n"
                    "        out = response.outputs[-1].text\n"
                    "    print(out if out else 'Erro: API retornou vazio')\n"
                    "except Exception as e:\n"
                    "    print(f'Erro: {e}', file=sys.stderr)\n"
                ).arg(env.value("GEMINI_API_KEY"), model);

                process->start("/home/marcel1237/venvs/gemini/bin/python3", QStringList() << "-c" << pythonScript);
                process->write(fullPrompt.toUtf8());
                process->closeWriteChannel();

                chatHistory->append(QString("<b style='color: #4285f4;'>✨ Gemini (%1) via SDK Python</b>").arg(model));
            }
            hasOutput = false;
        }

        void readOutput() {
            QString output = QString::fromUtf8(process->readAllStandardOutput());
            if (output.isEmpty()) return;

            // Limpa códigos de escape ANSI do terminal
            static QRegularExpression ansiRegex("\x1b\\[[0-9;]*[mK]");
            output.remove(ansiRegex);

            // Detecta erros de cota
            if (output.contains("Quota exceeded") || output.contains("429")) {
                handleRateLimit(output);
                return;
            }

            // Ignora apenas o ruído inútil do Ripgrep, mas deixa passar outros logs para depuração
            if (output.contains("Ripgrep is not available", Qt::CaseInsensitive)) return;

            hasOutput = true;
            QTextCursor cursor = chatHistory->textCursor();
            cursor.movePosition(QTextCursor::End);
            chatHistory->setTextCursor(cursor);
            chatHistory->insertPlainText(output);
            chatHistory->verticalScrollBar()->setValue(chatHistory->verticalScrollBar()->maximum());
        }

        void handleRateLimit(const QString& msg) {
            double seconds = 60.0;
            QRegularExpression re("retry in ([\\d\\.]+)s");
            QRegularExpressionMatch match = re.match(msg);
            if (match.hasMatch()) seconds = match.captured(1).toDouble();
            remainingSeconds = seconds;
            countdownTimer->start();
            process->terminate();
        }

        void updateCountdown() {
            remainingSeconds -= 0.1;
            if (remainingSeconds <= 0) {
                remainingSeconds = 0;
                countdownTimer->stop();
                updateUsageDisplay();
            } else {
                statusBanner->setText(QString("<b style='color: #e67e22;'>⚠️ RPM LIMIT: Aguarde %1s</b>").arg(QString::number(remainingSeconds, 'f', 1)));
            }
        }

        void updateUsageDisplay() {
            Service::PersistenceService persistence;
            auto stats = persistence.getUsageStats();
            statusBanner->setText(QString("✨ <b>Uso:</b> Hoje: %1 | Semana: %2 | Mês: %3").arg(stats.daily).arg(stats.weekly).arg(stats.monthly));
        }

        void onFinished(int exitCode) {
            userInput->setEnabled(true);
            userInput->setFocus();
            if (exitCode == 0 && hasOutput) {
                Service::PersistenceService persistence;
                persistence.incrementUsage();
            }
            if (!hasOutput && !countdownTimer->isActive()) {
                chatHistory->append("<i style='color: #6b6e74; font-size: 11px;'>[Sistema: O CLI não produziu saída. Verifique se o binário /snap/bin/gemini funciona manualmente no terminal.]</i>");
            }
            statusBanner->setText("✨ <b>Status:</b> Gemini Pronto");
        }

    private:
        QTextEdit* chatHistory;
        QLabel* statusBanner;
        QLineEdit* userInput;
        QComboBox* modelSelector;
        QProcess* process;
        QTimer* countdownTimer;
        double remainingSeconds = 0;
        QString projectRoot;
        QString currentFileName;
        QString currentFileContent;
        bool hasOutput = false;
        QString lastContextKey;
    };
}

#endif

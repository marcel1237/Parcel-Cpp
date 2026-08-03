#ifndef SSAI_CONSOLE_PANE_HPP
#define SSAI_CONSOLE_PANE_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTemporaryFile>
#include <QDir>
#include "SS AI Agent/SSAIModule.hpp"

namespace Parcel::View {

    class SSAIConsolePane : public QWidget {
        Q_OBJECT
    public:
        explicit SSAIConsolePane(QWidget* parent = nullptr) : QWidget(parent) {
            auto* layout = new QVBoxLayout(this);
            m_agent = new SSAI::SSAIModule(this);

            auto* header = new QLabel("🤖 SS AI AGENT CONSOLE - QLM Engine");
            header->setStyleSheet("font-weight: bold; color: #00BFFF; font-size: 14px; margin-bottom: 5px;");
            layout->addWidget(header);

            m_logView = new QTextEdit(this);
            m_logView->setReadOnly(true);
            m_logView->setStyleSheet("background-color: #0f111a; color: #adbac7; font-family: 'Monospace'; border: 1px solid #1e1e30;");
            m_logView->append("[SYSTEM] Agent initialized with QLM Knowledge Base.");
            m_logView->append("<font color='#adbac7'><i>Dica: Pergunte 'quem é você' ou diga 'analise main.cpp'</i></font>");

            m_statusLabel = new QLabel("Foco: Nenhum arquivo selecionado");
            m_statusLabel->setStyleSheet("color: #00BFFF; font-size: 11px; font-weight: bold; background-color: #1e1e30; padding: 3px 10px; border-radius: 4px;");
            layout->addWidget(m_statusLabel);

            layout->addWidget(m_logView);

            auto* inputLayout = new QHBoxLayout();
            m_input = new QLineEdit(this);
            m_input->setPlaceholderText("Enter instruction...");
            m_input->setStyleSheet("background-color: #161925; color: white; border: 1px solid #4285F4; padding: 8px; border-radius: 4px;");

            auto* btnRun = new QPushButton("Execute", this);
            btnRun->setStyleSheet("background-color: #4285F4; color: white; font-weight: bold; padding: 8px 15px;");

            inputLayout->addWidget(m_input);
            inputLayout->addWidget(btnRun);
            layout->addLayout(inputLayout);

            connect(btnRun, &QPushButton::clicked, this, &SSAIConsolePane::onExecuteClicked);
            connect(m_input, &QLineEdit::returnPressed, this, &SSAIConsolePane::onExecuteClicked);

            // Connect Gemini signals
            connect(m_agent, &SSAI::SSAIModule::geminiResponseReceived, this, [this](const QString& response) {
                m_logView->append("<font color='#00BFFF'>[GEMINI]</font> " + response);
            });
            connect(m_agent, &SSAI::SSAIModule::geminiErrorOccurred, this, [this](const QString& error) {
                m_logView->append("<font color='#FF4500'>[ERROR]</font> " + error);
            });

            // Connect OpenAI signals
            connect(m_agent, &SSAI::SSAIModule::openaiResponseReceived, this, [this](const QString& response) {
                m_logView->append("<font color='#10a37f'>[OPENAI]</font> " + response);
            });
            connect(m_agent, &SSAI::SSAIModule::openaiErrorOccurred, this, [this](const QString& error) {
                m_logView->append("<font color='#FF4500'>[ERROR]</font> " + error);
            });
        }

    private slots:
        void onExecuteClicked() {
            QString cmd = m_input->text().trimmed();
            if (cmd.isEmpty()) return;

            if (cmd.toLower().startsWith("set key ")) {
                m_logView->append("\n> set key [REDACTED]");
            } else {
                m_logView->append("\n> " + cmd);
            }
            m_input->clear();

            auto response = m_agent->processInstruction(cmd);

            if (response.type == SSAI::SSAIModule::QLMResponse::MESSAGE) {
                if (response.message.contains("Extraordinary SSQLM Plan")) {
                    m_logView->append("<div style='background-color: #1a1b26; border-left: 4px solid #BB9AF7; padding: 10px; margin: 5px; color: #c0caf5;'>" + response.message + "</div>");
                } else {
                    m_logView->append("<font color='#00BFFF'>[AGENT]</font> " + response.message);
                }
            }
            else if (response.type == SSAI::SSAIModule::QLMResponse::ASYNC_WAIT) {
                m_logView->append("<font color='#adbac7'><i>[SISTEMA] " + response.message + "</i></font>");
            }
            else if (response.type == SSAI::SSAIModule::QLMResponse::ACTION) {
                m_logView->append("<font color='#4285F4'>[SSAI Hint]</font> " + response.message);

                if (response.targetFile != "") {
                    m_statusLabel->setText("Foco: " + response.targetFile);
                }

                if (response.actionType == "ANALYZE") {
                    m_logView->append(m_agent->requestAnalysis(response.targetFile));
                } else if (response.actionType == "APPLY_TEMPLATE") {
                    QString templateContent = "#!/usr/bin/env bash\nset -Eeuo pipefail\n# Extraordinary SSQLM Template\n";
                    auto status = m_agent->requestFullBuild(response.targetFile, templateContent);
                    if (status == SSAI::FilePatcher::PatchStatus::Success) {
                         m_logView->append("<font color='#00FF7F'>[SUCCESS]</font> Template aplicado em " + response.targetFile);
                    }
                } else if (response.actionType == "PYTHON") {
                    // Gemini Python execution (existing)
                    m_logView->append("[SSAI] Executando Gemini via SDK Python oficial...");
                    // ... gemini logic ...
                } else if (response.actionType == "PYTHON_GPT") {
                    m_logView->append("[SSAI] Executando OpenAI via SDK Python oficial...");
                    QString userMsg = cmd.replace("rode gpt ", "");
                    QString escapedMsg = userMsg.replace("\\", "\\\\").replace("\"", "\\\"").replace("\n", "\\n");
                    QString pythonCode = QString(
                        "from openai import OpenAI\n"
                        "import sys\n"
                        "try:\n"
                        "    client = OpenAI(api_key='%1')\n"
                        "    try:\n"
                        "        response = client.responses.create(model='gpt-5.6', input=\"%2\")\n"
                        "        print(response.output_text)\n"
                        "    except:\n"
                        "        response = client.chat.completions.create(\n"
                        "            model='gpt-4o',\n"
                        "            messages=[{\"role\": \"user\", \"content\": \"%2\"}]\n"
                        "        )\n"
                        "        print(response.choices[0].message.content)\n"
                        "except Exception as e:\n"
                        "    print(f'Erro no script OpenAI: {e}')\n"
                    ).arg(qgetenv("OPENAI_API_KEY"), escapedMsg);

                    auto result = m_agent->requestPythonExecution(pythonCode);
                    if (result.success) {
                        m_logView->append("<font color='#10a37f'>[PYTHON/OPENAI]</font><br>" + result.output.replace("\n", "<br>"));
                    } else {
                        m_logView->append("<font color='#FF4500'>[ERRO OPENAI]</font><br>" + result.error);
                    }
                } else if (response.actionType == "PREVIEW_SCRIPT") {
                    m_logView->append("<font color='#FFD700'>[SSQLM GEN]</font> Script gerado:");
                    m_logView->append("<pre style='color: #7AA2F7; background-color: #1a1b26; padding: 10px;'>" + response.targetFile + "</pre>");
                    m_logView->append("<font color='#adbac7'><i>Dica: Digite 'executar' para rodar este script agora.</i></font>");
                } else if (response.actionType == "EXECUTE_SCRIPT") {
                    m_logView->append("[SYSTEM] Preparando execução segura...");
                    QTemporaryFile* tempSh = new QTemporaryFile(QDir::tempPath() + "/ssqlm_exec_XXXXXX.sh", this);
                    tempSh->setAutoRemove(true);
                    if (tempSh->open()) {
                        tempSh->write(response.targetFile.toUtf8());
                        QString path = tempSh->fileName();
                        tempSh->close();

                        QFile::setPermissions(path, QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner | QFile::ReadUser | QFile::ExeUser);

                        QProcess* proc = new QProcess(this);
                        connect(proc, &QProcess::readyReadStandardOutput, [this, proc]() {
                            m_logView->append("<font color='#888'>" + QString::fromUtf8(proc->readAllStandardOutput()).trimmed() + "</font>");
                        });
                        connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [this, proc](int exitCode) {
                            m_logView->append(QString("<font color='%1'>[FINISHED] Script finalizado com código %2</font>")
                                .arg(exitCode == 0 ? "#00FF7F" : "#FF4500").arg(exitCode));
                            proc->deleteLater();
                        });
                        proc->start("bash", QStringList() << path);
                    }
                } else if (response.actionType == "OPEN") {
                    m_logView->append("<font color='#00FF7F'>[SSAI]</font> Abrindo arquivo: " + response.targetFile);
                    // Emit a signal or call a method to open in editor
                } else if (response.actionType == "SAVE_RESEARCH") {
                    m_logView->append(m_agent->saveResearchResult("Web Research", response.targetFile));
                } else if (response.actionType == "SAVE_LAST_AI") {
                    m_logView->append(m_agent->saveResearchResult("AI Knowledge", ""));
                } else if (response.actionType == "CODE_SAMPLE") {
                    m_logView->append("<font color='#FFD700'>[SSQLM CODE]</font> Exemplo de implementação:");
                    m_logView->append("<pre style='color: #E0AF68; background-color: #1a1b26; padding: 10px; border: 1px solid #333;'>" + response.targetFile.toHtmlEscaped() + "</pre>");
                } else if (response.actionType == "WEB_DEEP_SEARCH") {
                    m_logView->append("<font color='#00BFFF'>[SSAI]</font> Iniciando pesquisa exaustiva na internet...");
                    QString webResult = m_agent->requestWebSearch(response.targetFile);
                    m_logView->append(webResult);
                    m_logView->append("<font color='#adbac7'><i>Enviando síntese para o cérebro da IA...</i></font>");
                    // A orquestração final acontece no SSAIModule
                } else {
                    // Placeholder for other actions
                    m_logView->append("Executing " + response.actionType + " on " + response.targetFile + "...");
                }
            }
            else if (response.type == SSAI::SSAIModule::QLMResponse::ERROR) {
                m_logView->append("<font color='#FF4500'>[ERROR]</font> " + response.message);
            }
        }

    private:
        SSAI::SSAIModule* m_agent;
        QTextEdit* m_logView;
        QLineEdit* m_input;
        QLabel* m_statusLabel;
    };

}

#endif // SSAI_CONSOLE_PANE_HPP

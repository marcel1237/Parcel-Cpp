#ifndef SHELL_SCRIPTING_PANE_HPP
#define SHELL_SCRIPTING_PANE_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QScrollArea>
#include <QGroupBox>
#include <QIcon>
#include <QProcess>
#include <QDir>
#include <QFile>

namespace Parcel::View {

    class ShellScriptingPane : public QWidget {
        Q_OBJECT
    public:
        explicit ShellScriptingPane(QWidget* parent = nullptr) : QWidget(parent) {
            auto* mainLayout = new QVBoxLayout(this);
            mainLayout->setContentsMargins(10, 10, 10, 10);
            mainLayout->setSpacing(15);

            auto* title = new QLabel("🐚 Shell Scripting Assistant", this);
            title->setStyleSheet("font-size: 18px; font-weight: bold; color: #4285F4;");
            mainLayout->addWidget(title);

            // Gerador de Scripts
            auto* genGroup = new QGroupBox("Geradores Rápidos", this);
            genGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #bcbec4; border: 1px solid #3c3f41; margin-top: 10px; padding-top: 15px; }");
            auto* genLayout = new QVBoxLayout(genGroup);

            auto* btnCron = new QPushButton(QIcon::fromTheme("appointment-new"), " Gerar Tarefa Cron (Agendamento)", this);
            auto* btnBackup = new QPushButton(QIcon::fromTheme("document-save"), " Gerar Script de Backup", this);
            auto* btnInstaller = new QPushButton(QIcon::fromTheme("system-software-install"), " Gerar Instalador (.deb/ppa)", this);

            QString btnStyle = "QPushButton { text-align: left; padding: 8px; background: #2b2d30; border: 1px solid #3c3f41; color: #bcbec4; border-radius: 4px; } "
                               "QPushButton:hover { background: #393b40; }";
            btnCron->setStyleSheet(btnStyle);
            btnBackup->setStyleSheet(btnStyle);
            btnInstaller->setStyleSheet(btnStyle);

            genLayout->addWidget(btnCron);
            genLayout->addWidget(btnBackup);
            genLayout->addWidget(btnInstaller);
            mainLayout->addWidget(genGroup);

            // Ferramentas de Análise
            auto* toolsGroup = new QGroupBox("Ferramentas de Análise", this);
            toolsGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #bcbec4; border: 1px solid #3c3f41; margin-top: 10px; padding-top: 15px; }");
            auto* toolsLayout = new QVBoxLayout(toolsGroup);

            auto* btnLint = new QPushButton(QIcon::fromTheme("system-search"), " Check Syntax (ShellCheck)", this);
            auto* btnExplain = new QPushButton(QIcon::fromTheme("help-about"), " Explicar Seleção via IA", this);

            btnLint->setStyleSheet(btnStyle);
            btnExplain->setStyleSheet(btnStyle);

            toolsLayout->addWidget(btnLint);
            toolsLayout->addWidget(btnExplain);
            mainLayout->addWidget(toolsGroup);

            // Área de Output / Preview
            auto* outputLabel = new QLabel("Preview do Script Gerado:", this);
            outputLabel->setStyleSheet("color: #bcbec4; font-weight: bold;");
            mainLayout->addWidget(outputLabel);

            scriptPreview = new QTextEdit(this);
            scriptPreview->setReadOnly(true);
            scriptPreview->setStyleSheet("background-color: #0c0c0c; color: #aaaaaa; font-family: 'Monospace'; font-size: 12px; border: 1px solid #3c3f41;");
            mainLayout->addWidget(scriptPreview, 1);

            connect(btnCron, &QPushButton::clicked, this, &ShellScriptingPane::onCronClicked);
            connect(btnBackup, &QPushButton::clicked, this, &ShellScriptingPane::onBackupClicked);
            connect(btnLint, &QPushButton::clicked, this, &ShellScriptingPane::onLintClicked);
            connect(btnExplain, &QPushButton::clicked, this, &ShellScriptingPane::onExplainClicked);

            linterProcess = new QProcess(this);
            connect(linterProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                    this, &ShellScriptingPane::handleLintResults);
        }

    signals:
        void requestAIExplanation(const QString& script);

    private slots:
        void onCronClicked() {
            scriptPreview->setPlainText("#!/bin/bash\n# Exemplo de Cron Job\n# Roda todo dia às 03:00 AM\n0 3 * * * /path/to/your/script.sh >> /var/log/myscript.log 2>&1");
        }

        void onBackupClicked() {
            scriptPreview->setPlainText("#!/bin/bash\n# Script de Backup Automático\nDEST=\"/home/$USER/backups\"\nmkdir -p \"$DEST\"\ntar -czf \"$DEST/project_$(date +%Y%m%d).tar.gz\" .\necho \"Backup concluído em $DEST\"");
        }

        void onLintClicked() {
            QString content = scriptPreview->toPlainText();
            if (content.isEmpty()) return;

            QString tempPath = QDir::tempPath() + "/parcel_shell_lint.sh";
            QFile tempFile(tempPath);
            if (tempFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                tempFile.write(content.toUtf8());
                tempFile.close();
            }

            // Tenta usar shellcheck, se falhar no handleLintResults tentaremos bash -n
            lastLinterAttempt = "shellcheck";
            linterProcess->start("shellcheck", {tempPath});
        }

        void onExplainClicked() {
            QString selected = scriptPreview->textCursor().selectedText();
            if (selected.isEmpty()) selected = scriptPreview->toPlainText();
            if (selected.isEmpty()) return;

            emit requestAIExplanation(selected);
        }

        void handleLintResults() {
            QString output = linterProcess->readAllStandardOutput();
            QString error = linterProcess->readAllStandardError();

            if (linterProcess->exitCode() == 127 && lastLinterAttempt == "shellcheck") {
                // Shellcheck não encontrado, tenta bash -n
                lastLinterAttempt = "bash";
                QString tempPath = QDir::tempPath() + "/parcel_shell_lint.sh";
                linterProcess->start("bash", {"-n", tempPath});
                return;
            }

            QString fullResult = output + error;
            if (fullResult.isEmpty()) {
                if (linterProcess->exitCode() == 0) {
                    scriptPreview->append("\n\n# ✅ Script validado com sucesso (" + lastLinterAttempt + ").");
                } else {
                    scriptPreview->append("\n\n# ❌ Erro na execução do linter (Exit code: " + QString::number(linterProcess->exitCode()) + ").");
                }
            } else {
                scriptPreview->append("\n\n# ⚠️ Resultados da Análise (" + lastLinterAttempt + "):\n" + fullResult.split("\n").join("\n# "));
            }
        }

    private:
        QTextEdit* scriptPreview;
        QProcess* linterProcess;
        QString lastLinterAttempt;
    };
}

#endif

#ifndef PROJECT_WORKSPACE_HPP
#define PROJECT_WORKSPACE_HPP

#include <QWidget>
#include <QSplitter>
#include <QTreeView>
#include <QTabWidget>
#include <QFileSystemModel>
#include <QFileInfo>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QApplication>
#include <QClipboard>
#include <QProcess>
#include "ProjectHeader.hpp"
#include "explorer/PackageExplorer.hpp"
#include "explorer/DBMSExplorer.hpp"
#include "explorer/CustomIconProvider.hpp"
#include "explorer/FileTreeDelegate.hpp"
#include "editor/EditorHost.hpp"
#include "GeminiPane.hpp"
#include "OpenAIPane.hpp"
#include "BrowserPane.hpp"
#include "TerminalPane.hpp"
#include "InspectionPane.hpp"
#include "DatabasePane.hpp"
#include "LogcatPane.hpp"
#include "VersionsPane.hpp"
#include "SavesPane.hpp"
#include "ShellScriptingPane.hpp"
#include "GitPane.hpp"
#include "SSAIConsolePane.hpp"
#include "editor/DesignerPane.hpp"
#include "editor/DiffDialog.hpp"
#include "../service/PdfService.hpp"
#include "../service/BackupService.hpp"

namespace Parcel::View {
    class ProjectWorkspace : public QWidget {
        Q_OBJECT
    public:
        explicit ProjectWorkspace(const std::string& projectPath, QWidget *parent = nullptr)
            : QWidget(parent), m_projectPath(projectPath) {

            Service::BackupService::getInstance().init(projectPath);

            auto* vLayout = new QVBoxLayout(this);
            vLayout->setContentsMargins(0, 0, 0, 0);
            vLayout->setSpacing(0);

            // Header compacto
            header = new ProjectHeader(projectPath, this);
            vLayout->addWidget(header);

            verticalSplitter = new QSplitter(Qt::Vertical, this);
            horizontalSplitter = new QSplitter(Qt::Horizontal, this);

            // Explorer
            treeView = new QTreeView(this);
            treeView->setIconSize(QSize(22, 22));
            treeView->setStyleSheet("QTreeView { font-size: 15px; } QTreeView::item { height: 28px; }");
            treeView->setContextMenuPolicy(Qt::CustomContextMenu);

            model = new QFileSystemModel(this);
            model->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
            iconProvider = new CustomIconProvider();
            model->setIconProvider(iconProvider);
            model->setReadOnly(false); // Permite edições como renomear
            model->setRootPath(QString::fromStdString(projectPath));
            treeView->setModel(model);
            treeView->setRootIndex(model->index(QString::fromStdString(projectPath)));

            treeView->hideColumn(1);
            treeView->hideColumn(2);
            treeView->hideColumn(3);
            treeView->header()->hide();
            treeView->setItemDelegate(new FileTreeDelegate(this));

            // DBMS Explorer
            dbmsExplorer = new DBMSExplorer(projectPath, this);

            // Tabs do Explorer (Esquerda)
            leftTabs = new QTabWidget(this);
            leftTabs->addTab(treeView, QIcon::fromTheme("folder"), "Files");
            leftTabs->addTab(dbmsExplorer, QIcon::fromTheme("network-server"), "DBMS");
            leftTabs->setStyleSheet("QTabWidget::pane { border: none; }");

            // Editor
            editorHost = new EditorHost(this);
            editorHost->setProjectPath(projectPath);

            // AI Panes
            geminiPane = new GeminiPane(this);
            geminiPane->setProjectPath(QString::fromStdString(projectPath));
            geminiPane->setVisible(true);

            openaiPane = new OpenAIPane(this);
            openaiPane->setVisible(false);

            // Designer
            designerPane = new DesignerPane(this);
            designerPane->setVisible(false);

            // EAST PANE Stack (Right Side Tabs)
            eastTabs = new QTabWidget(this);
            eastTabs->addTab(geminiPane, QIcon::fromTheme("dialog-information"), "Gemini");
            eastTabs->addTab(openaiPane, QIcon::fromTheme("dialog-information"), "OpenAI");
            shellScriptingPane = new ShellScriptingPane(this);
            eastTabs->addTab(shellScriptingPane, QIcon::fromTheme("utilities-terminal"), "Shell Scripting");
            gitPane = new GitPane(this);
            gitPane->setProjectPath(QString::fromStdString(projectPath));
            eastTabs->addTab(gitPane, QIcon::fromTheme("vcs-branch"), "Git");

            connect(shellScriptingPane, &ShellScriptingPane::requestAIExplanation, [this](const QString& script) {
                eastTabs->setVisible(true);
                eastTabs->setCurrentWidget(geminiPane);
                geminiPane->ask("Explique este script shell em detalhes, passo a passo, identificando possíveis riscos:\n\n" + script);
            });

            horizontalSplitter->addWidget(leftTabs);
            horizontalSplitter->addWidget(editorHost);
            horizontalSplitter->addWidget(designerPane);
            horizontalSplitter->addWidget(eastTabs);

            horizontalSplitter->setStretchFactor(0, 0);
            horizontalSplitter->setStretchFactor(1, 1);
            horizontalSplitter->setStretchFactor(2, 1);
            horizontalSplitter->setStretchFactor(3, 0);
            horizontalSplitter->setSizes({250, 750, 0, 400});

            // Painel Inferior
            bottomTabs = new QTabWidget(this);
            bottomTabs->setTabPosition(QTabWidget::South);
            bottomTabs->setStyleSheet("QTabWidget::pane { border-top: 1px solid #333; }");

            QString projectPathQStr = QString::fromStdString(projectPath);
            googlePane = new BrowserPane("https://www.google.com", this);
            duckAiPane = new BrowserPane("https://duck.ai", this);
            braveLeoPane = new BrowserPane("https://search.brave.com/chat", this);
            chatGptPane = new BrowserPane("https://chatgpt.com", this);
            auto* explainShellPane = new BrowserPane("https://explainshell.com", this);
            terminalPane = new TerminalPane(this);
            inspectionPane = new InspectionPane(this);
            databasePane = new DatabasePane(this);
            logcatPane = new LogcatPane(this);
            versionsPane = new VersionsPane(projectPath, this);
            savesPane = new SavesPane(this);
            auto* ssAiPane = new SSAIConsolePane(this);

            bottomTabs->addTab(googlePane, QIcon::fromTheme("help-browser"), "Google");
            bottomTabs->addTab(duckAiPane, QIcon::fromTheme("help-browser"), "Duck.ai");
            bottomTabs->addTab(braveLeoPane, QIcon::fromTheme("help-browser"), "Brave Leo");
            bottomTabs->addTab(chatGptPane, QIcon::fromTheme("help-browser"), "ChatGPT");
            bottomTabs->addTab(explainShellPane, QIcon::fromTheme("help-about"), "ExplainShell");
            bottomTabs->addTab(terminalPane, QIcon::fromTheme("utilities-terminal"), "Terminal");
            bottomTabs->addTab(ssAiPane, QIcon::fromTheme("system-run"), "SS AI Agent");
            bottomTabs->addTab(inspectionPane, QIcon::fromTheme("system-search"), "Problemas");
            bottomTabs->addTab(databasePane, QIcon::fromTheme("network-server"), "Banco de Dados");
            bottomTabs->addTab(logcatPane, QIcon::fromTheme("utilities-log-viewer"), "Logs");
            bottomTabs->addTab(versionsPane, QIcon::fromTheme("document-revert"), "Versões");
            bottomTabs->addTab(savesPane, QIcon::fromTheme("document-save"), "Saves");

            bottomTabs->setVisible(false);

            verticalSplitter->addWidget(horizontalSplitter);
            verticalSplitter->addWidget(bottomTabs);
            verticalSplitter->setStretchFactor(0, 0);
            verticalSplitter->setStretchFactor(1, 1);

            vLayout->addWidget(verticalSplitter);

            // Conexões
            connect(header, &ProjectHeader::geminiToggled, [this]() {
                eastTabs->setVisible(!eastTabs->isVisible());
            });

            connect(header, &ProjectHeader::designerToggled, [this]() {
                bool visible = !designerPane->isVisible();
                designerPane->setVisible(visible);
                if (visible) {
                    horizontalSplitter->setSizes({250, 0, 1000, 400}); // Foca no designer
                } else {
                    horizontalSplitter->setSizes({250, 1000, 0, 400}); // Volta pro editor
                }
            });

            connect(header, &ProjectHeader::buildRequested, [this]() {
                bottomTabs->setVisible(true);
                bottomTabs->setCurrentWidget(terminalPane);
                terminalPane->logSystem("Iniciando Build + Backup do Projeto...");

                QProcess* buildProc = new QProcess(this);
                buildProc->setWorkingDirectory(QString::fromStdString(m_projectPath));

                connect(buildProc, &QProcess::readyReadStandardOutput, [this, buildProc]() {
                    terminalPane->appendOutput(buildProc->readAllStandardOutput());
                });
                connect(buildProc, &QProcess::readyReadStandardError, [this, buildProc]() {
                    terminalPane->appendError(buildProc->readAllStandardError());
                });

                buildProc->start("bash", QStringList() << "./build/build.sh");
            });

            connect(header, &ProjectHeader::exportRequested, [this]() {
                QMenu exportMenu(this);
                exportMenu.setStyleSheet("QMenu { background-color: #2b2d30; color: #bcbec4; } QMenu::item:selected { background-color: #8E44AD; }");

                auto* debAct = exportMenu.addAction("📦 Debian Package (.deb)");
                auto* rpmAct = exportMenu.addAction("📦 RPM Package (.rpm)");
                auto* appImageAct = exportMenu.addAction("📦 AppImage (.AppImage)");

                QAction* selected = exportMenu.exec(QCursor::pos());
                if (!selected) return;

                QString scriptName;
                if (selected == debAct) scriptName = "gen_deb_package.sh";
                else if (selected == rpmAct) scriptName = "gen_rpm_package.sh";
                else if (selected == appImageAct) scriptName = "gen_appimage.sh";

                if (!scriptName.isEmpty()) {
                    bottomTabs->setVisible(true);
                    bottomTabs->setCurrentWidget(terminalPane);
                    terminalPane->logSystem("Iniciando Exportação (" + scriptName + ")...");

                    QString scriptPath = QString::fromStdString(m_projectPath) + "/templates/shell/" + scriptName;

                    QProcess* exportProc = new QProcess(this);
                    exportProc->setWorkingDirectory(QString::fromStdString(m_projectPath));

                    connect(exportProc, &QProcess::readyReadStandardOutput, [this, exportProc]() {
                        terminalPane->appendOutput(exportProc->readAllStandardOutput());
                    });
                    connect(exportProc, &QProcess::readyReadStandardError, [this, exportProc]() {
                        terminalPane->appendError(exportProc->readAllStandardError());
                    });

                    exportProc->start("bash", QStringList() << scriptPath);
                }
            });

            connect(dbmsExplorer, &DBMSExplorer::databaseSelected, [this](const QString& path) {
                bottomTabs->setVisible(true);
                bottomTabs->setCurrentWidget(databasePane);
                verticalSplitter->setSizes({1, 10000});
                databasePane->openSpecificDB(path);
            });

            connect(header, &ProjectHeader::browserToggled, [this]() {
                bool isOpening = !bottomTabs->isVisible();
                bottomTabs->setVisible(isOpening);
                if (isOpening) {
                    bottomTabs->setCurrentWidget(googlePane);
                    verticalSplitter->setSizes({1, 10000});
                }
            });

            connect(editorHost, &EditorHost::requestBrowserUrl, [this](const QString& url) {
                chatGptPane->loadUrl(url);
                bottomTabs->setVisible(true);
                bottomTabs->setCurrentWidget(chatGptPane);
                verticalSplitter->setSizes({1, 10000});
            });

            connect(editorHost, &EditorHost::requestGoogleUrl, [this](const QString& url) {
                googlePane->loadUrl(url);
                bottomTabs->setVisible(true);
                bottomTabs->setCurrentWidget(googlePane);
                verticalSplitter->setSizes({1, 10000});
            });

            auto connectBrowser = [this](BrowserPane* pane) {
                connect(pane, &BrowserPane::scriptStarted, [this](const QString& cmd) {
                    bottomTabs->setVisible(true);
                    bottomTabs->setCurrentWidget(terminalPane);
                    verticalSplitter->setSizes({1, 10000});
                    terminalPane->logSystem("Iniciando script: " + cmd);
                });
                connect(pane, &BrowserPane::scriptOutput, terminalPane, &TerminalPane::appendOutput);
                connect(pane, &BrowserPane::scriptError, terminalPane, &TerminalPane::appendError);
                connect(pane, &BrowserPane::scriptFinished, [this](int code) {
                    terminalPane->logSystem(QString("Script finalizado com código: %1").arg(code));
                });
            };

            connectBrowser(googlePane);
            connectBrowser(duckAiPane);
            connectBrowser(braveLeoPane);
            connectBrowser(chatGptPane);
            connectBrowser(explainShellPane);

            connect(inspectionPane, &InspectionPane::openFileRequested, [this](const std::string& path, int line) {
                Q_UNUSED(line);
                editorHost->openFile(path);
                // TODO: Implementar scroll para a linha específica no editor
            });

            connect(savesPane, &SavesPane::previewRequested, [this](const QString& title, const QString& content) {
                editorHost->openPreview(title, content);
            });

            connect(editorHost, &EditorHost::requestAutonomousAI, [this](const QString& selection) {
                eastTabs->setVisible(true);
                eastTabs->setCurrentWidget(geminiPane);
                geminiPane->ask("Refatore este código de forma autônoma: \n\n" + selection +
                               "\n\nInstruções: Reescreva usando uma estrutura mais eficiente, melhore a lógica e aplique padrões modernos de C++. Retorne apenas o bloco de código corrigido.");
            });

            connect(treeView, &QTreeView::doubleClicked, [this](const QModelIndex& index) {
                QString path = model->filePath(index);
                QFileInfo info(path);
                if (info.isFile()) {
                    QString ext = info.suffix().toLower();
                    if (ext == "db" || ext == "sqlite" || ext == "sqlite3" || ext == "duckdb" || ext == "csv") {
                        bottomTabs->setVisible(true);
                        bottomTabs->setCurrentWidget(databasePane);
                        verticalSplitter->setSizes({1, 10000});
                        // Precisamos de um método para carregar o arquivo no DatabasePane
                        databasePane->openSpecificDB(path);
                    } else {
                        editorHost->openFile(path.toStdString());
                    }
                }
            });

            connect(treeView, &QTreeView::customContextMenuRequested, this, &ProjectWorkspace::showExplorerContextMenu);

            editorHost->restoreState();
        }

    private slots:
        void showExplorerContextMenu(const QPoint& pos) {
            QModelIndex index = treeView->indexAt(pos);

            QString targetDir;
            QString targetPath;
            if (index.isValid()) {
                targetPath = model->filePath(index);
                QFileInfo info(targetPath);
                targetDir = info.isDir() ? info.absoluteFilePath() : info.absolutePath();
            } else {
                targetDir = QString::fromStdString(m_projectPath);
                targetPath = targetDir;
            }

            QMenu menu(this);

            QAction* newFileAction = menu.addAction(QIcon::fromTheme("document-new"), "Novo Arquivo");
            QAction* newFolderAction = menu.addAction(QIcon::fromTheme("folder-new"), "Nova Pasta");
            menu.addSeparator();

            QAction* renameAction = nullptr;
            QAction* deleteAction = nullptr;
            QAction* copyPathAction = nullptr;
            QAction* openTerminalAction = nullptr;
            QAction* inspectAction = nullptr;
            QAction* localInspectAction = nullptr;
            QAction* restoreVersionAction = nullptr;
            QAction* propertiesAction = nullptr;

            // Novo menu de templates
            QMenu* templateMenu = menu.addMenu(QIcon::fromTheme("document-new"), "Novo Arquivo via Template");
            setupTemplateMenu(templateMenu, targetDir);
            menu.addSeparator();

            if (index.isValid()) {
                renameAction = menu.addAction(QIcon::fromTheme("edit-rename"), "Renomear");
                deleteAction = menu.addAction(QIcon::fromTheme("edit-delete"), "Deletar");
                menu.addSeparator();
                copyPathAction = menu.addAction(QIcon::fromTheme("edit-copy"), "Copiar Caminho");
                openTerminalAction = menu.addAction(QIcon::fromTheme("utilities-terminal"), "Abrir no Terminal");
                menu.addSeparator();
                inspectAction = menu.addAction(QIcon::fromTheme("system-search"), "✨ Code Inspector (AI)");
                localInspectAction = menu.addAction(QIcon::fromTheme("system-search"), "🔍 Inspect Code (Local)");

                QFileInfo info(targetPath);
                if (info.isFile()) {
                    restoreVersionAction = menu.addAction(QIcon::fromTheme("document-revert"), "Restaurar Versão Antiga...");
                }

                propertiesAction = menu.addAction(QIcon::fromTheme("document-properties"), "Propriedades");
            }

            QAction* selectedAction = menu.exec(treeView->mapToGlobal(pos));

            if (selectedAction == newFileAction) {
                bool ok;
                QString fileName = QInputDialog::getText(this, "Novo Arquivo", "Nome do arquivo:", QLineEdit::Normal, "", &ok);
                if (ok && !fileName.isEmpty()) {
                    QFile file(targetDir + "/" + fileName);
                    if (file.open(QIODevice::WriteOnly)) {
                        file.close();
                        // Atualiza o PDF de conhecimento
                        Service::PdfService pdf;
                        pdf.updateProjectPdf(m_projectPath);
                    } else {
                        QMessageBox::critical(this, "Erro", "Não foi possível criar o arquivo.");
                    }
                }
            } else if (selectedAction == newFolderAction) {
                bool ok;
                QString folderName = QInputDialog::getText(this, "Nova Pasta", "Nome da pasta:", QLineEdit::Normal, "", &ok);
                if (ok && !folderName.isEmpty()) {
                    QDir dir(targetDir);
                    if (!dir.mkdir(folderName)) {
                        QMessageBox::critical(this, "Erro", "Não foi possível criar a pasta.");
                    }
                }
            } else if (selectedAction == renameAction) {
                QFileInfo info(targetPath);
                bool ok;
                QString newName = QInputDialog::getText(this, "Renomear", "Novo nome:", QLineEdit::Normal, info.fileName(), &ok);
                if (ok && !newName.isEmpty()) {
                    model->setData(index, newName, Qt::EditRole);
                }
            } else if (selectedAction == deleteAction) {
                QFileInfo info(targetPath);
                auto result = QMessageBox::question(this, "Deletar", "Tem certeza que deseja deletar '" + info.fileName() + "'?",
                                                  QMessageBox::Yes | QMessageBox::No);
                if (result == QMessageBox::Yes) {
                    model->remove(index);
                    // Atualiza o PDF de conhecimento
                    Service::PdfService pdf;
                    pdf.updateProjectPdf(m_projectPath);
                }
            } else if (selectedAction == copyPathAction) {
                QApplication::clipboard()->setText(targetPath);
            } else if (selectedAction == openTerminalAction) {
                QProcess::startDetached("gnome-terminal", QStringList() << "--working-directory=" + targetDir);
            } else if (selectedAction == inspectAction) {
                QFileInfo info(targetPath);
                if (info.isFile()) {
                    QFile file(targetPath);
                    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        QString content = file.read(8000); // Envia os primeiros 8kb para análise
                        file.close();

                        eastTabs->setVisible(true);
                        eastTabs->setCurrentWidget(geminiPane);
                        geminiPane->setContext(info.fileName(), content);
                        geminiPane->ask("Realize uma inspeção profunda neste código. Procure por bugs, vulnerabilidades de segurança, problemas de lógica e sugira melhorias de performance e legibilidade seguindo as melhores práticas de C++.");
                    }
                } else {
                    QMessageBox::information(this, "✨ Code Inspector", "Por favor, selecione um arquivo individual para inspeção automática da IA.");
                }
            } else if (selectedAction == localInspectAction) {
                if (index.isValid() || !targetPath.isEmpty()) {
                    bottomTabs->setVisible(true);
                    bottomTabs->setCurrentWidget(inspectionPane);
                    verticalSplitter->setSizes({1, 10000});
                    inspectionPane->inspectPath(targetPath);
                }
            } else if (selectedAction == restoreVersionAction) {
                showRestoreVersionDialog(targetPath);
            } else if (selectedAction == propertiesAction) {
                QFileInfo info(targetPath);
                QString props = QString("<b>Nome:</b> %1<br>"
                                        "<b>Caminho:</b> %2<br>"
                                        "<b>Tamanho:</b> %3 bytes<br>"
                                        "<b>Criado em:</b> %4<br>"
                                        "<b>Modificado em:</b> %5")
                                        .arg(info.fileName())
                                        .arg(info.absoluteFilePath())
                                        .arg(info.size())
                                        .arg(info.birthTime().toString())
                                        .arg(info.lastModified().toString());

                QMessageBox::information(this, "Propriedades", props);
            }
        }

        void showRestoreVersionDialog(const QString& filePath) {
            auto history = Service::BackupService::getInstance().getFileHistory(filePath);
            if (history.isEmpty()) {
                QMessageBox::information(this, "Restauração", "Nenhuma versão anterior encontrada para este arquivo.");
                return;
            }

            QMenu versionMenu(this);
            versionMenu.setStyleSheet("QMenu { background-color: #2b2d30; color: #bcbec4; } QMenu::item:selected { background-color: #0e639c; }");

            for (const auto& version : history) {
                QString label = QString("[%1] %2").arg(version.timestamp, version.summary);
                QMenu* sub = versionMenu.addMenu(label);

                auto* compareAct = sub->addAction("🔍 Comparar (Diff)");
                auto* restoreAct = sub->addAction("🔄 Restaurar esta versão");

                connect(compareAct, &QAction::triggered, [this, version, filePath]() {
                    QFile file(filePath);
                    if (file.open(QIODevice::ReadOnly)) {
                        QString currentContent = QString::fromUtf8(file.readAll());
                        file.close();
                        QString oldContent = QString::fromUtf8(Service::BackupService::getInstance().getSaveContent(version.id));

                        DiffDialog diag(QFileInfo(filePath).fileName(), currentContent, oldContent, this);
                        diag.exec();
                    }
                });

                connect(restoreAct, &QAction::triggered, [this, version, filePath]() {
                    auto result = QMessageBox::question(this, "Confirmar Restauração",
                        "Deseja restaurar o arquivo para a versão de " + version.timestamp + "?\n\n"
                        "A versão atual será sobrescrita.",
                        QMessageBox::Yes | QMessageBox::No);

                    if (result == QMessageBox::Yes) {
                        if (Service::BackupService::getInstance().restoreFileFromSave(version.id, filePath)) {
                            QMessageBox::information(this, "Sucesso", "Arquivo restaurado com sucesso!");
                            editorHost->openFile(filePath.toStdString());
                        }
                    }
                });
            }
            versionMenu.exec(QCursor::pos());
        }

        void setupTemplateMenu(QMenu* menu, const QString& targetDir) {
            QString templateRoot = QString::fromStdString(m_projectPath) + "/templates";
            QDir rootDir(templateRoot);

            for (const QString& category : rootDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
                QMenu* catMenu = menu->addMenu(category.toUpper());
                QDir catDir(templateRoot + "/" + category);

                for (const QString& tplFile : catDir.entryList(QDir::Files)) {
                    QAction* tplAct = catMenu->addAction(tplFile);
                    QString fullTplPath = catDir.absoluteFilePath(tplFile);

                    connect(tplAct, &QAction::triggered, [this, fullTplPath, targetDir, tplFile]() {
                        bool ok;
                        QString newName = QInputDialog::getText(this, "Novo Arquivo via Template",
                                                              "Nome do novo arquivo:", QLineEdit::Normal, tplFile, &ok);
                        if (ok && !newName.isEmpty()) {
                            QString destPath = targetDir + "/" + newName;
                            if (QFile::copy(fullTplPath, destPath)) {
                                QFile::setPermissions(destPath, QFile::ReadOwner | QFile::WriteOwner | QFile::ReadUser | QFile::WriteUser);
                                editorHost->openFile(destPath.toStdString());
                            } else {
                                QMessageBox::critical(this, "Erro", "Não foi possível criar o arquivo a partir do template.");
                            }
                        }
                    });
                }
            }
        }

    private:
        std::string m_projectPath;
        ProjectHeader* header;
        QTreeView* treeView;
        QFileSystemModel* model;
        QTabWidget* leftTabs;
        DBMSExplorer* dbmsExplorer;
        QSplitter* verticalSplitter;
        QSplitter* horizontalSplitter;
        EditorHost* editorHost;
        GeminiPane* geminiPane;
        OpenAIPane* openaiPane;
        ShellScriptingPane* shellScriptingPane;
        GitPane* gitPane;
        QTabWidget* eastTabs;
        DesignerPane* designerPane;
        QTabWidget* bottomTabs;
        BrowserPane* googlePane;
        BrowserPane* duckAiPane;
        BrowserPane* braveLeoPane;
        BrowserPane* chatGptPane;
        BrowserPane* explainShellPane;
        TerminalPane* terminalPane;
        InspectionPane* inspectionPane;
        DatabasePane* databasePane;
        LogcatPane* logcatPane;
        VersionsPane* versionsPane;
        SavesPane* savesPane;
        CustomIconProvider* iconProvider;
    };
}
#endif

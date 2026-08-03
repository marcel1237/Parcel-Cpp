#ifndef EDITOR_HOST_HPP
#define EDITOR_HOST_HPP

#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <QTabWidget>
#include <QToolBar>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QFileInfo>
#include "EditorPane.hpp"
#include "HexEditorPane.hpp"
#include "PdfViewerPane.hpp"
#include "PdfComposerPane.hpp"
#include "../../service/FileSystemService.hpp"
#include "../../service/DialogService.hpp"
#include "../../service/PersistenceService.hpp"
#include "../../service/PdfService.hpp"
#include "../../service/BackupService.hpp"

namespace Parcel::View {
    class EditorHost : public QWidget {
        Q_OBJECT
    public:
        EditorHost(QWidget* parent = nullptr) : QWidget(parent) {
            auto* layout = new QVBoxLayout(this);
            layout->setContentsMargins(0, 0, 0, 0);
            layout->setSpacing(0);

            setupToolBar();

            tabPane = new QTabWidget(this);
            tabPane->setTabsClosable(true);
            connect(tabPane, &QTabWidget::tabCloseRequested, this, &EditorHost::closeTab);

            layout->addWidget(tabPane);
        }

        void setProjectPath(const std::string& path) {
            m_projectPath = path;
        }

        void openFile(const std::string& filePath) {
            if (filePath.empty()) return;

            QString qFilePath = QString::fromStdString(filePath);
            QFileInfo info(qFilePath);
            if (!info.exists() || !info.isFile()) return;

            QString fileName = info.fileName();

            for (int i = 0; i < tabPane->count(); ++i) {
                if (tabPane->tabText(i) == fileName) {
                    tabPane->setCurrentIndex(i);
                    return;
                }
            }

            if (fileName.endsWith(".pdf", Qt::CaseInsensitive)) {
                auto* newEditor = new PdfViewerPane(qFilePath);
                int index = tabPane->addTab(newEditor, QIcon::fromTheme("document-print"), fileName);
                tabPane->setCurrentIndex(index);
            } else if (isBinaryFile(filePath)) {
                QFile file(qFilePath);
                if (file.open(QIODevice::ReadOnly)) {
                    QByteArray data = file.readAll();
                    auto* newEditor = new HexEditorPane(filePath, data);
                    int index = tabPane->addTab(newEditor, QIcon::fromTheme("application-x-executable"), fileName);
                    tabPane->setCurrentIndex(index);
                }
            } else {
                std::string content = readFile(filePath);
                auto* newEditor = new EditorPane(filePath, content);
                int index = tabPane->addTab(newEditor, QIcon::fromTheme("text-x-generic"), fileName);
                tabPane->setCurrentIndex(index);
                connect(newEditor, &EditorPane::requestBrowserUrl, this, &EditorHost::requestBrowserUrl);
                connect(newEditor, &EditorPane::requestGoogleUrl, this, &EditorHost::handleAutonomousAI);
            }

            saveState();
        }

        void openPreview(const QString& title, const QString& content) {
            for (int i = 0; i < tabPane->count(); ++i) {
                if (tabPane->tabText(i) == title) {
                    tabPane->setCurrentIndex(i);
                    return;
                }
            }
            auto* newEditor = new EditorPane("", content.toStdString());
            int index = tabPane->addTab(newEditor, QIcon::fromTheme("document-import"), title);
            tabPane->setCurrentIndex(index);
        }

        void handleAutonomousAI(const QString& data) {
            if (data.startsWith("REF_AUTO:")) {
                QString selection = data.mid(9);
                emit requestAutonomousAI(selection);
            } else if (data.startsWith("QUICK_FIX:")) {
                QString lineText = data.mid(10);
                emit requestAutonomousAI("CORRIJA ESTA LINHA ESPECÍFICA (QUICK FIX):\n\n" + lineText +
                                       "\n\nInstruções: Identifique o erro, corrija-o mantendo a compatibilidade com o resto do código e retorne apenas a linha corrigida ou o bloco mínimo necessário.");
            } else {
                emit requestGoogleUrl(data);
            }
        }

        void restoreState() {
            Service::PersistenceService persistence;
            auto files = persistence.getOpenFiles();
            for (const auto& path : files) {
                openFile(path);
            }
        }

    private slots:
        void handleSave() {
            QWidget* current = tabPane->currentWidget();
            if (auto* editor = dynamic_cast<EditorPane*>(current)) {
                try {
                    Service::FileSystemService fs;
                    std::string path = editor->getFile().string();
                    std::string content = editor->getText();
                    fs.saveFile(path, content);

                    Service::BackupService::getInstance().performSaveLog(
                        QString::fromStdString(path),
                        QByteArray::fromStdString(content)
                    );

                    Service::PdfService pdf;
                    pdf.updateProjectPdf(m_projectPath);
                } catch (const std::exception& e) {
                    Service::DialogService::showError(e.what());
                }
            } else if (auto* composer = dynamic_cast<PdfComposerPane*>(current)) {
                (void)composer;
            } else if (auto* hexEditor = dynamic_cast<HexEditorPane*>(current)) {
                try {
                    Service::FileSystemService fs;
                    QByteArray data = hexEditor->getData();
                    std::vector<char> vecData(data.begin(), data.end());
                    fs.saveBinaryFile(hexEditor->getFile().string(), vecData);
                } catch (const std::exception& e) {
                    Service::DialogService::showError(e.what());
                }
            }
        }

        void handleSaveAs() {
            QWidget* current = tabPane->currentWidget();
            std::string path;
            std::string content;
            QByteArray binaryData;
            bool isBinary = false;

            if (auto* editor = dynamic_cast<EditorPane*>(current)) {
                path = editor->getFile().string();
                content = editor->getText();
            } else if (auto* hexEditor = dynamic_cast<HexEditorPane*>(current)) {
                path = hexEditor->getFile().string();
                binaryData = hexEditor->getData();
                isBinary = true;
            } else {
                return;
            }

            QString newPath = QFileDialog::getSaveFileName(this, "Save As...", QString::fromStdString(path));
            if (!newPath.isEmpty()) {
                try {
                    Service::FileSystemService fs;
                    if (isBinary) {
                        std::vector<char> vecData(binaryData.begin(), binaryData.end());
                        fs.saveBinaryFile(newPath.toStdString(), vecData);
                    } else {
                        fs.saveFile(newPath.toStdString(), content);
                        Service::BackupService::getInstance().performSaveLog(
                            newPath,
                            QByteArray::fromStdString(content)
                        );
                        Service::PdfService pdf;
                        pdf.updateProjectPdf(m_projectPath);
                    }
                    tabPane->setTabText(tabPane->currentIndex(), QFileInfo(newPath).fileName());
                    saveState();
                } catch (const std::exception& e) {
                    Service::DialogService::showError(e.what());
                }
            }
        }

        void closeTab(int index) {
            QWidget* widget = tabPane->widget(index);
            tabPane->removeTab(index);
            delete widget;
            saveState();
        }

    signals:
        void requestBrowserUrl(const QString& url);
        void requestGoogleUrl(const QString& url);
        void requestAutonomousAI(const QString& selection);

    private:
        QTabWidget* tabPane;
        QToolBar* toolBar;
        std::string m_projectPath;

        bool isBinaryFile(const std::string& path) {
            std::ifstream file(path, std::ios::binary);
            if (!file) return false;
            char buffer[1024];
            file.read(buffer, sizeof(buffer));
            std::streamsize bytesRead = file.gcount();
            for (int i = 0; i < bytesRead; ++i) {
                if (buffer[i] == '\0') return true;
            }
            return false;
        }

        void saveState() {
            std::vector<std::string> filePaths;
            for (int i = 0; i < tabPane->count(); ++i) {
                QWidget* widget = tabPane->widget(i);
                if (auto* pane = dynamic_cast<EditorPane*>(widget)) {
                    filePaths.push_back(pane->getFile().string());
                } else if (auto* hexPane = dynamic_cast<HexEditorPane*>(widget)) {
                    filePaths.push_back(hexPane->getFile().string());
                } else if (auto* pdfPane = dynamic_cast<PdfViewerPane*>(widget)) {
                    filePaths.push_back(pdfPane->getFilePath().toStdString());
                } else if (auto* composerPane = dynamic_cast<PdfComposerPane*>(widget)) {
                    if (!composerPane->getFile().empty()) {
                        filePaths.push_back(composerPane->getFile().string());
                    }
                }
            }
            Service::PersistenceService persistence;
            persistence.saveOpenFiles(filePaths);
        }

        void setupToolBar() {
            toolBar = new QToolBar(this);
            toolBar->setIconSize(QSize(20, 20));
            toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            toolBar->setStyleSheet("QToolBar { spacing: 10px; padding: 5px; background: #333; } "
                                 "QToolButton { color: white; padding: 4px; border-radius: 4px; } "
                                 "QToolButton:hover { background: #444; }");

            QIcon saveIcon = QIcon::fromTheme("media-floppy", QIcon::fromTheme("document-save"));
            QIcon saveAsIcon = QIcon::fromTheme("document-save-all", QIcon::fromTheme("document-save-as", QIcon::fromTheme("media-floppy")));
            QIcon newPdfIcon = QIcon::fromTheme("document-new", QIcon::fromTheme("list-add"));

            auto* saveAction = toolBar->addAction(saveIcon, "Save");
            auto* saveAsAction = toolBar->addAction(saveAsIcon, "Save As...");
            auto* newPdfAction = toolBar->addAction(newPdfIcon, "New PDF Document");

            connect(saveAction, &QAction::triggered, this, &EditorHost::handleSave);
            connect(saveAsAction, &QAction::triggered, this, &EditorHost::handleSaveAs);
            connect(newPdfAction, &QAction::triggered, this, &EditorHost::handleNewPdf);

            layout()->addWidget(toolBar);
        }

        void handleNewPdf() {
            auto* composer = new PdfComposerPane("", this);
            int index = tabPane->addTab(composer, QIcon::fromTheme("document-new"), "Novo PDF");
            tabPane->setCurrentIndex(index);
        }

        std::string readFile(const std::string& path) {
            std::ifstream file(path);
            if (!file.is_open()) return "";
            std::stringstream buffer;
            buffer << file.rdbuf();
            return buffer.str();
        }
    };
}
#endif

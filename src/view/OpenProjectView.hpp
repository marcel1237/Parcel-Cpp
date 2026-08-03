#ifndef OPEN_PROJECT_VIEW_HPP
#define OPEN_PROJECT_VIEW_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QTextEdit>
#include <filesystem>
#include <set>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <regex>
#include "../core/navigation/NavigationController.hpp"
#include "../service/GitService.hpp"

namespace Parcel::View {
    class OpenProjectView : public QWidget {
        Q_OBJECT
    public:
        explicit OpenProjectView(QWidget *parent = nullptr) : QWidget(parent) {
            auto* layout = new QVBoxLayout(this);
            layout->setContentsMargins(20, 20, 20, 20);
            layout->setSpacing(20);

            auto* title = new QLabel("Open Project", this);
            title->setStyleSheet("font-size: 26px; font-weight: bold;");
            layout->addWidget(title);

            auto* pathLayout = new QHBoxLayout();
            pathField = new QLineEdit(this);
            pathField->setPlaceholderText("Choose a project folder...");
            pathField->setReadOnly(true);

            auto* browseBtn = new QPushButton("Browse...", this);
            browseBtn->setIcon(QIcon::fromTheme("folder-open"));
            pathLayout->addWidget(pathField);
            pathLayout->addWidget(browseBtn);
            layout->addLayout(pathLayout);

            infoArea = new QTextEdit(this);
            infoArea->setReadOnly(true);
            infoArea->setPlaceholderText("Choose a folder to continue.");
            layout->addWidget(infoArea);

            auto* buttonLayout = new QHBoxLayout();
            openBtn = new QPushButton("Open", this);
            openBtn->setIcon(QIcon::fromTheme("document-open"));
            openBtn->setEnabled(false);
            auto* cancelBtn = new QPushButton("Cancel", this);
            cancelBtn->setIcon(QIcon::fromTheme("process-stop"));
            buttonLayout->addWidget(openBtn);
            buttonLayout->addWidget(cancelBtn);
            buttonLayout->addStretch();
            layout->addLayout(buttonLayout);

            connect(browseBtn, &QPushButton::clicked, this, &OpenProjectView::handleBrowse);
            connect(openBtn, &QPushButton::clicked, this, &OpenProjectView::handleOpen);
            connect(cancelBtn, &QPushButton::clicked, this, &OpenProjectView::handleCancel);
        }

    private slots:
        void handleBrowse() {
            QString dir = QFileDialog::getExistingDirectory(this, "Open Project Folder", "");
            if (!dir.isEmpty()) {
                selectedPath = dir;
                pathField->setText(dir);
                analyzeProject(dir);
                openBtn->setEnabled(true);
            }
        }

        void analyzeProject(const QString& path) {
            infoArea->clear();
            std::filesystem::path projectPath(path.toStdString());

            if (!std::filesystem::exists(projectPath)) return;

            QString report = "<b>🔍 Análise do Projeto</b><br><br>";
            report += "<b>Caminho:</b> " + path + "<br>";

            // 1. Detecção de Git
            Service::GitService git(path.toStdString());
            if (git.isGitRepository()) {
                report += "<b>🌿 Repositório Git:</b> Ativo<br>";
                report += "<b>🔀 Branch Atual:</b> " + QString::fromStdString(git.getCurrentBranch()) + "<br>";
            } else {
                report += "<b>🌿 Repositório Git:</b> Não detectado<br>";
            }

            // 2. Escaneamento de Conteúdo
            std::set<std::string> languages;
            std::set<std::string> buildSystems;
            long long totalSize = 0;
            int fileCount = 0;
            std::filesystem::file_time_type lastMod;
            bool firstFile = true;

            try {
                for (const auto& entry : std::filesystem::recursive_directory_iterator(projectPath)) {
                    if (entry.is_regular_file()) {
                        fileCount++;
                        totalSize += entry.file_size();

                        auto fTime = entry.last_write_time();
                        if (firstFile || fTime > lastMod) {
                            lastMod = fTime;
                            firstFile = false;
                        }

                        std::string ext = entry.path().extension().string();
                        std::string fileName = entry.path().filename().string();

                        // Linguagens
                        if (ext == ".cpp" || ext == ".hpp" || ext == ".h" || ext == ".cc") languages.insert("C++");
                        else if (ext == ".java") languages.insert("Java");
                        else if (ext == ".py") languages.insert("Python");
                        else if (ext == ".js" || ext == ".ts") languages.insert("JavaScript/TypeScript");
                        else if (ext == ".sh") languages.insert("Shell Script");

                        // Build Systems
                        if (fileName == "CMakeLists.txt") buildSystems.insert("CMake");
                        else if (fileName == "pom.xml") buildSystems.insert("Maven");
                        else if (fileName == "build.gradle") buildSystems.insert("Gradle");
                        else if (fileName == "Makefile") buildSystems.insert("Makefile");
                        else if (fileName == "package.json") buildSystems.insert("NPM/Node");
                    }
                }
            } catch (...) {}

            // 3. Formatação do Relatório
            if (!languages.empty()) {
                report += "<b>💻 Linguagens:</b> ";
                for (const auto& lang : languages) report += QString::fromStdString(lang) + ", ";
                report.chop(2); report += "<br>";
            }

            if (!buildSystems.empty()) {
                report += "<b>🛠️ Build System:</b> ";
                for (const auto& bs : buildSystems) report += QString::fromStdString(bs) + ", ";
                report.chop(2); report += "<br>";

                // Detalhes extras do Build System
                report += extractBuildDetails(projectPath);
            }

            report += "<b>📂 Estatísticas:</b> " + QString::number(fileCount) + " arquivos (" + formatSize(totalSize) + ")<br>";

            if (!firstFile) {
                auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(lastMod - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
                std::time_t tt = std::chrono::system_clock::to_time_t(sctp);
                report += "<b>🕒 Última Modificação:</b> " + QString::fromStdString(std::ctime(&tt));
            }

            report += "<br>🚀 <i>Clique em 'Open' para carregar o Workspace.</i>";
            infoArea->setHtml(report);
        }

        QString extractBuildDetails(const std::filesystem::path& projectPath) {
            QString details = "";

            // CMake
            if (std::filesystem::exists(projectPath / "CMakeLists.txt")) {
                std::ifstream file(projectPath / "CMakeLists.txt");
                std::string line;
                details += "   ▫️ <i>CMake:</i> ";
                while (std::getline(file, line)) {
                    if (line.find("project(") != std::string::npos) {
                        std::regex re("project\\s*\\(\\s*\"?([^\")\\s]+)\"?");
                        std::smatch match;
                        if (std::regex_search(line, match, re)) {
                            details += "Nome: " + QString::fromStdString(match[1]) + " | ";
                        }
                    }
                    if (line.find("cmake_minimum_required") != std::string::npos) {
                        std::regex re("VERSION\\s+([0-9.]+)");
                        std::smatch match;
                        if (std::regex_search(line, match, re)) {
                            details += "Ver: " + QString::fromStdString(match[1]) + " | ";
                        }
                    }
                }
                if (details.endsWith(" | ")) details.chop(3);
                details += "<br>";
            }

            // Maven
            if (std::filesystem::exists(projectPath / "pom.xml")) {
                std::ifstream file(projectPath / "pom.xml");
                std::string line;
                QString artifactId = "", version = "";
                while (std::getline(file, line)) {
                    if (artifactId.isEmpty() && line.find("<artifactId>") != std::string::npos) {
                        std::regex re("<artifactId>(.+)</artifactId>");
                        std::smatch match;
                        if (std::regex_search(line, match, re)) artifactId = QString::fromStdString(match[1]);
                    }
                    if (version.isEmpty() && line.find("<version>") != std::string::npos) {
                        std::regex re("<version>(.+)</version>");
                        std::smatch match;
                        if (std::regex_search(line, match, re)) version = QString::fromStdString(match[1]);
                    }
                }
                details += "   ▫️ <i>Maven:</i> " + artifactId + " (v" + version + ")<br>";
            }

            // NPM
            if (std::filesystem::exists(projectPath / "package.json")) {
                std::ifstream file(projectPath / "package.json");
                std::string line;
                QString name = "", version = "";
                while (std::getline(file, line)) {
                    if (name.isEmpty() && line.find("\"name\":") != std::string::npos) {
                        std::regex re("\"name\"\\s*:\\s*\"([^\"]+)\"");
                        std::smatch match;
                        if (std::regex_search(line, match, re)) name = QString::fromStdString(match[1]);
                    }
                    if (version.isEmpty() && line.find("\"version\":") != std::string::npos) {
                        std::regex re("\"version\"\\s*:\\s*\"([^\"]+)\"");
                        std::smatch match;
                        if (std::regex_search(line, match, re)) version = QString::fromStdString(match[1]);
                    }
                }
                details += "   ▫️ <i>NPM:</i> " + name + " (v" + version + ")<br>";
            }

            // Extração de Dependências Principais
            QString deps = extractDependencies(projectPath);
            if (!deps.isEmpty()) {
                details += "<b>📦 Dependências Detectadas:</b><br>" + deps;
            }

            // Informações de IDEs/Arquivos de Projeto
            details += extractIDEProjectDetails(projectPath);

            return details;
        }

        QString extractIDEProjectDetails(const std::filesystem::path& projectPath) {
            QString details = "";

            // KDevelop (.kdev4)
            for (const auto& entry : std::filesystem::directory_iterator(projectPath)) {
                if (entry.path().extension() == ".kdev4") {
                    std::ifstream file(entry.path());
                    std::string line;
                    details += "   ▫️ <i>KDevelop:</i> ";
                    while (std::getline(file, line)) {
                        if (line.find("Name=") == 0) {
                            details += "Nome: " + QString::fromStdString(line.substr(5)) + " | ";
                        }
                    }
                    if (details.endsWith(" | ")) details.chop(3);
                    details += "<br>";
                }
            }

            // Qt Creator (.pro)
            if (std::filesystem::exists(projectPath)) {
                for (const auto& entry : std::filesystem::directory_iterator(projectPath)) {
                    if (entry.path().extension() == ".pro") {
                        details += "   ▫️ <i>Qt Project:</i> " + QString::fromStdString(entry.path().filename().string()) + "<br>";
                    }
                }
            }

            // Android Studio / IntelliJ (.idea)
            if (std::filesystem::exists(projectPath / ".idea")) {
                details += "   ▫️ <i>IntelliJ/Android Studio:</i> Pasta .idea detectada<br>";
            }

            // VS Code (.vscode)
            if (std::filesystem::exists(projectPath / ".vscode")) {
                details += "   ▫️ <i>VS Code:</i> Pasta .vscode detectada<br>";
            }

            if (!details.isEmpty()) {
                details = "<b>🖥️ Arquivos de Projeto/IDEs:</b><br>" + details;
            }

            return details;
        }

        QString extractDependencies(const std::filesystem::path& projectPath) {
            QString deps = "";
            std::set<QString> uniqueDeps;

            // CMake Dependencies (find_package)
            if (std::filesystem::exists(projectPath / "CMakeLists.txt")) {
                std::ifstream file(projectPath / "CMakeLists.txt");
                std::string line;
                while (std::getline(file, line)) {
                    std::regex re("find_package\\s*\\(\\s*([A-Za-z0-9_]+)");
                    std::smatch match;
                    if (std::regex_search(line, match, re)) {
                        uniqueDeps.insert(QString::fromStdString(match[1]));
                    }
                }
            }

            // Maven Dependencies (artifactId)
            if (std::filesystem::exists(projectPath / "pom.xml")) {
                std::ifstream file(projectPath / "pom.xml");
                std::string line;
                while (std::getline(file, line)) {
                    std::regex re("<artifactId>(.+)</artifactId>");
                    std::smatch match;
                    if (std::regex_search(line, match, re)) {
                        QString name = QString::fromStdString(match[1]);
                        // Ignora o próprio projeto se capturado por engano
                        uniqueDeps.insert(name);
                    }
                }
            }

            // NPM Dependencies
            if (std::filesystem::exists(projectPath / "package.json")) {
                std::ifstream file(projectPath / "package.json");
                std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

                std::regex re("\"dependencies\"\\s*:\\s*\\{([^\\}]+)\\}");
                std::smatch match;
                if (std::regex_search(content, match, re)) {
                    std::string depsBlock = match[1];
                    std::regex depRe("\"([^\"]+)\"\\s*:");
                    auto it = std::sregex_iterator(depsBlock.begin(), depsBlock.end(), depRe);
                    auto end = std::sregex_iterator();
                    for (; it != end; ++it) uniqueDeps.insert(QString::fromStdString((*it)[1]));
                }
            }

            if (!uniqueDeps.empty()) {
                deps += "   <small>";
                int count = 0;
                for (const auto& d : uniqueDeps) {
                    deps += d + ", ";
                    if (++count > 15) { deps += "..."; break; } // Limite visual
                }
                if (deps.endsWith(", ")) deps.chop(2);
                deps += "</small><br>";
            }

            return deps;
        }

        QString formatSize(long long bytes) {
            QStringList units = {"B", "KB", "MB", "GB", "TB"};
            double size = bytes;
            int i = 0;
            while (size >= 1024 && i < units.size() - 1) {
                size /= 1024;
                i++;
            }
            return QString::number(size, 'f', 2) + " " + units[i];
        }

        void handleOpen() {
            if (!selectedPath.isEmpty()) {
                Core::Navigation::NavigationController::getInstance().navigateTo(
                    Core::Navigation::NavigationTarget::WORKSPACE, selectedPath.toStdString());
            }
        }

        void handleCancel() {
            pathField->clear();
            selectedPath.clear();
            infoArea->clear();
            openBtn->setEnabled(false);
        }

    private:
        QLineEdit* pathField;
        QTextEdit* infoArea;
        QPushButton* openBtn;
        QString selectedPath;
    };
}
#endif

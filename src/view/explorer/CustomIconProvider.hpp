#ifndef CUSTOM_ICON_PROVIDER_HPP
#define CUSTOM_ICON_PROVIDER_HPP

#include <QFileIconProvider>
#include <QIcon>
#include <QFileInfo>

namespace Parcel::View {

    class CustomIconProvider : public QFileIconProvider {
    public:
        QIcon icon(const QFileInfo &info) const override {
            if (info.isDir()) {
                QString name = info.fileName().toLower();

                // Pastas Especiais com ícones bem distintos
                if (name == "src") {
                    return QIcon::fromTheme("folder-blue", QIcon::fromTheme("folder-source", QFileIconProvider::icon(QFileIconProvider::Folder)));
                }
                if (name == "test") {
                    return QIcon::fromTheme("folder-green", QIcon::fromTheme("folder-development", QFileIconProvider::icon(QFileIconProvider::Folder)));
                }
                if (name == "resources" || name == "res") {
                    return QIcon::fromTheme("folder-yellow", QIcon::fromTheme("folder-documents", QFileIconProvider::icon(QFileIconProvider::Folder)));
                }
                if (name == ".git" || name == "git") {
                    return QIcon::fromTheme("folder-red", QIcon::fromTheme("folder-git", QFileIconProvider::icon(QFileIconProvider::Folder)));
                }
                if (name == "build" || name == "target" || name == "bin" || name == "out") {
                    return QIcon::fromTheme("folder-grey", QIcon::fromTheme("folder-build", QFileIconProvider::icon(QFileIconProvider::Folder)));
                }

                return QFileIconProvider::icon(info);
            }

            // Arquivos com ícones bem diferenciados por tipo
            QString ext = info.suffix().toLower();
            if (ext == "java") {
                return QIcon::fromTheme("text-x-java", QIcon::fromTheme("x-office-document", QFileIconProvider::icon(QFileIconProvider::File)));
            }
            if (ext == "cpp" || ext == "hpp" || ext == "h" || ext == "c" || ext == "cc") {
                return QIcon::fromTheme("text-x-c++src", QIcon::fromTheme("text-x-c++", QFileIconProvider::icon(QFileIconProvider::File)));
            }
            if (ext == "xml" || ext == "fxml") {
                return QIcon::fromTheme("text-xml", QIcon::fromTheme("text-html", QFileIconProvider::icon(QFileIconProvider::File)));
            }
            if (ext == "md" || ext == "txt") {
                return QIcon::fromTheme("text-markdown", QIcon::fromTheme("text-x-generic", QFileIconProvider::icon(QFileIconProvider::File)));
            }
            if (ext == "sh" || ext == "bash" || ext == "bat") {
                return QIcon::fromTheme("utilities-terminal", QIcon::fromTheme("text-x-script", QFileIconProvider::icon(QFileIconProvider::File)));
            }
            if (ext == "json" || ext == "yaml" || ext == "yml") {
                return QIcon::fromTheme("application-json", QIcon::fromTheme("text-x-generic", QFileIconProvider::icon(QFileIconProvider::File)));
            }
            if (ext == "css" || ext == "scss") {
                return QIcon::fromTheme("text-css", QIcon::fromTheme("applications-graphics", QFileIconProvider::icon(QFileIconProvider::File)));
            }
            if (ext == "db" || ext == "sqlite" || ext == "sqlite3" || ext == "duckdb") {
                return QIcon::fromTheme("network-server", QIcon::fromTheme("database", QFileIconProvider::icon(QFileIconProvider::File)));
            }
            if (ext == "csv") {
                return QIcon::fromTheme("x-office-spreadsheet", QIcon::fromTheme("text-x-generic", QFileIconProvider::icon(QFileIconProvider::File)));
            }

            return QFileIconProvider::icon(info);
        }
    };
}

#endif

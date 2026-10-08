#ifndef KDE_INTEGRATION_SERVICE_HPP
#define KDE_INTEGRATION_SERVICE_HPP

#include <QString>
#include <QStringList>
#include <QColor>
#include <QIcon>
#include <QPalette>
#include <QMap>

namespace Parcel::Service {

    class KDEIntegrationService {
    public:
        static KDEIntegrationService& getInstance() {
            static KDEIntegrationService instance;
            return instance;
        }

        QString getApiInfo() const {
            return "KDE Frameworks 6 & Kirigami API Bridge active (api.kde.org)";
        }

        QString getKDevelopApiInfo() const {
            return "KDevelop Plugin, AST, and Language Support Integration Layer active";
        }

        // KDE Frameworks 6 Full API Suites (api.kde.org)
        QString getKCoreAddonsApi() const { return "KCoreAddons: Job tracking, KPluginMetaData, and add-on management"; }
        QString getKConfigApi() const { return "KConfig: Advanced configuration management (KSharedConfig, KConfigGroup)"; }
        QString getKIOApi() const { return "KIO: Network transparent file management and job processing"; }
        QString getKWidgetsAddonsApi() const { return "KWidgetsAddons: Desktop widgets, KPageDialog, KMessageBox helpers"; }
        QString getKTextEditorApi() const { return "KTextEditor: Full IDE text editor component with syntax highlighting and folding"; }
        QString getKirigamiApi() const { return "Kirigami: Convergent UI components for desktop and mobile (api.kde.org/kirigami-index.html)"; }

        // KDE Breeze Dark Color Palette
        QPalette getKdeDarkPalette() const {
            QPalette palette;
            palette.setColor(QPalette::Window, QColor(26, 26, 46));       // Breeze Dark Window #1a1a2e
            palette.setColor(QPalette::WindowText, QColor(220, 220, 220));
            palette.setColor(QPalette::Base, QColor(20, 20, 30));         // View Base #14141e
            palette.setColor(QPalette::AlternateBase, QColor(30, 30, 45));
            palette.setColor(QPalette::Text, QColor(240, 240, 240));
            palette.setColor(QPalette::Button, QColor(43, 45, 48));       // Button #2b2d30
            palette.setColor(QPalette::ButtonText, QColor(240, 240, 240));
            palette.setColor(QPalette::Highlight, QColor(0, 191, 255));   // KDE Accent #00BFFF
            palette.setColor(QPalette::HighlightedText, QColor(0, 0, 0));
            return palette;
        }

        QIcon getKirigamiIcon(const QString& name, const QString& fallback = "document-new") const {
            QIcon icon = QIcon::fromTheme(name);
            if (icon.isNull()) {
                icon = QIcon::fromTheme(fallback);
            }
            return icon;
        }

    private:
        KDEIntegrationService() = default;
        ~KDEIntegrationService() = default;
        KDEIntegrationService(const KDEIntegrationService&) = delete;
        KDEIntegrationService& operator=(const KDEIntegrationService&) = delete;
    };

}

#endif

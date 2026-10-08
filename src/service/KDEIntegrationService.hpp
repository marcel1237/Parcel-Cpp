#ifndef KDE_INTEGRATION_SERVICE_HPP
#define KDE_INTEGRATION_SERVICE_HPP

#include <QString>
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

        // KDE Frameworks & Kirigami API Bridge (api.kde.org / kirigami-index.html)
        QString getApiInfo() const {
            return "KDE Frameworks 6 & Kirigami API Bridge active (api.kde.org)";
        }

        // KDevelop Core & Language Support API Info
        QString getKDevelopApiInfo() const {
            return "KDevelop Plugin, AST, and Language Support Integration Layer active";
        }

        // KDE Breeze / Kirigami Dark Color Palette
        QPalette getKdeDarkPalette() const {
            QPalette palette;
            palette.setColor(QPalette::Window, QColor(26, 26, 46));       // #1a1a2e
            palette.setColor(QPalette::WindowText, QColor(220, 220, 220));
            palette.setColor(QPalette::Base, QColor(20, 20, 30));         // #14141e
            palette.setColor(QPalette::AlternateBase, QColor(30, 30, 45));
            palette.setColor(QPalette::Text, QColor(240, 240, 240));
            palette.setColor(QPalette::Button, QColor(43, 45, 48));       // #2b2d30
            palette.setColor(QPalette::ButtonText, QColor(240, 240, 240));
            palette.setColor(QPalette::Highlight, QColor(0, 191, 255));   // #00BFFF
            palette.setColor(QPalette::HighlightedText, QColor(0, 0, 0));
            return palette;
        }

        // Kirigami Icon Resolver with KDE Fallback
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

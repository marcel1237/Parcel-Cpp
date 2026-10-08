#ifndef GTK3_INTEGRATION_SERVICE_HPP
#define GTK3_INTEGRATION_SERVICE_HPP

#include <QString>
#include <QColor>
#include <QPalette>

namespace Parcel::Service {

    class GTK3IntegrationService {
    public:
        static GTK3IntegrationService& getInstance() {
            static GTK3IntegrationService instance;
            return instance;
        }

        QString getApiInfo() const {
            return "GTK+ 3 Full API Integration Layer active (developer.gnome.org/gtk3)";
        }

        QString getGtk3WidgetsApi() const {
            return "GTK+ 3 Widget Toolkit: GtkWindow, GtkBox, GtkButton, GtkContainer, and GtkCssProvider";
        }

        QPalette getGtk3DarkPalette() const {
            QPalette palette;
            palette.setColor(QPalette::Window, QColor(45, 45, 45));
            palette.setColor(QPalette::WindowText, QColor(220, 220, 220));
            palette.setColor(QPalette::Base, QColor(35, 35, 35));
            palette.setColor(QPalette::Text, QColor(220, 220, 220));
            palette.setColor(QPalette::Button, QColor(53, 53, 53));
            palette.setColor(QPalette::ButtonText, QColor(220, 220, 220));
            palette.setColor(QPalette::Highlight, QColor(33, 93, 156));
            palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
            return palette;
        }

    private:
        GTK3IntegrationService() = default;
        ~GTK3IntegrationService() = default;
        GTK3IntegrationService(const GTK3IntegrationService&) = delete;
        GTK3IntegrationService& operator=(const GTK3IntegrationService&) = delete;
    };

}

#endif

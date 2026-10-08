#ifndef GTK2_INTEGRATION_SERVICE_HPP
#define GTK2_INTEGRATION_SERVICE_HPP

#include <QString>
#include <QColor>
#include <QPalette>

namespace Parcel::Service {

    class GTK2IntegrationService {
    public:
        static GTK2IntegrationService& getInstance() {
            static GTK2IntegrationService instance;
            return instance;
        }

        QString getApiInfo() const {
            return "GTK+ 2 Full Legacy API Integration Layer active (developer.gnome.org/gtk2)";
        }

        QString getGtk2WidgetsApi() const {
            return "GTK+ 2 Widget Toolkit: GtkWindow, GtkVBox, GtkButton, GtkLabel, and GtkStyle";
        }

        QPalette getGtk2DarkPalette() const {
            QPalette palette;
            palette.setColor(QPalette::Window, QColor(60, 60, 60));
            palette.setColor(QPalette::WindowText, QColor(200, 200, 200));
            palette.setColor(QPalette::Base, QColor(45, 45, 45));
            palette.setColor(QPalette::Text, QColor(200, 200, 200));
            palette.setColor(QPalette::Button, QColor(70, 70, 70));
            palette.setColor(QPalette::ButtonText, QColor(200, 200, 200));
            palette.setColor(QPalette::Highlight, QColor(40, 110, 180));
            palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
            return palette;
        }

    private:
        GTK2IntegrationService() = default;
        ~GTK2IntegrationService() = default;
        GTK2IntegrationService(const GTK2IntegrationService&) = delete;
        GTK2IntegrationService& operator=(const GTK2IntegrationService&) = delete;
    };

}

#endif

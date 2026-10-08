#ifndef GTK4_INTEGRATION_SERVICE_HPP
#define GTK4_INTEGRATION_SERVICE_HPP

#include <QString>
#include <QStringList>
#include <QColor>
#include <QPalette>

namespace Parcel::Service {

    class GTK4IntegrationService {
    public:
        static GTK4IntegrationService& getInstance() {
            static GTK4IntegrationService instance;
            return instance;
        }

        QString getApiInfo() const {
            return "GTK 4 Full API Integration Layer active (docs.gtk.org/gtk4)";
        }

        // GTK 4 Core Subsystems & Libraries
        QString getGLibApi() const { return "GLib: Core application building blocks, type system, memory management, and utility functions"; }
        QString getGObjectApi() const { return "GObject: Object-oriented type system, properties, and signal/closure mechanisms"; }
        QString getGioApi() const { return "Gio: Modern VFS, asynchronous I/O, network sockets, and GAction/GMenu model"; }
        QString getGdkApi() const { return "GDK 4: Abstraction layer for windowing systems, input events, and surfaces"; }
        QString getGskApi() const { return "GSK 4: Rendering and scene graph acceleration library (OpenGL, Vulkan, Cairo)"; }
        QString getGtk4WidgetsApi() const { return "GTK 4 Widget Toolkit: High-performance UI controls, list models, custom layout managers, and CSS theming"; }

        // Adwaita / GTK4 Dark Theme Palette representation
        QPalette getGtkDarkPalette() const {
            QPalette palette;
            palette.setColor(QPalette::Window, QColor(30, 30, 30));         // Adwaita Dark background #1e1e1e
            palette.setColor(QPalette::WindowText, QColor(240, 240, 240));
            palette.setColor(QPalette::Base, QColor(24, 24, 24));
            palette.setColor(QPalette::AlternateBase, QColor(38, 38, 38));
            palette.setColor(QPalette::Text, QColor(240, 240, 240));
            palette.setColor(QPalette::Button, QColor(53, 53, 53));
            palette.setColor(QPalette::ButtonText, QColor(240, 240, 240));
            palette.setColor(QPalette::Highlight, QColor(53, 132, 228));   // Adwaita Blue accent #3584e4
            palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
            return palette;
        }

        // GTK 4 CSS Provider template for styling widgets
        QString generateGtkCssSnippet() const {
            return QString(
                "/* GTK 4 / Adwaita Dark CSS Theme Snippet */\n"
                "window {\n"
                "    background-color: #1e1e1e;\n"
                "    color: #f6f6f6;\n"
                "}\n"
                "button.suggested-action {\n"
                "    background-color: #3584e4;\n"
                "    color: white;\n"
                "    border-radius: 6px;\n"
                "}\n"
            );
        }

    private:
        GTK4IntegrationService() = default;
        ~GTK4IntegrationService() = default;
        GTK4IntegrationService(const GTK4IntegrationService&) = delete;
        GTK4IntegrationService& operator=(const GTK4IntegrationService&) = delete;
    };

}

#endif

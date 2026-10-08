#ifndef GTK4_MANAGER_HPP
#define GTK4_MANAGER_HPP

#include <QString>
#include <QObject>

namespace Parcel::Service {

    class GTK4Manager : public QObject {
        Q_OBJECT
    public:
        static GTK4Manager& getInstance() {
            static GTK4Manager instance;
            return instance;
        }

        // Generates C++ source code template for a GTK 4 application
        QString generateGtk4AppCode(const QString& appName = "ParcelGtkApp") const {
            return QString(
                "#include <gtk/gtk.h>\n\n"
                "static void on_activate(GtkApplication *app, gpointer user_data) {\n"
                "    GtkWidget *window = gtk_application_window_new(app);\n"
                "    gtk_window_set_title(GTK_WINDOW(window), \"%1 - GTK 4 Window\");\n"
                "    gtk_window_set_default_size(GTK_WINDOW(window), 500, 400);\n\n"
                "    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);\n"
                "    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);\n"
                "    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);\n\n"
                "    GtkWidget *btn = gtk_button_new_with_label(\"Clique em Mim (GTK 4)\");\n"
                "    gtk_box_append(GTK_BOX(box), btn);\n\n"
                "    gtk_window_set_child(GTK_WINDOW(window), box);\n"
                "    gtk_window_present(GTK_WINDOW(window));\n"
                "}\n\n"
                "int main(int argc, char **argv) {\n"
                "    GtkApplication *app = gtk_application_new(\"com.parcel.%2\", G_APPLICATION_DEFAULT_FLAGS);\n"
                "    g_signal_connect(app, \"activate\", G_CALLBACK(on_activate), NULL);\n"
                "    int status = g_application_run(G_APPLICATION(app), argc, argv);\n"
                "    g_object_unref(app);\n"
                "    return status;\n"
                "}\n"
            ).arg(appName, appName.toLower());
        }

        // Generates GTK 4 CSS Theme template
        QString generateGtkCssTheme() const {
            return QString(
                "/* Adwaita Dark Custom Theme for GTK 4 */\n"
                "window {\n"
                "    background-color: #1e1e1e;\n"
                "    color: #f6f6f6;\n"
                "}\n"
                "button {\n"
                "    background-color: #3584e4;\n"
                "    color: white;\n"
                "    border-radius: 6px;\n"
                "    padding: 8px 16px;\n"
                "    font-weight: bold;\n"
                "}\n"
                "button:hover {\n"
                "    background-color: #1c71d8;\n"
                "}\n"
            );
        }

    private:
        GTK4Manager() = default;
        ~GTK4Manager() = default;
        GTK4Manager(const GTK4Manager&) = delete;
        GTK4Manager& operator=(const GTK4Manager&) = delete;
    };

}

#endif

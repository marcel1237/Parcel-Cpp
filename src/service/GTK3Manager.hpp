#ifndef GTK3_MANAGER_HPP
#define GTK3_MANAGER_HPP

#include <QString>
#include <QObject>

namespace Parcel::Service {

    class GTK3Manager : public QObject {
        Q_OBJECT
    public:
        static GTK3Manager& getInstance() {
            static GTK3Manager instance;
            return instance;
        }

        // Generates C++ source code template for a GTK+ 3 application
        QString generateGtk3AppCode(const QString& appName = "ParcelGtk3App") const {
            return QString(
                "#include <gtk/gtk.h>\n\n"
                "static void on_button_clicked(GtkButton *button, gpointer user_data) {\n"
                "    g_print(\"Botão clicado na aplicação GTK+ 3!\\n\");\n"
                "}\n\n"
                "int main(int argc, char *argv[]) {\n"
                "    gtk_init(&argc, &argv);\n\n"
                "    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);\n"
                "    gtk_window_set_title(GTK_WINDOW(window), \"%1 - GTK+ 3 Window\");\n"
                "    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);\n\n"
                "    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);\n"
                "    gtk_container_set_border_width(GTK_CONTAINER(box), 15);\n"
                "    gtk_container_add(GTK_CONTAINER(window), box);\n\n"
                "    GtkWidget *button = gtk_button_new_with_label(\"Clique em Mim (GTK+ 3)\");\n"
                "    g_signal_connect(button, \"clicked\", G_CALLBACK(on_button_clicked), NULL);\n"
                "    gtk_box_pack_start(GTK_BOX(box), button, TRUE, TRUE, 0);\n\n"
                "    g_signal_connect(window, \"destroy\", G_CALLBACK(gtk_main_quit), NULL);\n"
                "    gtk_widget_show_all(window);\n\n"
                "    gtk_main();\n"
                "    return 0;\n"
                "}\n"
            ).arg(appName);
        }

        // Generates GTK+ 3 CSS Theme template
        QString generateGtk3CssTheme() const {
            return QString(
                "/* GTK+ 3 CSS Theme Snippet (Adwaita / Dark) */\n"
                "GtkWindow {\n"
                "    background-color: #2d2d2d;\n"
                "    color: #e0e0e0;\n"
                "}\n"
                "GtkButton {\n"
                "    background: #215d9c;\n"
                "    color: white;\n"
                "    border-radius: 4px;\n"
                "    padding: 6px 12px;\n"
                "}\n"
            );
        }

    private:
        GTK3Manager() = default;
        ~GTK3Manager() = default;
        GTK3Manager(const GTK3Manager&) = delete;
        GTK3Manager& operator=(const GTK3Manager&) = delete;
    };

}

#endif

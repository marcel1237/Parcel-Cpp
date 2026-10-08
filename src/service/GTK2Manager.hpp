#ifndef GTK2_MANAGER_HPP
#define GTK2_MANAGER_HPP

#include <QString>
#include <QObject>

namespace Parcel::Service {

    class GTK2Manager : public QObject {
        Q_OBJECT
    public:
        static GTK2Manager& getInstance() {
            static GTK2Manager instance;
            return instance;
        }

        // Generates C source code template for a GTK+ 2 application
        QString generateGtk2AppCode(const QString& appName = "ParcelGtk2App") const {
            return QString(
                "#include <gtk/gtk.h>\n\n"
                "static void on_button_clicked(GtkWidget *widget, gpointer data) {\n"
                "    g_print(\"Botão clicado na aplicação GTK+ 2!\\n\");\n"
                "}\n\n"
                "int main(int argc, char *argv[]) {\n"
                "    gtk_init(&argc, &argv);\n\n"
                "    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);\n"
                "    gtk_window_set_title(GTK_WINDOW(window), \"%1 - GTK+ 2 Window\");\n"
                "    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);\n\n"
                "    GtkWidget *vbox = gtk_vbox_new(FALSE, 10);\n"
                "    gtk_container_add(GTK_CONTAINER(window), vbox);\n\n"
                "    GtkWidget *button = gtk_button_new_with_label(\"Clique em Mim (GTK+ 2)\");\n"
                "    g_signal_connect(button, \"clicked\", G_CALLBACK(on_button_clicked), NULL);\n"
                "    gtk_box_pack_start(GTK_BOX(vbox), button, TRUE, TRUE, 0);\n\n"
                "    g_signal_connect(window, \"destroy\", G_CALLBACK(gtk_main_quit), NULL);\n"
                "    gtk_widget_show_all(window);\n\n"
                "    gtk_main();\n"
                "    return 0;\n"
                "}\n"
            ).arg(appName);
        }

    private:
        GTK2Manager() = default;
        ~GTK2Manager() = default;
        GTK2Manager(const GTK2Manager&) = delete;
        GTK2Manager& operator=(const GTK2Manager&) = delete;
    };

}

#endif

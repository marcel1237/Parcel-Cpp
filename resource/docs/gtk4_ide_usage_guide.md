# Guia Prático: Como Usar as Bibliotecas do GTK 4 em uma Aplicação C++ / IDE

Este guia detalha como configurar, compilar e utilizar o **GTK 4** em projetos C++ dentro de uma IDE ou aplicação nativa no Linux.

---

## 🛠️ 1. Configuração de Compilação (`pkg-config`)

O GTK 4 requer flags específicas de compilação fornecidas pelo `pkg-config`. No CMake ou Makefile, obtém-se os parâmetros com:

```bash
pkg-config --cflags --libs gtk4
```

### Exemplo de CMakeLists.txt para GTK 4:
```cmake
cmake_minimum_required(VERSION 3.16)
project(GtkApp LANGUAGES CXX C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

add_executable(GtkApp main.cpp)
target_include_directories(GtkApp PRIVATE ${GTK4_INCLUDE_DIRS})
target_link_libraries(GtkApp PRIVATE ${GTK4_LIBRARIES})
```

---

## 💻 2. Exemplo Básico de Aplicação GTK 4 em C++

Abaixo está a estrutura padrão para inicializar uma aplicação GTK 4 com `GtkApplication`:

```cpp
#initializer / main.cpp
#include <gtk/gtk.h>

static void on_button_clicked(GtkButton *button, gpointer user_data) {
    g_print("Botão clicado na aplicação GTK 4!\n");
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Parcel C++ - GTK 4 Integration");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    GtkWidget *button = gtk_button_new_with_label("Clique em Mim (GTK 4)");
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), NULL);

    gtk_window_set_child(GTK_WINDOW(window), button);
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("com.parcel.gtkapp", G_APPLICATION_DEFAULT_FLAGS);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
```

---

## 🎨 3. Estilização Dinâmica com CSS (`GtkCssProvider`)

O GTK 4 possui um motor CSS nativo muito poderoso. Podemos aplicar temas escuros estilo Adwaita em tempo de execução:

```cpp
void apply_custom_css() {
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "window { background-color: #1e1e1e; color: #ffffff; }"
        "button { background-color: #3584e4; color: white; border-radius: 6px; padding: 8px 16px; }", -1);

    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    g_object_unref(provider);
}
```

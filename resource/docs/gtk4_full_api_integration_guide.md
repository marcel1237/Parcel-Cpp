# Guia Definitivo: Integração Completa da API do GTK 4 no Linux

Este documento descreve a arquitetura para integrar o ecossistema completo do **GTK 4** e suas bibliotecas fundamentais (**GLib**, **GObject**, **Gio**, **GDK 4**, **GSK 4** e **GTK 4 Widgets**) em aplicações de desenvolvimento no Linux.

---

## 🏛️ 1. Subсистemas e Módulos do GTK 4

A arquitetura do GTK 4 (`docs.gtk.org/gtk4`) é modularizada em camadas bem definidas:

1. **GLib:** Camada fundamental de tipos de dados, manipulação de strings, controle de eventos (MainLoop) e gerenciamento de memória.
2. **GObject:** Sistema de tipos orientado a objetos baseado em C, suporte a propriedades e reflexão em tempo de execução.
3. **Gio:** Sistema de arquivos virtual (VFS), I/O assíncrono, sockets de rede, modelo de ações (`GAction`) e menus (`GMenu`).
4. **GDK 4:** Camada de abstração de janelas, superfícies e tratamento de eventos de entrada (mouse, teclado, toque).
5. **GSK 4 (GTK Scene Graph Kit):** Biblioteca de renderização acelerada por GPU (suporte a OpenGL, Vulkan e Cairo) para composição de elementos visuais.
6. **GTK 4 Widgets:** Toolkit visual de controles de interface, modelos de listas (`GListModel`), gerenciadores de layout flexíveis e sistema nativo de estilização por CSS.

---

## 🛠️ 2. Configuração no CMake (`CMakeLists.txt`)

Para encontrar e linkar as bibliotecas do GTK 4 via pkg-config no CMake:

```cmake
# Encontrar GTK 4 via pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

# Incluir diretórios e linkar ao alvo
target_include_directories(ParcelCPP PRIVATE ${GTK4_INCLUDE_DIRS})
target_link_libraries(ParcelCPP PRIVATE ${GTK4_LIBRARIES})
```

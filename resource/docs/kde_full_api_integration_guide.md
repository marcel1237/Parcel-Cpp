# Guia Definitivo: Integração Completa da API do KDE (KDE Frameworks 6) no Linux

Este documento descreve como integrar o ecossistema completo de bibliotecas e frameworks do KDE (**KDE Frameworks 6 / KF6** e **Kirigami**) em aplicações C++/Qt no Linux.

---

## 🏛️ 1. Principais Módulos do KDE Frameworks (KF6)

Para integrar a API do KDE (`https://api.kde.org/`) em uma aplicação C++, dividimos os módulos conforme a necessidade arquitetural:

1. **KCoreAddons:** Utilitários centrais de sistema, gerenciamento de plugins (`KPluginMetaData`) e filas de tarefas assíncronas.
2. **KConfig & KConfigCore:** Sistema avançado de configuração (`KSharedConfig`, `KConfigGroup`) compatível com padrões XDG.
3. **KIO (KDE Input/Output):** Gerenciamento de arquivos transparente à rede (suporte a sftp, smb, http, file) e operações de IO assíncronas.
4. **KWidgetsAddons:** Widgets de desktop adicionais, diálogos padronizados (`KPageDialog`, `KMessageBox`).
5. **KTextEditor:** Componente completo de editor de código para IDEs com suporte a destaque de sintaxe, dobras de código e autocompletar.
6. **Kirigami (`api.kde.org/kirigami-index.html`):** Toolkit de UI convergente para QML/Qt Quick, ideal para interfaces modernas e adaptativas.

---

## 🛠️ 2. Configuração no CMake (`CMakeLists.txt`)

Para encontrar e linkar os pacotes do KDE Frameworks via CMake:

```cmake
# Encontrar pacotes KF6 essenciais
find_package(KF6CoreAddons REQUIRED)
find_package(KF6Config REQUIRED)
find_package(KF6WidgetsAddons REQUIRED)
find_package(KF6I18n REQUIRED)
find_package(KF6IconThemes REQUIRED)
find_package(KF6TextEditor REQUIRED)

# Linkar ao executável alvo
target_link_libraries(ParcelCPP PRIVATE
    KF6::CoreAddons
    KF6::ConfigCore
    KF6::WidgetsAddons
    KF6::I18n
    KF6::IconThemes
    KF6::TextEditor
)
```

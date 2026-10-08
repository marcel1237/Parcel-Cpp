# 🗑️ Relatório de Objetos Não Utilizados no Projeto Parcel C++

---

## 1. 📦 Classes e Arquivos Inteiramente Não Utilizados (Código Morto)

Estes arquivos/classes existem na estrutura e alguns até são compilados via `CMakeLists.txt`, mas **nunca são instanciados ou utilizados** em nenhuma parte do sistema:

1. **`CatCore` (`CatCore.h` / `CatCore.c`)**
   - **Arquivos:** [CatCore.h](file:///home/marcel1237/Parcel%20C++/src/SS%20AI%20Agent/CatCore.h) | [CatCore.c](file:///home/marcel1237/Parcel%20C++/src/SS%20AI%20Agent/CatCore.c)
   - **Descrição:** As funções C puras `cat_has_marker()`, `cat_file_exists()` e o enum `CatStatus` são compilados no CMake, mas nenhum arquivo inclui ou chama essas funções.

2. **`PackageExplorer`**
   - **Arquivos:** [PackageExplorer.hpp](file:///home/marcel1237/Parcel%20C++/src/view/explorer/PackageExplorer.hpp) | [PackageExplorer.cpp](file:///home/marcel1237/Parcel%20C++/src/view/explorer/PackageExplorer.cpp)
   - **Descrição:** A classe `PackageExplorer` e seus métodos (`openProject`, `shouldFilter`, `listFilteredFiles`) estão incluídos em [ProjectWorkspace.hpp](file:///home/marcel1237/Parcel%20C++/src/view/ProjectWorkspace.hpp#L22), porém o workspace utiliza diretamente o `QFileSystemModel` do Qt e nunca instancia essa classe.

3. **`ProjectTypeView` & `ProjectCard`**
   - **Arquivo:** [ProjectTypeView.hpp](file:///home/marcel1237/Parcel%20C++/src/view/ProjectTypeView.hpp)
   - **Descrição:** O arquivo é incluído no [MainWindow.hpp](file:///home/marcel1237/Parcel%20C++/src/view/MainWindow.hpp#L18), mas a classe `ProjectTypeView` nunca é instanciada nem adicionada ao `QStackedWidget` principal.

4. **`FileIconProvider` & Enum `IconType`**
   - **Arquivo:** [FileIconProvider.hpp](file:///home/marcel1237/Parcel%20C++/src/view/explorer/FileIconProvider.hpp)
   - **Descrição:** O projeto utiliza a classe `CustomIconProvider` (derivada de `QFileIconProvider` do Qt). A classe `FileIconProvider` e seu enum `IconType` não possuem referências.

---

## 2. ⚙️ Funções e Métodos Não Utilizados

1. **`NavigationController::goBack()`**
   - **Arquivo:** [NavigationController.hpp](file:///home/marcel1237/Parcel%20C++/src/core/navigation/NavigationController.hpp#L29)
   - **Descrição:** Método implementado para retornar ao estado de navegação anterior, porém nunca é invocado.

2. **`DialogService::showConfirmation()`**
   - **Arquivo:** [DialogService.hpp](file:///home/marcel1237/Parcel%20C++/src/service/DialogService.hpp#L10)
   - **Descrição:** Função utilitária via `std::cin` de terminal. Apenas o método `showError` é utilizado no projeto.

3. **`EditorPane::runHighlightingAsync()`**
   - **Arquivo:** [EditorPane.hpp](file:///home/marcel1237/Parcel%20C++/src/view/editor/EditorPane.hpp#L99) | [EditorPane.cpp](file:///home/marcel1237/Parcel%20C++/src/view/editor/EditorPane.cpp#L471)
   - **Descrição:** Método declarado e implementado com corpo vazio no `.cpp`, sem chamadas no código.

4. **`DesignerPane::setViewName()`**
   - **Arquivo:** [DesignerPane.hpp](file:///home/marcel1237/Parcel%20C++/src/view/editor/DesignerPane.hpp#L508)
   - **Descrição:** Método setter definido para alterar o nome da view QML, mas sem invocações externas.

---

## 3. 📌 Constantes, Enums e Variáveis Não Utilizadas

1. **Constante `VERSION_VALUE`**
   - **Arquivo:** [Version.hpp](file:///home/marcel1237/Parcel%20C++/src/core/Version.hpp#L6)
   - **Descrição:** `inline const std::string VERSION_VALUE = "1.0.0";` é definida no header, mas o [MainWindow.hpp](file:///home/marcel1237/Parcel%20C++/src/view/MainWindow.hpp#L29) usa a string hardcoded `"1.0.0"` em vez de importar a constante.

2. **Valores do Enum `NavigationTarget` (`TEMPLATES`, `GIT`, `GITHUB`, `ABOUT`)**
   - **Arquivo:** [NavigationTarget.hpp](file:///home/marcel1237/Parcel%20C++/src/core/navigation/NavigationTarget.hpp#L9-L13)
   - **Descrição:** Opções declaradas no enum que não possuem tratamento em `switch/case` nem são navegadas no controlador.

3. **`EditorPane::MAX_FILE_SIZE`**
   - **Arquivo:** [EditorPane.hpp](file:///home/marcel1237/Parcel%20C++/src/view/editor/EditorPane.hpp#L59)
   - **Descrição:** `static constexpr long MAX_FILE_SIZE = 1024 * 1024;` declarada mas nunca testada na leitura de arquivos.

4. **Atributo `VersionsPane::m_projectPath`**
   - **Arquivo:** [VersionsPane.hpp](file:///home/marcel1237/Parcel%20C++/src/view/VersionsPane.hpp#L61)
   - **Descrição:** Variável privada definida e inicializada no construtor, mas nunca lida na classe.

5. **Mapa de timestamps `ProjectKnowledge::m_lastSeen`**
   - **Arquivo:** [ProjectKnowledge.hpp](file:///home/marcel1237/Parcel%20C++/src/SS%20AI%20Agent/ProjectKnowledge.hpp#L44)
   - **Descrição:** `m_lastSeen` é atualizado em `recordModifiedFile()`, mas os valores gravados no mapa nunca são lidos em getters ou análises.

---

## 4. 📄 Directivas de Inclusão Não Utilizadas (`#include` Obsoletos)

Estes cabeçalhos são importados nos arquivos abaixo sem que nenhum tipo, macro ou função deles seja utilizado:

| Arquivo | Includes Não Utilizados |
| :--- | :--- |
| [NavigationController.hpp](file:///home/marcel1237/Parcel%20C++/src/core/navigation/NavigationController.hpp) | `<map>`, `<memory>` |
| [BackupService.hpp](file:///home/marcel1237/Parcel%20C++/src/service/BackupService.hpp) | `<QDebug>` |
| [FileSystemService.cpp](file:///home/marcel1237/Parcel%20C++/src/service/FileSystemService.cpp) | `<filesystem>` |
| [PythonExecutor.hpp](file:///home/marcel1237/Parcel%20C++/src/SS%20AI%20Agent/PythonExecutor.hpp) | `<QDebug>` |
| [ProjectKnowledge.cpp](file:///home/marcel1237/Parcel%20C++/src/SS%20AI%20Agent/ProjectKnowledge.cpp) | `<QFileInfo>` |
| [QLMEngine.hpp](file:///home/marcel1237/Parcel%20C++/src/SS%20AI%20Agent/QLMEngine.hpp) | `<QJsonArray>` |
| [VersionsPane.hpp](file:///home/marcel1237/Parcel%20C++/src/view/VersionsPane.hpp) | `<QMessageBox>`, `<QSqlError>` |
| [SavesPane.hpp](file:///home/marcel1237/Parcel%20C++/src/view/SavesPane.hpp) | `<QSqlError>`, `<QFileInfo>` |
| [EditorPane.hpp](file:///home/marcel1237/Parcel%20C++/src/view/editor/EditorPane.hpp) | `<QDesktopServices>`, `<QLineEdit>`, `<QStringListModel>` |
| [EditorPane.cpp](file:///home/marcel1237/Parcel%20C++/src/view/editor/EditorPane.cpp) | `<QDesktopServices>`, `<QUrlQuery>` |
| [DesignerPane.hpp](file:///home/marcel1237/Parcel%20C++/src/view/editor/DesignerPane.hpp) | `<QQmlEngine>` |
| [EditorHost.hpp](file:///home/marcel1237/Parcel%20C++/src/view/editor/EditorHost.hpp) | `<memory>`, `<algorithm>`, `<QMessageBox>` |
| [PdfViewerPane.hpp](file:///home/marcel1237/Parcel%20C++/src/view/editor/PdfViewerPane.hpp) | `<QFileInfo>` |
| [PdfComposerPane.hpp](file:///home/marcel1237/Parcel%20C++/src/view/editor/PdfComposerPane.hpp) | `<QPainter>` |
| [FileTreeDelegate.hpp](file:///home/marcel1237/Parcel%20C++/src/view/explorer/FileTreeDelegate.hpp) | `<QPainter>`, `<QFileInfo>` |

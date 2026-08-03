cmake_minimum_required(VERSION 3.16)

# Nome do projeto
project("Parcel C++" LANGUAGES CXX)

# Exige C++17 para suporte nativo a std::filesystem (usado no GitService e PackageExplorer)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Gera compile_commands.json para que o Clangd (IDE) encontre os headers do Qt
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Habilita o processamento automático de arquivos do Qt
set(CMAKE_AUTOMOC ON) # Necessário para Q_OBJECT e sinais/slots
set(CMAKE_AUTOUIC ON) # Para arquivos de interface .ui
set(CMAKE_AUTORCC ON) # Para recursos como ícones e temas

# Localiza os pacotes do Qt 6 necessários para a interface e o navegador
find_package(Qt6 COMPONENTS Widgets WebEngineWidgets PrintSupport Sql Charts Quick QuickWidgets Network REQUIRED)

# Organização dos arquivos fonte conforme as classes convertidas
set(SOURCES
    src/main.cpp

    # Core e Navegação
    src/core/navigation/NavigationController.hpp
    src/core/navigation/NavigationTarget.hpp
    src/core/Constants.hpp
    src/core/Version.hpp

    # Serviços (Backend Nativo)
    src/service/GitService.cpp
    src/service/GitService.hpp
    src/service/FileSystemService.cpp
    src/service/FileSystemService.hpp
    src/service/PersistenceService.cpp
    src/service/PersistenceService.hpp
    src/service/DialogService.hpp
    src/service/PdfService.hpp
    src/service/BackupService.hpp

    # Views (Interface Qt)
    src/view/MainWindow.hpp
    src/view/DashboardView.hpp
    src/view/NewProjectView.hpp
    src/view/OpenProjectView.hpp
    src/view/ProjectTypeView.hpp
    src/view/SettingsView.hpp
    src/view/DatabasePane.hpp
    src/view/ProjectWorkspace.hpp
    src/view/ProjectHeader.hpp
    src/view/GeminiPane.hpp
    src/view/OpenAIPane.hpp
    src/view/BrowserPane.hpp
    src/view/TerminalPane.hpp
    src/view/InspectionPane.hpp
    src/view/ShellScriptingPane.hpp
    src/view/LogcatPane.hpp
    src/view/GitPane.hpp
    src/view/SSAIConsolePane.hpp
    src/view/VersionsPane.hpp
    src/view/SavesPane.hpp
    src/view/explorer/PackageExplorer.cpp
    src/view/explorer/PackageExplorer.hpp
    src/view/explorer/DBMSExplorer.hpp
    src/view/explorer/CustomIconProvider.hpp
    src/view/explorer/FileTreeDelegate.hpp
    src/view/explorer/FileIconProvider.hpp
    src/view/editor/EditorPane.cpp
    src/view/editor/EditorPane.hpp
    src/view/editor/DesignerPane.hpp
    src/view/editor/DiffDialog.hpp
    src/view/editor/HexEditorPane.hpp
    src/view/editor/PdfViewerPane.hpp
    src/view/editor/PdfComposerPane.hpp
    src/view/editor/EditorHost.hpp
    src/view/editor/ProjectHighlighter.cpp
    src/view/editor/ProjectHighlighter.hpp

    # SS AI Agent (Extraordinary Shell Intelligence)
    "src/SS AI Agent/CatCore.c"
    "src/SS AI Agent/CatCore.h"
    "src/SS AI Agent/FilePatcher.cpp"
    "src/SS AI Agent/FilePatcher.hpp"
    "src/SS AI Agent/GeminiClient.cpp"
    "src/SS AI Agent/GeminiClient.hpp"
    "src/SS AI Agent/LocalAgent.cpp"
    "src/SS AI Agent/LocalAgent.hpp"
    "src/SS AI Agent/MLService.cpp"
    "src/SS AI Agent/MLService.hpp"
    "src/SS AI Agent/WebSearchService.cpp"
    "src/SS AI Agent/WebSearchService.hpp"
    "src/SS AI Agent/ProjectKnowledge.cpp"
    "src/SS AI Agent/ProjectKnowledge.hpp"
    "src/SS AI Agent/OpenAIClient.cpp"
    "src/SS AI Agent/OpenAIClient.hpp"
    "src/SS AI Agent/PythonExecutor.cpp"
    "src/SS AI Agent/PythonExecutor.hpp"
    "src/SS AI Agent/QLMEngine.cpp"
    "src/SS AI Agent/QLMEngine.hpp"
    "src/SS AI Agent/SSAIModule.cpp"
    "src/SS AI Agent/SSAIModule.hpp"
    "src/SS AI Agent/ScriptValidator.cpp"
    "src/SS AI Agent/ScriptValidator.hpp"
)

# Cria o executável final para Linux
# Usamos um nome de alvo sem espaços, mas configuramos o nome de saída final
add_executable(ParcelCPP ${SOURCES})
set_target_properties(ParcelCPP PROPERTIES OUTPUT_NAME "Parcel C++")

# Define os diretórios de inclusão para facilitar os imports (#include "GitService.hpp")
target_include_directories(ParcelCPP PRIVATE src)

# Linka as bibliotecas do Qt e o suporte a sistema de arquivos
target_link_libraries(ParcelCPP PRIVATE
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::PrintSupport
    Qt6::Sql
    Qt6::Charts
    Qt6::Quick
    Qt6::QuickWidgets
    Qt6::Network
    stdc++fs
)

# Configurações de compilação para Ubuntu (G++ / Clang)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(ParcelCPP PRIVATE -Wall -Wextra)
endif()

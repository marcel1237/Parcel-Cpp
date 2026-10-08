# Guia de Arquitetura: Integração Completa do .NET em uma IDE Linux

Este documento detalha a estratégia de engenharia e a arquitetura para integrar o ecossistema completo do **.NET (.NET 8/9, Roslyn, OmniSharp, Entity Framework Core e LINQ)** em uma IDE de desenvolvimento nativa em C++/Qt no Linux (como o **Parcel C++**).

---

## 🏗️ 1. Arquitetura Geral da Integração .NET no Linux

Para rodar tecnologias .NET em um ambiente Linux (gerenciado por uma aplicação Qt/C++), a arquitetura se divide em quatro camadas principais:

```mermaid
graph TD
    A[Parcel C++ IDE (Qt 6 / C++)] --> B[.NET CLI Bridge Process]
    A --> C[OmniSharp / LSP Server (C# Autocomplete & Diagnostics)]
    A --> D[Roslyn Compiler Services (Code Analysis & Refactoring)]
    
    B --> E[.NET 8/9 Runtime SDK]
    C --> E
    D --> E
    
    E --> F[Entity Framework Core (EF Core) & LINQ Engine]
```

---

## 🛠️ 2. Componentes e Implementação Prática

### A. .NET CLI & Process Execution (`dotnet` CLI)
A IDE interage com o SDK do .NET executando processos assíncronos (`QProcess` em C++):
- **Compilação e Execução:** `dotnet build`, `dotnet run`, `dotnet test`.
- **Gerenciamento de Pacotes NuGet:** `dotnet add package <PackageName>`.

### B. Suporte a C# e Intellisense via OmniSharp / LSP
Para prover autocomplete, navegação de código (Go-to-Definition) e diagnósticos em tempo real para C#:
- Utiliza-se o **OmniSharp Server** (`omnisharp` via Mono ou .NET global tool) ou o **C# Dev Kit LSP** rodando em segundo plano comunincando-se via Protocolo de Servidor de Linguagem (LSP) sobre JSON-RPC.

### C. Roslyn Compiler Platform (`Microsoft.CodeAnalysis`)
- Permite que a IDE analise árvores de sintaxe abstrata (AST) em C#, execute refatorações automáticas e verifique erros de compilação sem precisar invocar o compilador de linha de comando completo a cada tecla digitada.

### D. Entity Framework Core (EF Core) e Ferramentas CLI
- Integração com o pacote `dotnet-ef` para gerenciar migrações de banco de dados (`dotnet ef migrations add`) e persistência ORM diretamente nos projetos gerenciados pela IDE.

### E. Motor LINQ (Language Integrated Query)
- Permite consultas declarativas e manipulação de coleções de dados com operadores como `.Where()`, `.Select()`, `.OrderBy()` tanto em projetos .NET quanto em pontes de dados híbridas C++/C#.

---

## 📦 3. Pré-requisitos de Instalação no Linux (Ubuntu/Debian)

Para habilitar todo o ecossistema .NET na máquina de desenvolvimento:

```bash
# Instalação do .NET SDK 8.0+
sudo apt update
sudo apt install -y dotnet-sdk-8.0

# Instalação das ferramentas globais do Entity Framework e Scripting
dotnet tool install --global dotnet-ef
dotnet tool install --global dotnet-script
```

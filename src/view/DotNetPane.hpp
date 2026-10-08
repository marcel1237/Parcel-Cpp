#ifndef DOTNET_PANE_HPP
#define DOTNET_PANE_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include "../service/DotNetService.hpp"
#include "../service/DotNetManager.hpp"

namespace Parcel::View {

    class DotNetPane : public QWidget {
        Q_OBJECT
    public:
        explicit DotNetPane(QWidget* parent = nullptr) : QWidget(parent) {
            auto* mainLayout = new QVBoxLayout(this);
            mainLayout->setContentsMargins(15, 15, 15, 15);
            mainLayout->setSpacing(12);

            auto* title = new QLabel("⚡ .NET Framework, LINQ & Entity Framework Core Studio", this);
            title->setStyleSheet("font-size: 16px; font-weight: bold; color: #34A853;");
            mainLayout->addWidget(title);

            // Controls layout
            auto* toolbarLayout = new QHBoxLayout();

            auto* checkSdkBtn = new QPushButton("Verificar .NET SDK", this);
            checkSdkBtn->setStyleSheet("background-color: #2b2d30; color: white; padding: 6px 12px; border-radius: 4px;");
            toolbarLayout->addWidget(checkSdkBtn);

            auto* genCodeBtn = new QPushButton("Gerar C# Program.cs", this);
            genCodeBtn->setStyleSheet("background-color: #34A853; color: white; padding: 6px 12px; border-radius: 4px;");
            toolbarLayout->addWidget(genCodeBtn);

            auto* genEfBtn = new QPushButton("Gerar EF DbContext", this);
            genEfBtn->setStyleSheet("background-color: #4285F4; color: white; padding: 6px 12px; border-radius: 4px;");
            toolbarLayout->addWidget(genEfBtn);

            auto* genProjBtn = new QPushButton("Gerar .csproj", this);
            genProjBtn->setStyleSheet("background-color: #FBBC05; color: black; font-weight: bold; padding: 6px 12px; border-radius: 4px;");
            toolbarLayout->addWidget(genProjBtn);

            auto* efMigrateBtn = new QPushButton("EF: Add Migration", this);
            efMigrateBtn->setStyleSheet("background-color: #34A853; color: white; padding: 6px 12px; border-radius: 4px;");
            toolbarLayout->addWidget(efMigrateBtn);

            toolbarLayout->addStretch();
            mainLayout->addLayout(toolbarLayout);

            // Console / Code Output
            m_console = new QTextEdit(this);
            m_console->setStyleSheet("background-color: #1a1a1a; color: #34A853; font-family: 'Monospace'; font-size: 11px; padding: 8px; border-radius: 4px;");
            m_console->setPlainText(Service::DotNetManager::getInstance().generateCSharpAppCode());
            mainLayout->addWidget(m_console);

            // Connections
            connect(checkSdkBtn, &QPushButton::clicked, [this]() {
                bool available = Service::DotNetService::getInstance().isDotNetAvailable();
                m_console->append(available ? "\n✅ .NET SDK está instalado e disponível no sistema." : "\n❌ .NET SDK não encontrado. Instale com 'sudo apt install dotnet-sdk-8.0'.");
            });

            connect(genCodeBtn, &QPushButton::clicked, [this]() {
                m_console->setPlainText(Service::DotNetManager::getInstance().generateCSharpAppCode());
            });

            connect(genEfBtn, &QPushButton::clicked, [this]() {
                m_console->setPlainText(Service::DotNetManager::getInstance().generateEfDbContextTemplate());
            });

            connect(genProjBtn, &QPushButton::clicked, [this]() {
                m_console->setPlainText(Service::DotNetManager::getInstance().generateCsprojTemplate());
            });

            connect(efMigrateBtn, &QPushButton::clicked, [this]() {
                m_console->append("\n📦 Executando Entity Framework: dotnet ef migrations add InitialMigration...");
                Service::DotNetService::getInstance().runDotNetCommand(QStringList() << "ef" << "migrations" << "add" << "InitialMigration", QString(), [this](QString out) {
                    m_console->append(out);
                });
            });
        }

    private:
        QTextEdit* m_console;
    };

}

#endif

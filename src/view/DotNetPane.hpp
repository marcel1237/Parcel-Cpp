#ifndef DOTNET_PANE_HPP
#define DOTNET_PANE_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QComboBox>
#include "../service/DotNetService.hpp"

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

            auto* newProjBtn = new QPushButton("Novo Projeto C#", this);
            newProjBtn->setStyleSheet("background-color: #4285F4; color: white; padding: 6px 12px; border-radius: 4px;");
            toolbarLayout->addWidget(newProjBtn);

            auto* efMigrateBtn = new QPushButton("EF Core: Add Migration", this);
            efMigrateBtn->setStyleSheet("background-color: #34A853; color: white; padding: 6px 12px; border-radius: 4px;");
            toolbarLayout->addWidget(efMigrateBtn);

            auto* efUpdateBtn = new QPushButton("EF Core: Update DB", this);
            efUpdateBtn->setStyleSheet("background-color: #FBBC05; color: black; font-weight: bold; padding: 6px 12px; border-radius: 4px;");
            toolbarLayout->addWidget(efUpdateBtn);

            toolbarLayout->addStretch();
            mainLayout->addLayout(toolbarLayout);

            // Console Output
            m_console = new QTextEdit(this);
            m_console->setReadOnly(true);
            m_console->setStyleSheet("background-color: #1a1a1a; color: #00FF7F; font-family: 'Monospace'; font-size: 11px; padding: 8px; border-radius: 4px;");
            mainLayout->addWidget(m_console);

            // Connections
            connect(checkSdkBtn, &QPushButton::clicked, [this]() {
                bool available = Service::DotNetService::getInstance().isDotNetAvailable();
                m_console->append(available ? "✅ .NET SDK está instalado e disponível no sistema." : "❌ .NET SDK não encontrado. Instale com 'sudo apt install dotnet-sdk-8.0'.");
            });

            connect(newProjBtn, &QPushButton::clicked, [this]() {
                m_console->append("🚀 Criando projeto console C# (.NET Core)...");
                Service::DotNetService::getInstance().runDotNetCommand(QStringList() << "new" << "console" << "-n" << "ParcelDotNetApp", QString(), [this](QString out) {
                    m_console->append(out);
                });
            });

            connect(efMigrateBtn, &QPushButton::clicked, [this]() {
                m_console->append("📦 Executando Entity Framework: dotnet ef migrations add InitialMigration...");
                Service::DotNetService::getInstance().runDotNetCommand(QStringList() << "ef" << "migrations" << "add" << "InitialMigration", QString(), [this](QString out) {
                    m_console->append(out);
                });
            });

            connect(efUpdateBtn, &QPushButton::clicked, [this]() {
                m_console->append("🗄️ Executando Entity Framework: dotnet ef database update...");
                Service::DotNetService::getInstance().runDotNetCommand(QStringList() << "ef" << "database" << "update", QString(), [this](QString out) {
                    m_console->append(out);
                });
            });
        }

    private:
        QTextEdit* m_console;
    };

}

#endif

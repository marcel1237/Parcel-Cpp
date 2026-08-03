#ifndef PROJECT_HEADER_HPP
#define PROJECT_HEADER_HPP

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileInfo>
#include "../service/GitService.hpp"
#include "../service/BackupService.hpp"

namespace Parcel::View {
    class ProjectHeader : public QWidget {
        Q_OBJECT
    public:
        explicit ProjectHeader(const std::string& projectPath, QWidget *parent = nullptr)
            : QWidget(parent), gitService(projectPath) {

            QFileInfo info(QString::fromStdString(projectPath));

            auto* mainLayout = new QHBoxLayout(this);
            mainLayout->setContentsMargins(15, 2, 15, 2);
            mainLayout->setSpacing(15);
            setFixedHeight(45); // Aumentado um pouco para caber os botões empilhados
            setStyleSheet("background-color: #252525; border-bottom: 1px solid #333;");

            // Unificado em uma única linha (Informações)
            auto* projectLabel = new QLabel("📁 <b>" + info.fileName() + "</b>", this);
            projectLabel->setStyleSheet("color: white; font-size: 13px;");
            mainLayout->addWidget(projectLabel);

            if (gitService.isGitRepository()) {
                auto* gitBadge = new QLabel("🌿 Git:" + QString::fromStdString(gitService.getCurrentBranch()), this);
                gitBadge->setStyleSheet("color: #4caf50; font-weight: bold; font-size: 11px;");
                mainLayout->addWidget(gitBadge);
            }

            auto* pathLabel = new QLabel("📍 <small>" + info.absoluteFilePath() + "</small>", this);
            pathLabel->setStyleSheet("color: #666;");
            mainLayout->addWidget(pathLabel);

            mainLayout->addStretch();

            // Layout Horizontal para os Botões
            auto* buttonLayout = new QHBoxLayout();
            buttonLayout->setSpacing(8);
            buttonLayout->setContentsMargins(0, 0, 0, 0);

            // Botão para o East Pane (AI + Shell)
            auto* geminiBtn = new QPushButton("✨ East Pane", this);
            geminiBtn->setStyleSheet("background-color: #4285F4; color: white; border-radius: 4px; padding: 5px 12px; font-weight: bold; font-size: 11px;");
            buttonLayout->addWidget(geminiBtn);

            // Botão para o South Pane (Browser, Terminal, etc)
            auto* browserBtn = new QPushButton("🌐 South Pane", this);
            browserBtn->setStyleSheet("background-color: #34A853; color: white; border-radius: 4px; padding: 5px 12px; font-weight: bold; font-size: 11px;");
            buttonLayout->addWidget(browserBtn);

            // Botão para o Designer Visual
            auto* designerBtn = new QPushButton("🎨 Designer", this);
            designerBtn->setStyleSheet("background-color: #E67E22; color: white; border-radius: 4px; padding: 5px 12px; font-weight: bold; font-size: 11px;");
            buttonLayout->addWidget(designerBtn);

            // Botão para o Build (com Backup)
            auto* buildBtn = new QPushButton("🔨 Build + Backup", this);
            buildBtn->setStyleSheet("background-color: #27ae60; color: white; border-radius: 4px; padding: 5px 12px; font-weight: bold; font-size: 11px;");
            buttonLayout->addWidget(buildBtn);

            // Botão para Exportar (Packaging)
            auto* exportBtn = new QPushButton("📦 Export", this);
            exportBtn->setStyleSheet("background-color: #8E44AD; color: white; border-radius: 4px; padding: 5px 12px; font-weight: bold; font-size: 11px;");
            buttonLayout->addWidget(exportBtn);

            mainLayout->addLayout(buttonLayout);

            connect(geminiBtn, &QPushButton::clicked, this, &ProjectHeader::geminiToggled);
            connect(browserBtn, &QPushButton::clicked, this, &ProjectHeader::browserToggled);
            connect(designerBtn, &QPushButton::clicked, this, &ProjectHeader::designerToggled);
            connect(exportBtn, &QPushButton::clicked, this, &ProjectHeader::exportRequested);
            connect(buildBtn, &QPushButton::clicked, [this, projectPath]() {
                Service::BackupService::getInstance().init(projectPath);
                Service::BackupService::getInstance().performBuildBackup("Automated build backup");
                emit buildRequested();
            });
        }

    signals:
        void geminiToggled();
        void browserToggled();
        void designerToggled();
        void buildRequested();
        void exportRequested();

    private:
        Service::GitService gitService;
    };
}
#endif

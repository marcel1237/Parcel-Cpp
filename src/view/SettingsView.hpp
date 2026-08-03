#ifndef SETTINGS_VIEW_HPP
#define SETTINGS_VIEW_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include "../service/PersistenceService.hpp"
#include "../core/navigation/NavigationController.hpp"

namespace Parcel::View {

    class SettingsView : public QWidget {
        Q_OBJECT
    public:
        explicit SettingsView(QWidget *parent = nullptr) : QWidget(parent) {
            auto* mainLayout = new QVBoxLayout(this);
            mainLayout->setContentsMargins(40, 40, 40, 40);
            mainLayout->setSpacing(20);

            auto* title = new QLabel("Configurações", this);
            title->setStyleSheet("font-size: 24px; font-weight: bold; color: white;");
            mainLayout->addWidget(title);

            // Gemini API Key Section
            auto* geminiBox = new QVBoxLayout();
            auto* geminiLabel = new QLabel("Gemini API Key (Criptografada localmente):", this);
            geminiLabel->setStyleSheet("font-size: 14px; color: #bbb;");
            geminiBox->addWidget(geminiLabel);

            apiKeyField = new QLineEdit(this);
            apiKeyField->setEchoMode(QLineEdit::Password);
            apiKeyField->setPlaceholderText("Insira sua API Key aqui...");
            apiKeyField->setStyleSheet("background-color: #2b2d30; color: white; border: 1px solid #4e5157; border-radius: 4px; padding: 8px;");

            // Carrega chave atual
            Service::PersistenceService persistence;
            std::string currentKey = persistence.getGeminiApiKey();
            if (!currentKey.empty()) {
                apiKeyField->setText(QString::fromStdString(currentKey));
            }

            geminiBox->addWidget(apiKeyField);
            mainLayout->addLayout(geminiBox);

            auto* infoLabel = new QLabel("Nota: Se deixado em branco, a IDE tentará ler do seu .bashrc.", this);
            infoLabel->setStyleSheet("font-size: 11px; color: #888; font-style: italic;");
            mainLayout->addWidget(infoLabel);

            mainLayout->addStretch();

            // Action Buttons
            auto* btnLayout = new QHBoxLayout();
            auto* saveBtn = new QPushButton("Salvar Configurações", this);
            saveBtn->setStyleSheet("background-color: #34A853; color: white; font-weight: bold; padding: 8px 20px; border-radius: 4px;");

            auto* backBtn = new QPushButton("Voltar", this);
            backBtn->setStyleSheet("background-color: #444; color: white; padding: 8px 20px; border-radius: 4px;");

            btnLayout->addWidget(saveBtn);
            btnLayout->addWidget(backBtn);
            btnLayout->addStretch();
            mainLayout->addLayout(btnLayout);

            connect(saveBtn, &QPushButton::clicked, this, &SettingsView::handleSave);
            connect(backBtn, &QPushButton::clicked, []() {
                Core::Navigation::NavigationController::getInstance().navigateTo(Core::Navigation::NavigationTarget::HOME);
            });
        }

    private slots:
        void handleSave() {
            Service::PersistenceService persistence;
            persistence.saveGeminiApiKey(apiKeyField->text().trimmed().toStdString());
            QMessageBox::information(this, "Sucesso", "Configurações salvas com sucesso!");
        }

    private:
        QLineEdit* apiKeyField;
    };
}

#endif

#ifndef KDE_PANE_HPP
#define KDE_PANE_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include "../service/KDEManager.hpp"

namespace Parcel::View {

    class KDEPane : public QWidget {
        Q_OBJECT
    public:
        explicit KDEPane(QWidget* parent = nullptr) : QWidget(parent) {
            auto* mainLayout = new QVBoxLayout(this);
            mainLayout->setContentsMargins(15, 15, 15, 15);
            mainLayout->setSpacing(12);

            auto* title = new QLabel("💙 KDE Frameworks 6 & Kirigami Studio", this);
            title->setStyleSheet("font-size: 16px; font-weight: bold; color: #1d99f3;");
            mainLayout->addWidget(title);

            auto* toolbar = new QHBoxLayout();
            auto* genQmlBtn = new QPushButton("Gerar App Kirigami QML", this);
            genQmlBtn->setStyleSheet("background-color: #1d99f3; color: white; padding: 6px 12px; border-radius: 4px;");
            toolbar->addWidget(genQmlBtn);

            auto* genCMakeBtn = new QPushButton("Gerar CMake KF6", this);
            genCMakeBtn->setStyleSheet("background-color: #2b2d30; color: white; padding: 6px 12px; border-radius: 4px;");
            toolbar->addWidget(genCMakeBtn);

            toolbar->addStretch();
            mainLayout->addLayout(toolbar);

            m_editor = new QTextEdit(this);
            m_editor->setStyleSheet("background-color: #1a1a1a; color: #1d99f3; font-family: 'Monospace'; font-size: 11px; padding: 8px; border-radius: 4px;");
            m_editor->setPlainText(Service::KDEManager::getInstance().generateKirigamiAppCode());
            mainLayout->addWidget(m_editor);

            connect(genQmlBtn, &QPushButton::clicked, [this]() {
                m_editor->setPlainText(Service::KDEManager::getInstance().generateKirigamiAppCode());
            });

            connect(genCMakeBtn, &QPushButton::clicked, [this]() {
                m_editor->setPlainText(Service::KDEManager::getInstance().generateKdeCMakeTemplate());
            });
        }

    private:
        QTextEdit* m_editor;
    };

}

#endif

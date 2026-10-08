#ifndef GTK2_PANE_HPP
#define GTK2_PANE_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include "../service/GTK2Manager.hpp"

namespace Parcel::View {

    class GTK2Pane : public QWidget {
        Q_OBJECT
    public:
        explicit GTK2Pane(QWidget* parent = nullptr) : QWidget(parent) {
            auto* mainLayout = new QVBoxLayout(this);
            mainLayout->setContentsMargins(15, 15, 15, 15);
            mainLayout->setSpacing(12);

            auto* title = new QLabel("🎨 GTK+ 2 Legacy Studio", this);
            title->setStyleSheet("font-size: 16px; font-weight: bold; color: #4078c0;");
            mainLayout->addWidget(title);

            auto* toolbar = new QHBoxLayout();
            auto* genAppBtn = new QPushButton("Gerar Código App GTK+ 2", this);
            genAppBtn->setStyleSheet("background-color: #4078c0; color: white; padding: 6px 12px; border-radius: 4px;");
            toolbar->addWidget(genAppBtn);

            toolbar->addStretch();
            mainLayout->addLayout(toolbar);

            m_editor = new QTextEdit(this);
            m_editor->setStyleSheet("background-color: #1a1a1a; color: #4078c0; font-family: 'Monospace'; font-size: 11px; padding: 8px; border-radius: 4px;");
            m_editor->setPlainText(Service::GTK2Manager::getInstance().generateGtk2AppCode());
            mainLayout->addWidget(m_editor);

            connect(genAppBtn, &QPushButton::clicked, [this]() {
                m_editor->setPlainText(Service::GTK2Manager::getInstance().generateGtk2AppCode());
            });
        }

    private:
        QTextEdit* m_editor;
    };

}

#endif

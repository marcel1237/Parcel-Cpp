#ifndef GTK4_PANE_HPP
#define GTK4_PANE_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include "../service/GTK4Manager.hpp"

namespace Parcel::View {

    class GTK4Pane : public QWidget {
        Q_OBJECT
    public:
        explicit GTK4Pane(QWidget* parent = nullptr) : QWidget(parent) {
            auto* mainLayout = new QVBoxLayout(this);
            mainLayout->setContentsMargins(15, 15, 15, 15);
            mainLayout->setSpacing(12);

            auto* title = new QLabel("🎨 GTK 4 Studio & Adwaita Theming", this);
            title->setStyleSheet("font-size: 16px; font-weight: bold; color: #3584e4;");
            mainLayout->addWidget(title);

            auto* toolbar = new QHBoxLayout();
            auto* genAppBtn = new QPushButton("Gerar Código App GTK 4", this);
            genAppBtn->setStyleSheet("background-color: #3584e4; color: white; padding: 6px 12px; border-radius: 4px;");
            toolbar->addWidget(genAppBtn);

            auto* genCssBtn = new QPushButton("Gerar Tema CSS Adwaita", this);
            genCssBtn->setStyleSheet("background-color: #2b2d30; color: white; padding: 6px 12px; border-radius: 4px;");
            toolbar->addWidget(genCssBtn);

            toolbar->addStretch();
            mainLayout->addLayout(toolbar);

            m_editor = new QTextEdit(this);
            m_editor->setStyleSheet("background-color: #1a1a1a; color: #3584e4; font-family: 'Monospace'; font-size: 11px; padding: 8px; border-radius: 4px;");
            m_editor->setPlainText(Service::GTK4Manager::getInstance().generateGtk4AppCode());
            mainLayout->addWidget(m_editor);

            connect(genAppBtn, &QPushButton::clicked, [this]() {
                m_editor->setPlainText(Service::GTK4Manager::getInstance().generateGtk4AppCode());
            });

            connect(genCssBtn, &QPushButton::clicked, [this]() {
                m_editor->setPlainText(Service::GTK4Manager::getInstance().generateGtk4AppCode() + "\n\n" + Service::GTK4Manager::getInstance().generateGtkCssTheme());
            });
        }

    private:
        QTextEdit* m_editor;
    };

}

#endif

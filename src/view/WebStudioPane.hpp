#ifndef WEB_STUDIO_PANE_HPP
#define WEB_STUDIO_PANE_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include "../service/JavaScriptWebManager.hpp"
#include "../service/PhpWebManager.hpp"

namespace Parcel::View {

    class WebStudioPane : public QWidget {
        Q_OBJECT
    public:
        explicit WebStudioPane(QWidget* parent = nullptr) : QWidget(parent) {
            auto* mainLayout = new QVBoxLayout(this);
            mainLayout->setContentsMargins(15, 15, 15, 15);
            mainLayout->setSpacing(12);

            auto* title = new QLabel("🌐 JavaScript (jQuery, Vue, Angular, React) & PHP (Laravel, WordPress) Studio", this);
            title->setStyleSheet("font-size: 16px; font-weight: bold; color: #F7DF1E;");
            mainLayout->addWidget(title);

            auto* toolbar = new QHBoxLayout();

            auto* btnJQuery = new QPushButton("jQuery 3.7", this);
            btnJQuery->setStyleSheet("background-color: #0769AD; color: white; padding: 6px 10px; border-radius: 4px;");
            toolbar->addWidget(btnJQuery);

            auto* btnVue = new QPushButton("Vue.js 3", this);
            btnVue->setStyleSheet("background-color: #42B883; color: white; padding: 6px 10px; border-radius: 4px;");
            toolbar->addWidget(btnVue);

            auto* btnAngular = new QPushButton("Angular 17+", this);
            btnAngular->setStyleSheet("background-color: #DD0031; color: white; padding: 6px 10px; border-radius: 4px;");
            toolbar->addWidget(btnAngular);

            auto* btnReact = new QPushButton("React 18", this);
            btnReact->setStyleSheet("background-color: #61DAFB; color: black; font-weight: bold; padding: 6px 10px; border-radius: 4px;");
            toolbar->addWidget(btnReact);

            auto* btnPhp = new QPushButton("PHP 8.3", this);
            btnPhp->setStyleSheet("background-color: #777BB4; color: white; padding: 6px 10px; border-radius: 4px;");
            toolbar->addWidget(btnPhp);

            auto* btnLaravel = new QPushButton("Laravel 11", this);
            btnLaravel->setStyleSheet("background-color: #FF2D20; color: white; padding: 6px 10px; border-radius: 4px;");
            toolbar->addWidget(btnLaravel);

            auto* btnWordPress = new QPushButton("WordPress", this);
            btnWordPress->setStyleSheet("background-color: #21759B; color: white; padding: 6px 10px; border-radius: 4px;");
            toolbar->addWidget(btnWordPress);

            toolbar->addStretch();
            mainLayout->addLayout(toolbar);

            m_editor = new QTextEdit(this);
            m_editor->setStyleSheet("background-color: #1a1a1a; color: #F7DF1E; font-family: 'Monospace'; font-size: 11px; padding: 8px; border-radius: 4px;");
            m_editor->setPlainText(Service::JavaScriptWebManager::getInstance().generateJQueryCode());
            mainLayout->addWidget(m_editor);

            connect(btnJQuery, &QPushButton::clicked, [this]() {
                m_editor->setPlainText(Service::JavaScriptWebManager::getInstance().generateJQueryCode());
            });

            connect(btnVue, &QPushButton::clicked, [this]() {
                m_editor->setPlainText(Service::JavaScriptWebManager::getInstance().generateVueCode());
            });

            connect(btnAngular, &QPushButton::clicked, [this]() {
                m_editor->setPlainText(Service::JavaScriptWebManager::getInstance().generateAngularCode());
            });

            connect(btnReact, &QPushButton::clicked, [this]() {
                m_editor->setPlainText(Service::JavaScriptWebManager::getInstance().generateReactCode());
            });

            connect(btnPhp, &QPushButton::clicked, [this]() {
                m_editor->setPlainText(Service::PhpWebManager::getInstance().generateNativePhpCode());
            });

            connect(btnLaravel, &QPushButton::clicked, [this]() {
                m_editor->setPlainText(Service::PhpWebManager::getInstance().generateLaravelCode());
            });

            connect(btnWordPress, &QPushButton::clicked, [this]() {
                m_editor->setPlainText(Service::PhpWebManager::getInstance().generateWordPressCode());
            });
        }

    private:
        QTextEdit* m_editor;
    };

}

#endif

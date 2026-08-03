#ifndef DASHBOARD_VIEW_HPP
#define DASHBOARD_VIEW_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QIcon>
#include "../core/navigation/NavigationController.hpp"

namespace Parcel::View {
    class DashboardView : public QWidget {
        Q_OBJECT
    public:
        explicit DashboardView(QWidget *parent = nullptr) : QWidget(parent) {
            auto* layout = new QVBoxLayout(this);
            layout->setAlignment(Qt::AlignCenter);

            auto* title = new QLabel("Bem-vindo ao Parcel C++", this);
            title->setStyleSheet("font-size: 24px; font-weight: bold;");

            auto* btnNew = new QPushButton("New Project", this);
            btnNew->setIcon(QIcon::fromTheme("document-new"));
            connect(btnNew, &QPushButton::clicked, []() {
                Core::Navigation::NavigationController::getInstance().navigateTo(
                    Core::Navigation::NavigationTarget::NEW_PROJECT);
            });

            auto* btnOpen = new QPushButton("Open Project", this);
            btnOpen->setIcon(QIcon::fromTheme("document-open"));
            connect(btnOpen, &QPushButton::clicked, []() {
                Core::Navigation::NavigationController::getInstance().navigateTo(
                    Core::Navigation::NavigationTarget::OPEN_PROJECT);
            });

            layout->addWidget(title);
            layout->addWidget(btnNew);
            layout->addWidget(btnOpen);
        }
    };
}
#endif

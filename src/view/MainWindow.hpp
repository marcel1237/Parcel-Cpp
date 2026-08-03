#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QMainWindow>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QTimer>
#include "../core/navigation/NavigationController.hpp"
#include "DashboardView.hpp"
#include "NewProjectView.hpp"
#include "OpenProjectView.hpp"
#include "ProjectTypeView.hpp"
#include "ProjectWorkspace.hpp"
#include "SettingsView.hpp"
#include "../service/PersistenceService.hpp"

namespace Parcel::View {

    class StatusBar : public QHBoxLayout {
    public:
        explicit StatusBar(QWidget* parent) {
            auto* status = new QLabel("Ready", parent);
            auto* version = new QLabel("1.0.0", parent);
            status->setStyleSheet("color: #888;");
            version->setStyleSheet("color: #888;");

            addWidget(status);
            addStretch();
            addWidget(version);
        }
    };

    class MainToolBar : public QToolBar {
    public:
        explicit MainToolBar(QWidget* parent) : QToolBar(parent) {
            auto* newAction = addAction(QIcon::fromTheme("document-new"), "New Project");
            auto* openAction = addAction(QIcon::fromTheme("document-open"), "Open");
            auto* newPdfAction = addAction(QIcon::fromTheme("document-new"), "New PDF");
            addSeparator();
            auto* settingsAction = addAction(QIcon::fromTheme("preferences-system"), "Settings");

            connect(newAction, &QAction::triggered, []() {
                Core::Navigation::NavigationController::getInstance().navigateTo(Core::Navigation::NavigationTarget::NEW_PROJECT);
            });
            connect(openAction, &QAction::triggered, []() {
                Core::Navigation::NavigationController::getInstance().navigateTo(Core::Navigation::NavigationTarget::OPEN_PROJECT);
            });
            connect(settingsAction, &QAction::triggered, []() {
                Core::Navigation::NavigationController::getInstance().navigateTo(Core::Navigation::NavigationTarget::SETTINGS);
            });
            connect(newPdfAction, &QAction::triggered, this, &MainToolBar::createNewPdf);
        }

    private:
        void createNewPdf() {
            // Signal a request to the current workspace to create a PDF
            // For now, we can use the existing navigation to send a path-less workspace or just let EditorHost handle it
            // Better: find the active ProjectWorkspace and call a method
            // Simplified: we'll add it to the active editor host if available
        }
    };

    class MainWindow : public QMainWindow {
        Q_OBJECT
    public:
        explicit MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
            setWindowTitle("Parcel C++");
            resize(1400, 850);

            // Menu e ToolBar
            setupMenus();
            auto* toolBar = new MainToolBar(this);
            addToolBar(Qt::TopToolBarArea, toolBar);

            auto* centralContainer = new QWidget(this);
            auto* vLayout = new QVBoxLayout(centralContainer);
            vLayout->setContentsMargins(0, 0, 0, 0);
            vLayout->setSpacing(0);

            auto* mainContent = new QWidget(this);
            auto* hLayout = new QHBoxLayout(mainContent);
            hLayout->setContentsMargins(0, 0, 0, 0);
            hLayout->setSpacing(0);

            contentStack = new QStackedWidget(this);
            hLayout->addWidget(contentStack);

            vLayout->addWidget(mainContent);

            // StatusBar
            auto* statusWidget = new QWidget(this);
            statusWidget->setFixedHeight(25);
            statusWidget->setStyleSheet("background-color: #222; border-top: 1px solid #333;");
            auto* statusLayout = new StatusBar(statusWidget);
            statusLayout->setContentsMargins(10, 0, 10, 0);
            statusWidget->setLayout(statusLayout);
            vLayout->addWidget(statusWidget);

            setCentralWidget(centralContainer);

            // Initialize views
            dashboardView = new DashboardView(this);
            openProjectView = new OpenProjectView(this);
            newProjectView = new NewProjectView(this);
            settingsView = new SettingsView(this);

            contentStack->addWidget(dashboardView);    // 0
            contentStack->addWidget(openProjectView);  // 1
            contentStack->addWidget(newProjectView);   // 2
            contentStack->addWidget(settingsView);     // 3

            setupNavigation();

            // Restore session
            QTimer::singleShot(0, this, &MainWindow::restoreSession);
        }

    private:
        void setupNavigation() {
            Core::Navigation::NavigationController::getInstance().setOnNavigate([this](auto target, const std::string& path) {
                switch (target) {
                    case Core::Navigation::NavigationTarget::HOME:
                        contentStack->setCurrentWidget(dashboardView);
                        break;
                    case Core::Navigation::NavigationTarget::OPEN_PROJECT:
                        contentStack->setCurrentWidget(openProjectView);
                        break;
                    case Core::Navigation::NavigationTarget::NEW_PROJECT:
                        contentStack->setCurrentWidget(newProjectView);
                        break;
                    case Core::Navigation::NavigationTarget::SETTINGS:
                        contentStack->setCurrentWidget(settingsView);
                        break;
                    case Core::Navigation::NavigationTarget::WORKSPACE:
                        loadWorkspace(path);
                        break;
                    default:
                        break;
                }
            });
        }

        void loadWorkspace(const std::string& path) {
            if (path.empty()) return;

            Service::PersistenceService persistence;
            persistence.saveLastProject(path);

            // Remove o workspace anterior se existir para evitar vazamento de memória e processos
            if (activeWorkspace) {
                contentStack->removeWidget(activeWorkspace);
                activeWorkspace->deleteLater();
            }

            activeWorkspace = new ProjectWorkspace(path, this);
            int index = contentStack->addWidget(activeWorkspace);
            contentStack->setCurrentIndex(index);
        }

        void restoreSession() {
            Service::PersistenceService persistence;
            std::string lastProject = persistence.getLastProject();
            if (!lastProject.empty() && std::filesystem::exists(lastProject)) {
                Core::Navigation::NavigationController::getInstance().navigateTo(
                    Core::Navigation::NavigationTarget::WORKSPACE, lastProject);
            }
        }

        void setupMenus() {
            auto* fileMenu = menuBar()->addMenu("&File");
            auto* exitAction = fileMenu->addAction("&Exit");
            connect(exitAction, &QAction::triggered, this, &QWidget::close);

            auto* helpMenu = menuBar()->addMenu("&Help");
            helpMenu->addAction("&About");
        }

        QStackedWidget* contentStack;
        DashboardView* dashboardView;
        OpenProjectView* openProjectView;
        NewProjectView* newProjectView;
        SettingsView* settingsView;
        ProjectWorkspace* activeWorkspace = nullptr;
    };
}
#endif

#ifndef NEW_PROJECT_VIEW_HPP
#define NEW_PROJECT_VIEW_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QFrame>
#include "../core/navigation/NavigationController.hpp"

namespace Parcel::View {

    class NewProjectView : public QWidget {
        Q_OBJECT
    public:
        explicit NewProjectView(QWidget *parent = nullptr) : QWidget(parent) {
            auto* mainLayout = new QHBoxLayout(this);
            mainLayout->setContentsMargins(20, 20, 20, 20);
            mainLayout->setSpacing(30);

            // Painel Esquerdo (Passos)
            auto* stepsPanel = new QVBoxLayout();
            stepsPanel->setSpacing(15);
            QStringList steps = {"① Project", "② Language", "③ Framework", "④ Dependencies", "⑤ Git", "⑥ Summary"};
            for (const auto& step : steps) {
                auto* label = new QLabel(step, this);
                label->setStyleSheet("font-size: 15px; padding: 8px; color: #bbb;");
                stepsPanel->addWidget(label);
            }
            stepsPanel->addStretch();
            mainLayout->addLayout(stepsPanel, 1);

            // Painel Central (Formulário)
            auto* centerLayout = new QVBoxLayout();
            auto* title = new QLabel("Create New Project", this);
            title->setStyleSheet("font-size: 26px; font-weight: bold; margin-bottom: 20px;");
            centerLayout->addWidget(title);

            auto* formGrid = new QGridLayout();
            formGrid->setVerticalSpacing(15);
            formGrid->setHorizontalSpacing(10);

            formGrid->addWidget(new QLabel("Project Name:", this), 0, 0);
            formGrid->addWidget(new QLineEdit(this), 0, 1);

            formGrid->addWidget(new QLabel("Group:", this), 1, 0);
            formGrid->addWidget(new QLineEdit("com.example", this), 1, 1);

            formGrid->addWidget(new QLabel("Artifact:", this), 2, 0);
            formGrid->addWidget(new QLineEdit(this), 2, 1);

            formGrid->addWidget(new QLabel("Language:", this), 3, 0);
            auto* langCombo = new QComboBox(this);
            langCombo->addItems({"Java", "Kotlin", "Groovy", "C++"});
            formGrid->addWidget(langCombo, 3, 1);

            formGrid->addWidget(new QLabel("Build Tool:", this), 4, 0);
            auto* buildCombo = new QComboBox(this);
            buildCombo->addItems({"Maven", "Gradle", "CMake"});
            formGrid->addWidget(buildCombo, 4, 1);

            centerLayout->addLayout(formGrid);
            centerLayout->addSpacing(30);

            auto* separator = new QFrame();
            separator->setFrameShape(QFrame::HLine);
            separator->setFrameShadow(QFrame::Sunken);
            centerLayout->addWidget(separator);

            // Botões de Ação
            auto* btnLayout = new QHBoxLayout();
            auto* backBtn = new QPushButton("◀ Back", this);
            backBtn->setIcon(QIcon::fromTheme("go-previous"));
            auto* nextBtn = new QPushButton("Next ▶", this);
            nextBtn->setIcon(QIcon::fromTheme("go-next"));
            auto* finishBtn = new QPushButton("Finish", this);
            finishBtn->setIcon(QIcon::fromTheme("emblem-ok"));
            finishBtn->setStyleSheet("background-color: #0078d4; color: white; font-weight: bold; padding: 6px 15px;");

            btnLayout->addStretch();
            btnLayout->addWidget(backBtn);
            btnLayout->addWidget(nextBtn);
            btnLayout->addWidget(finishBtn);
            centerLayout->addLayout(btnLayout);
            centerLayout->addStretch();

            mainLayout->addLayout(centerLayout, 3);

            connect(backBtn, &QPushButton::clicked, []() {
                Core::Navigation::NavigationController::getInstance().navigateTo(Core::Navigation::NavigationTarget::HOME);
            });
        }
    };
}
#endif

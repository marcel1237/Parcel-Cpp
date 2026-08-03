#ifndef PROJECT_TYPE_VIEW_HPP
#define PROJECT_TYPE_VIEW_HPP

#include <QWidget>
#include <QGridLayout>
#include <QScrollArea>
#include <QLabel>
#include <QVBoxLayout>
#include <QFrame>

namespace Parcel::View {
    class ProjectCard : public QFrame {
    public:
        ProjectCard(const QString& title, const QString& desc) {
            setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
            auto* l = new QVBoxLayout(this);
            l->addWidget(new QLabel("<b>" + title + "</b>"));
            l->addWidget(new QLabel(desc));
        }
    };

    class ProjectTypeView : public QWidget {
        Q_OBJECT
    public:
        explicit ProjectTypeView(QWidget *parent = nullptr) : QWidget(parent) {
            auto* mainLayout = new QVBoxLayout(this);
            auto* grid = new QGridLayout();

            grid->addWidget(new ProjectCard("Java Maven", "Projeto padrão Maven"), 0, 0);
            grid->addWidget(new ProjectCard("C++ CMake", "Projeto nativo Linux"), 0, 1);
            grid->addWidget(new ProjectCard("Python", "Ambiente virtual isolado"), 1, 0);

            mainLayout->addLayout(grid);
        }
    };
}
#endif

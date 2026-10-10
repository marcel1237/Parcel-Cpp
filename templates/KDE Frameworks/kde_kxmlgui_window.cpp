// KDE KXmlGuiWindow Main Window Template
#include <KXmlGuiWindow>
#include <KActionCollection>
#include <QAction>
#include <QIcon>
#include <QDebug>

class ParcelKdeMainWindow : public KXmlGuiWindow {
    Q_OBJECT
public:
    explicit ParcelKdeMainWindow(QWidget* parent = nullptr) : KXmlGuiWindow(parent) {
        setupActions();
    }

private:
    void setupActions() {
        QAction* newAction = actionCollection()->addAction("file_new");
        newAction->setText("New File");
        newAction->setIcon(QIcon::fromTheme("document-new"));

        connect(newAction, &QAction::triggered, this, []() {
            qDebug() << "New File action triggered in KXmlGuiWindow!";
        });

        setupGUI(Default, "parcel_kde_ui.rc");
    }
};

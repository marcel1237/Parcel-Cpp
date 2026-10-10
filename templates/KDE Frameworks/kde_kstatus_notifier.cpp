// KDE KStatusNotifierItem System Tray Template
#include <KStatusNotifierItem>
#include <QMenu>
#include <QAction>
#include <QDebug>

KStatusNotifierItem* setupKdeSystemTray(QObject* parent) {
    auto* tray = new KStatusNotifierItem("parcel_cpp_tray", parent);
    tray->setIconByName("applications-development");
    tray->setToolTipTitle("Parcel C++ IDE");
    tray->setToolTipSubTitle("Background AI Agent Running");
    tray->setStatus(KStatusNotifierItem::Active);

    QMenu* menu = tray->contextMenu();
    QAction* openAction = menu->addAction("Open IDE Workspace");
    QObject::connect(openAction, &QAction::triggered, []() {
        qDebug() << "Tray icon clicked: Opening workspace...";
    });

    return tray;
}

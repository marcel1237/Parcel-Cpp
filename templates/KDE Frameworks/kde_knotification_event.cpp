// KDE KNotification Desktop Notification Template
#include <KNotification>
#include <QIcon>
#include <QDebug>

void sendKdeNotification(const QString& title, const QString& message) {
    KNotification* notification = new KNotification("parcel_event", KNotification::CloseOnTimeout);
    notification->setTitle(title);
    notification->setText(message);
    notification->setIconName("dialog-information");

    QObject::connect(notification, &KNotification::closed, notification, &QObject::deleteLater);
    notification->sendEvent();
}

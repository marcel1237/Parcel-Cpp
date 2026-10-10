// KDE KIconLoader Theme Icon Scaling Template
#include <KIconLoader>
#include <QIcon>
#include <QPixmap>
#include <QDebug>

QPixmap getKdeScaledIcon(const QString& iconName, int size) {
    KIconLoader* loader = KIconLoader::global();
    QPixmap pixmap = loader->loadIcon(iconName, KIconLoader::Desktop, size);

    if (pixmap.isNull()) {
        qDebug() << "Fallback to standard theme for:" << iconName;
        pixmap = QIcon::fromTheme(iconName).pixmap(size, size);
    }

    return pixmap;
}

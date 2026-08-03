#include <QApplication>
#include <QSurfaceFormat>
#include <QIcon>
#include <QPixmap>
#include <QPainter>
#include <QLinearGradient>
#include <QFont>
#include <QDir>
#include <QStyleFactory>
#include <QPalette>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include "view/MainWindow.hpp"
#include "view/Theme.hpp"

static QIcon createApplicationIcon() {
    const int size = 512;
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    QLinearGradient gradient(0, 0, size, size);
    gradient.setColorAt(0, QColor("#1a1a1a"));
    gradient.setColorAt(0.5, QColor("#050505"));
    gradient.setColorAt(1, QColor("#1a1a1a"));

    painter.setBrush(gradient);
    painter.setPen(QPen(QColor("#4285F4"), 20));
    painter.drawRoundedRect(10, 10, size-20, size-20, size/5, size/5);

    painter.setPen(Qt::white);
    QFont font("Sans Serif", size/3, QFont::Bold);
    painter.setFont(font);
    painter.setPen(QColor(66, 133, 244, 100));
    painter.drawText(QRect(4, 4, size, size), Qt::AlignCenter, "PC");
    painter.setPen(QColor("#ffffff"));
    painter.drawText(QRect(0, 0, size, size), Qt::AlignCenter, "PC");

    QLinearGradient glass(0, 0, 0, size / 3);
    glass.setColorAt(0, QColor(66, 133, 244, 40));
    glass.setColorAt(1, QColor(66, 133, 244, 0));
    painter.setBrush(glass);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(15, 15, size-30, size/3, size/5, size/5);
    painter.end();

    QString homeIconPath = QDir::homePath() + "/.local/share/icons/parcel-cpp-dark.png";
    QDir().mkpath(QDir::homePath() + "/.local/share/icons");
    pixmap.save(homeIconPath, "PNG");

    QDir projectDir(QCoreApplication::applicationDirPath());
    if (projectDir.dirName() == "build") projectDir.cdUp();
    QString resourcePath = projectDir.absolutePath() + "/resource/parcel-cpp-dark.png";
    projectDir.mkpath("resource");
    pixmap.save(resourcePath, "PNG");
    pixmap.save(QDir::currentPath() + "/parcel-cpp-dark.png", "PNG");

    return QIcon(pixmap);
}

int main(int argc, char *argv[]) {
    qputenv("QTWEBENGINE_DISABLE_SANDBOX", "1");
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--log-level=3 --enable-logging=none");
    qputenv("QT_LOGGING_RULES", "qt.webenginecontext.debug=false;*.debug=false");

    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")) {
        qputenv("QT_QPA_PLATFORM", "xcb");
    }

    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create("Fusion"));

    // --- APPLY EXTRAORDINARY SKIN ---
    Parcel::View::Theme::applyExtraordinarySkin(app);

    app.setApplicationName("Parcel C++");
    app.setApplicationDisplayName("Parcel C++");
    app.setOrganizationName("Parcel");
    app.setDesktopFileName("Parcel C++");

    QIcon appIcon = createApplicationIcon();
    app.setWindowIcon(appIcon);

    QWebEngineProfile* profile = QWebEngineProfile::defaultProfile();
    profile->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    profile->settings()->setAttribute(QWebEngineSettings::AutoLoadImages, true);
    profile->settings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true);

    Parcel::View::MainWindow mainWindow;
    mainWindow.setWindowTitle("Parcel C++");
    mainWindow.setWindowIcon(appIcon);
    mainWindow.show();

    return app.exec();
}

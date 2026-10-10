// KDE KConfig / KSharedConfig C++ Template
#include <KConfigGroup>
#include <KSharedConfig>
#include <QString>
#include <QDebug>

void loadKdeSettings() {
    KSharedConfig::Ptr config = KSharedConfig::openConfig("parcel_cpp_kde.conf");
    KConfigGroup generalGroup(config, "General");

    QString lastProject = generalGroup.readEntry("LastProject", "/home/user/project");
    bool darkMode = generalGroup.readEntry("DarkMode", true);

    qDebug() << "KDE Config Loaded -> Last Project:" << lastProject << "Dark Mode:" << darkMode;
}

void saveKdeSettings(const QString& lastProject, bool darkMode) {
    KSharedConfig::Ptr config = KSharedConfig::openConfig("parcel_cpp_kde.conf");
    KConfigGroup generalGroup(config, "General");

    generalGroup.writeEntry("LastProject", lastProject);
    generalGroup.writeEntry("DarkMode", darkMode);
    config->sync();
}

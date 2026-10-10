// KDE KPluginMetaData & Dynamic Plugin Loader Template
#include <KPluginMetaData>
#include <KPluginFactory>
#include <QPluginLoader>
#include <QDebug>

void loadKdePlugins(const QString& pluginPath) {
    KPluginMetaData metaData(pluginPath);
    if (!metaData.isValid()) {
        qDebug() << "Invalid KDE Plugin metadata:" << pluginPath;
        return;
    }

    qDebug() << "Loading KDE Plugin:" << metaData.name() << "Version:" << metaData.version();

    auto result = KPluginFactory::loadFactory(metaData);
    if (result.plugin) {
        qDebug() << "Plugin factory loaded successfully!";
    } else {
        qDebug() << "Failed to load plugin factory:" << result.errorString;
    }
}

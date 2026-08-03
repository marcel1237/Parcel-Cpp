#include "ProjectKnowledge.hpp"
#include <QFileInfo>

namespace SSAI {

ProjectKnowledge& ProjectKnowledge::getInstance() {
    static ProjectKnowledge instance;
    return instance;
}

void ProjectKnowledge::recordModifiedFile(const QString& path) {
    m_recentFiles.removeAll(path);
    m_recentFiles.prepend(path);
    if (m_recentFiles.size() > 20) m_recentFiles.removeLast();
    m_lastSeen[path] = QDateTime::currentDateTime();
}

void ProjectKnowledge::registerSymbol(const QString& name, const QString& filePath, const QString& type) {
    QString lowerName = name.toLower();
    m_symbolMap[lowerName] = filePath;
    m_symbolTypeMap[lowerName] = type;
}

QString ProjectKnowledge::findSymbolLocation(const QString& name) const {
    return m_symbolMap.value(name.toLower(), "");
}

QString ProjectKnowledge::getSymbolType(const QString& name) const {
    return m_symbolTypeMap.value(name.toLower(), "unknown");
}

void ProjectKnowledge::addTask(const QString& description) {
    m_pendingTasks << description;
}

void ProjectKnowledge::completeTask(int index) {
    if (index >= 0 && index < m_pendingTasks.size()) {
        m_pendingTasks.removeAt(index);
    }
}

}

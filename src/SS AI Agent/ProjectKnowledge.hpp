#ifndef PROJECT_KNOWLEDGE_HPP
#define PROJECT_KNOWLEDGE_HPP

#include <QObject>
#include <QStringList>
#include <QMap>
#include <QDateTime>

namespace SSAI {

    /**
     * @brief Stores "learned" project information for the SS AI Agent.
     * This acts as the short-term memory of the LocalAgent.
     */
    class ProjectKnowledge : public QObject {
        Q_OBJECT
    public:
        static ProjectKnowledge& getInstance();

        void recordModifiedFile(const QString& path);
        void registerSymbol(const QString& name, const QString& filePath, const QString& type = "class");

        QStringList recentFiles() const { return m_recentFiles; }
        QString findSymbolLocation(const QString& name) const;
        QString getSymbolType(const QString& name) const;

        // Task Tracking for Autonomous Plans
        void addTask(const QString& description);
        void completeTask(int index);
        QStringList pendingTasks() const { return m_pendingTasks; }
        void clearTasks() { m_pendingTasks.clear(); }

        void setProjectRoot(const QString& root) { m_projectRoot = root; }
        QString projectRoot() const { return m_projectRoot; }

    private:
        explicit ProjectKnowledge(QObject* parent = nullptr) : QObject(parent) {}

        QString m_projectRoot;
        QStringList m_recentFiles;
        QStringList m_pendingTasks;
        QMap<QString, QString> m_symbolMap;
        QMap<QString, QString> m_symbolTypeMap;
        QMap<QString, QDateTime> m_lastSeen;
    };

}

#endif // PROJECT_KNOWLEDGE_HPP

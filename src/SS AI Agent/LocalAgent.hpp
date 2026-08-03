#ifndef LOCAL_AGENT_HPP
#define LOCAL_AGENT_HPP

#include <QObject>
#include <QString>
#include <QList>
#include "QLMEngine.hpp"

namespace SSAI {

    /**
     * @brief The LocalAgent class represents the on-device intelligence.
     * It combines rule-based QLM with extraordinary SSQLM logic.
     */
    class LocalAgent : public QObject {
        Q_OBJECT
    public:
        explicit LocalAgent(QObject* parent = nullptr);

        struct AgentResponse {
            bool handled;
            QString message;
            QString action;
            QString target;
        };

        /**
         * @brief Processes an instruction using local knowledge.
         */
        AgentResponse process(const QString& instruction);

        /**
         * @brief Reloads all local knowledge bases.
         */
        void reload();

        /**
         * @brief Performs a local audit on a snippet of code.
         */
        QString auditCode(const QString& code);

        /**
         * @brief Scans the project root to index symbols.
         */
        void indexProject(const QString& rootPath);

        /**
         * @brief Gets autonomous suggestions based on project patterns.
         */
        QString getSuggestions(const QString& context);

        /**
         * @brief Generates a specialized SSQLM shell script.
         */
        QString generateScript(const QString& type);

        /**
         * @brief Generates a full C++ class (HPP/CPP) based on project standards.
         */
        QString generateClass(const QString& name, const QString& type);

        /**
         * @brief Analyzes a log snippet for errors and patterns.
         */
        QString analyzeLogs(const QString& logContent);

        /**
         * @brief Analyzes the entire project and returns stats.
         */
        QString getProjectStats();

        /**
         * @brief Proposes a code fix for a common pattern.
         */
        QString proposeFix(const QString& issue);

        /**
         * @brief Performs a full project health scan.
         */
        QString runHealthCheck();

        /**
         * @brief Performs a security audit for sensitive data and unsafe patterns.
         */
        QString runSecurityScan();

        /**
         * @brief Automatically fixes detected issues in a file (Auto-Healing).
         */
        QString autoHeal(const QString& path);

        /**
         * @brief Analyzes include dependencies for a specific file.
         */
        QString analyzeDependencies(const QString& path);

        /**
         * @brief Executes a Git command through the agent.
         */
        QString gitOperation(const QString& command, const QString& args = "");

        /**
         * @brief Searches for text across the entire project.
         */
        QString searchProject(const QString& query);

        /**
         * @brief Performs a regex-based search for advanced patterns.
         */
        QString regexSearch(const QString& pattern);

        /**
         * @brief Scans the project for TODOs, FIXMEs and technical debt.
         */
        QString findTechnicalDebt();

        /**
         * @brief Generates technical documentation for a file.
         */
        QString documentFile(const QString& path);

        /**
         * @brief Automatically refactors a file to follow SSQLM standards.
         */
        QString refactorFile(const QString& path);

        /**
         * @brief Refactors the entire project based on a specific rule.
         */
        QString refactorProject(const QString& rule);

        /**
         * @brief Analyzes code complexity and structural risk.
         */
        QString analyzeRisk(const QString& path);

        /**
         * @brief Returns the last generated artifact (script or plan).
         */
        QString lastArtifact() const { return m_lastArtifact; }
        void setLastArtifact(const QString& artifact) { m_lastArtifact = artifact; }

        /**
         * @brief Starts the Auto-Pilot mode, suggesting the next task in the queue.
         */
        QString runAutoPilot();

    private:
        QLMEngine* m_qlm;
        QLMEngine* m_ssqlm;
        QString m_lastArtifact;

        void scanDirectory(const QString& path);
        QString generateSsqlmPlan(const QString& task);
    };

}

#endif // LOCAL_AGENT_HPP

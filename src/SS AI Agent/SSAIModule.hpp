#ifndef SSAI_MODULE_HPP
#define SSAI_MODULE_HPP

#include <QObject>
#include <QString>
#include <QDebug>
#include <QJsonObject>
#include <QList>
#include "FilePatcher.hpp"
#include "GeminiClient.hpp"
#include "OpenAIClient.hpp"
#include "QLMEngine.hpp"
#include "PythonExecutor.hpp"
#include "LocalAgent.hpp"
#include "MLService.hpp"
#include "WebSearchService.hpp"

namespace SSAI {

    class SSAIModule : public QObject {
        Q_OBJECT
    public:
        explicit SSAIModule(QObject* parent = nullptr);

        struct QLMResponse {
            enum Type { MESSAGE, ACTION, ERROR, ASYNC_WAIT };
            Type type;
            QString message;
            QString actionType;
            QString targetFile;
        };

        QLMResponse processInstruction(const QString& instruction);
        QString requestAnalysis(const QString& path);

        // Web Search
        QString requestWebSearch(const QString& query);
        QString requestUrlContent(const QString& url);
        QString requestFullResearch(const QString& topic);
        QString requestWebAutomation(const QString& task);
        QString saveResearchResult(const QString& topic, const QString& content);

        // ML Operations
        QString requestMLPrediction(const QString& code);

        // Extraordinary SSQLM Reasoning
        QString generateExtraordinaryPlan(const QString& task);

        // Session Management
        void setCurrentFile(const QString& path) { m_currentFile = path; }
        QString currentFile() const { return m_currentFile; }

        // SSQLM methods
        FilePatcher::PatchStatus requestFullBuild(const QString& path, const QString& content) { return FilePatcher::reconstruct(path, content); }
        FilePatcher::PatchStatus requestSafeCreate(const QString& path, const QString& content) { return FilePatcher::safeCreate(path, content); }
        FilePatcher::PatchStatus requestSurgicalPatch(const QString& path, const QString& start, const QString& end, const QString& patch) { return FilePatcher::patch(path, start, end, patch); }
        FilePatcher::PatchStatus requestSmartAppend(const QString& path, const QString& content, const QString& marker) { return FilePatcher::smartAppend(path, content, marker); }

        // Python Execution
        PythonExecutor::ExecutionResult requestPythonExecution(const QString& content) { return PythonExecutor::executeScript(content); }

    signals:
        void geminiResponseReceived(const QString& response);
        void geminiErrorOccurred(const QString& error);
        void openaiResponseReceived(const QString& response);
        void openaiErrorOccurred(const QString& error);

    private:
        QString m_qlmDir;
        QString m_currentFile;
        QString m_lastAiResponse;
        QLMEngine* m_qlmEngine;
        LocalAgent* m_localAgent;
        MLService* m_mlService;
        WebSearchService* m_webSearch;
        GeminiClient* m_gemini;
        OpenAIClient* m_openai;
    };

}

#endif // SSAI_MODULE_HPP

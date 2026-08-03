#ifndef ML_SERVICE_HPP
#define ML_SERVICE_HPP

#include <QObject>
#include <QString>
#include <QStringList>
#include "PythonExecutor.hpp"

namespace SSAI {

    /**
     * @brief Specialized service for Local Machine Learning tasks.
     * Integrates Python ML libraries (scikit-learn, numpy) with the C++ core.
     */
    class MLService : public QObject {
        Q_OBJECT
    public:
        explicit MLService(QObject* parent = nullptr);

        struct MLResult {
            bool success;
            QString prediction;
            double confidence;
            QString details;
        };

        /**
         * @brief Trains a simple model based on project patterns (Simulated).
         */
        MLResult trainLocalModel(const QString& dataPath);

        /**
         * @brief Predicts the type of a code block using local ML logic.
         */
        MLResult predictCodeContext(const QString& codeSnippet);

        /**
         * @brief Specialized: Detects SSQLM code style violations using ML.
         */
        MLResult analyzeStyleML(const QString& code);

        /**
         * @brief Specialized: Predicts the likelihood of bugs in a logic block.
         */
        MLResult predictBugRisk(const QString& code);

        /**
         * @brief Performs a semantic search across indexed code using ML.
         */
        MLResult semanticSearch(const QString& query, const QMap<QString, QString>& corpus);

        /**
         * @brief Performs anomaly detection on project structure.
         */
        MLResult runAnomalyDetection();

    signals:
        void mlTaskFinished(const QString& taskName, bool success);

    private:
        QString runPythonML(const QString& script);
    };

}

#endif // ML_SERVICE_HPP

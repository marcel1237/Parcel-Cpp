#ifndef QLM_ENGINE_HPP
#define QLM_ENGINE_HPP

#include <QString>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDir>
#include <QFile>
#include <QRegularExpression>

namespace SSAI {

    /**
     * @brief Result of a QLM processing operation.
     */
    struct QLMMatch {
        bool matched = false;
        QString intent;
        QString response;
        QString action;
        QString mode;
        QString hint;
    };

    /**
     * @brief Engine responsible for loading and matching Quality Language Model rules.
     */
    class QLMEngine {
    public:
        explicit QLMEngine(const QString& rulesDir);

        /**
         * @brief Loads all JSON rules from the directory.
         */
        void reloadRules();

        /**
         * @brief Tries to match an instruction against loaded rules.
         */
        QLMMatch findMatch(const QString& instruction);

        /**
         * @brief Utility to extract a filename from an instruction.
         */
        static QString extractFilename(const QString& instruction);

    private:
        QString m_rulesDir;
        QList<QJsonObject> m_rules;
    };

}

#endif // QLM_ENGINE_HPP

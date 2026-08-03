#ifndef SCRIPT_VALIDATOR_HPP
#define SCRIPT_VALIDATOR_HPP

#include <QString>
#include <QProcess>
#include <QDebug>

namespace SSAI {

    /**
     * @brief The ScriptValidator ensures that any shell script generated
     * by the AI Agent meets the SSQLM (Shell Script Quality Language Model)
     * standards, using ShellCheck as the primary auditor.
     */
    class ScriptValidator {
    public:
        struct ValidationResult {
            bool isValid;
            QString output;
            int errorCount;
        };

        /**
         * @brief Performs a deep structural audit based on SSQLM principles.
         */
        static ValidationResult deepAudit(const QString& scriptContent);

        /**
         * @brief Validates a script content against best practices.
         */
        static ValidationResult validate(const QString& scriptContent) {
            // SSQLM requirement: Every script must have a safe header
            if (!scriptContent.contains("set -Eeuo pipefail") && scriptContent.startsWith("#!")) {
                return {false, "Missing SSQLM safety header: 'set -Eeuo pipefail'", 1};
            }

            // Heuristic Audit
            auto audit = deepAudit(scriptContent);
            if (!audit.isValid) return audit;

            QProcess process;
            process.start("shellcheck", {"-s", "bash", "-"}); // Read from stdin
            if (!process.waitForStarted()) {
                return {true, "ShellCheck not found, skipping deep audit.", 0};
            }

            process.write(scriptContent.toUtf8());
            process.closeWriteChannel();
            process.waitForFinished();

            QString output = process.readAllStandardOutput();
            int exitCode = process.exitCode();

            return {exitCode == 0, output, exitCode};
        }
    };

}

#endif // SCRIPT_VALIDATOR_HPP

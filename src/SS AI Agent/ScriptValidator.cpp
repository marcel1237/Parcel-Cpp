#include "ScriptValidator.hpp"
#include <QRegularExpression>

namespace SSAI {

ScriptValidator::ValidationResult ScriptValidator::deepAudit(const QString& content) {
    ValidationResult result = {true, "✓ Script passed SSQLM basic heuristics.", 0};
    QStringList issues;

    // Rule 1: Forbidden Commands (High Risk)
    if (content.contains("eval ")) {
        issues << "CRITICAL: 'eval' usage detected. Potential command injection vector.";
    }

    // Rule 2: Shell Redirection safety
    if (content.contains("> /tmp/") && !content.contains("mktemp")) {
        issues << "WARNING: Direct write to /tmp detected without mktemp. Brittle and insecure.";
    }

    // Rule 3: Error Handling
    if (!content.contains("trap ") && content.length() > 500) {
        issues << "ADVICE: Complex script detected without 'trap' for cleanup or error reporting.";
    }

    // Rule 4: Variable Quoting (SSQLM Enhancement)
    QRegularExpression rmRegex("rm\\s+-[a-zA-Z]*r[a-zA-Z]*\\s+\\$[^\\{\"]");
    if (rmRegex.match(content).hasMatch()) {
        issues << "CRITICAL: Unquoted variable in recursive 'rm'. SSQLM mandates quotes to prevent catastrophic data loss.";
    }

    // Rule 5: Generic Unquoted Variables (SC2086)
    QRegularExpression genericQuoteRegex("(?:^|\\s)\\$([a-zA-Z_][a-zA-Z0-9_]*)(?![\"\\}])");
    if (genericQuoteRegex.match(content).hasMatch() && content.contains("bash")) {
        issues << "ADVICE: Unquoted variable detected. Use \"$var\" to prevent word splitting and globbing (SSQLM standard).";
    }

    // Rule 6: Sudo Usage
    if (content.contains("sudo ")) {
        issues << "WARNING: 'sudo' detected. SSQLM recommends delegating privilege escalation to the orchestrator.";
    }

    // Rule 7: Missing local in functions
    QRegularExpression funcRegex("\\w+\\s*\\(\\)\\s*\\{");
    if (funcRegex.match(content).hasMatch() && !content.contains("local ")) {
         issues << "ADVICE: Function detected but no 'local' variables found. SSQLM mandates scope isolation.";
    }

    if (!issues.isEmpty()) {
        result.isValid = false;
        result.output = issues.join("\n");
        result.errorCount = issues.size();
    }

    return result;
}

}

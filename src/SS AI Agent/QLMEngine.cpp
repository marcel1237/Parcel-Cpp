#include "QLMEngine.hpp"

namespace SSAI {

QLMEngine::QLMEngine(const QString& rulesDir) : m_rulesDir(rulesDir) {
    reloadRules();
}

void QLMEngine::reloadRules() {
    m_rules.clear();
    QDir dir(m_rulesDir);
    for (const QString& filename : dir.entryList({"*.json"}, QDir::Files)) {
        QFile file(dir.absoluteFilePath(filename));
        if (file.open(QIODevice::ReadOnly)) {
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
            if (doc.isObject()) m_rules.append(doc.object());
        }
    }
}

QLMMatch QLMEngine::findMatch(const QString& instruction) {
    QString lower = instruction.toLower();
    QLMMatch result;

    for (const auto& config : m_rules) {
        QJsonArray rules = config["rules"].toArray();
        if (rules.isEmpty()) rules = config["intents"].toArray();
        if (rules.isEmpty()) rules = config["entries"].toArray();

        for (const auto& val : rules) {
            QJsonObject item = val.toObject();
            QJsonArray triggers = item["trigger"].toArray();
            if (triggers.isEmpty()) triggers = item["patterns"].toArray();

            // Support for single pattern string
            if (triggers.isEmpty() && item.contains("pattern")) {
                triggers.append(item["pattern"]);
            }
            if (triggers.isEmpty() && item.contains("capability")) {
                triggers.append(item["capability"]);
            }
            if (triggers.isEmpty() && item.contains("target")) {
                triggers.append(item["target"]);
            }

            for (const auto& t : triggers) {
                QString pattern = t.toString();
                bool matched = false;

                // Support for Regex triggers (starting with ^ or containing .*)
                if (pattern.startsWith("^") || pattern.contains(".*") || pattern.contains("\\b")) {
                    QRegularExpression regex(pattern, QRegularExpression::CaseInsensitiveOption);
                    matched = regex.match(lower).hasMatch();
                } else {
                    matched = lower.contains(pattern.toLower());
                }

                if (matched) {
                    result.matched = true;
                    result.intent = item["intent"].toString();
                    if (result.intent.isEmpty()) result.intent = item["capability"].toString();
                    if (result.intent.isEmpty()) result.intent = item["blueprint"].toString();
                    if (result.intent.isEmpty()) result.intent = item["utility"].toString();

                    if (item.contains("response")) result.response = item["response"].toString();
                    else if (item.contains("logic")) result.response = item["logic"].toString();
                    else if (item.contains("value")) result.response = item["value"].toString();
                    else if (item.contains("rationale")) result.response = item["rationale"].toString();
                    else if (item.contains("reason")) result.response = QString("Regra: %1 (Status: %2)").arg(item["reason"].toString(), item["status"].toString());

                    result.action = item["action"].toString();
                    if (result.action.isEmpty() && item.contains("implementation")) {
                        result.action = "CODE_SAMPLE";
                        result.mode = item["implementation"].toString();
                    }
                    if (result.action.isEmpty() && item.contains("use_instead")) {
                        result.action = "SUGGESTION: " + item["use_instead"].toString();
                    }

                    result.mode = item["mode"].toString();
                    if (result.mode.isEmpty()) result.mode = item["implementation"].toString();

                    result.hint = item.contains("hint") ? item["hint"].toString() : result.response;
                    return result;
                }
            }
        }
    }
    return result;
}

QString QLMEngine::extractFilename(const QString& instruction) {
    static QRegularExpression re("\\b\\w+\\.(sh|cpp|hpp|json|txt|xml)\\b");
    QRegularExpressionMatch match = re.match(instruction);
    return match.hasMatch() ? match.captured(0) : "";
}

}

#include "ProjectHighlighter.hpp"

using namespace Parcel::Editor;

ProjectHighlighter::ProjectHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) {
    keywordFormat.setForeground(Qt::magenta);
    keywordFormat.setFontWeight(QFont::Bold);
    commentFormat.setForeground(Qt::gray);
    stringFormat.setForeground(Qt::green);
    xmlTagFormat.setForeground(Qt::blue);
}

void ProjectHighlighter::setLanguage(const QString &fileName) {
    QString lowerName = fileName.toLower();
    QString ext = "";
    int lastDot = fileName.lastIndexOf('.');
    if (lastDot != -1) {
        ext = fileName.mid(lastDot + 1).toLower();
    }

    rules.clear();
    if (ext == "java" || ext == "kt") setupJavaRules();
    else if (ext == "xml" || ext == "fxml" || ext == "user") setupXmlRules();
    else if (ext == "md") setupMarkdownRules();
    else if (ext == "sh" || ext == "bash" || ext == "cflags" || ext == "cxxflags") setupShellRules();
    else if (ext == "js" || ext == "ts" || ext == "jsx" || ext == "tsx") setupJsRules();
    else if (ext == "css" || ext == "scss" || ext == "sass") setupCssRules();
    else if (ext == "cpp" || ext == "hpp" || ext == "c" || ext == "h" || ext == "cc" || ext == "rs" || ext == "go" || ext == "swift") setupCppRules();
    else if (ext == "json") setupJsonRules();
    else if (ext == "py") setupPythonRules();
    else if (ext == "php") setupPhpRules();
    else if (ext == "rb") setupRubyRules();
    else if (ext == "sql") setupSqlRules();
    else if (ext == "yaml" || ext == "yml") setupYamlRules();
    else if (lowerName == "makefile" || ext == "mk") setupMakefileRules();
    else if (lowerName == "cmakelists.txt" || ext == "cmake") setupCmakeRules();
    else if (ext == "desktop" || ext == "kdev4") setupDesktopRules();
    else if (ext == "pro" || ext == "pri") setupQtProjectRules();
    else if (ext == "files" || ext == "includes" || ext == "config" || ext == "creator") setupGenericListRules();
    else setupMarkdownRules();

    rehighlight();
}

void ProjectHighlighter::setLanguageByExtension(const QString &extension) {
    setLanguage("file." + extension);
}

void ProjectHighlighter::setupJavaRules() {
    QStringList keywords = {"abstract", "class", "final", "public", "static", "void", "int", "if", "else", "new", "package", "import", "val", "var", "fun", "object", "interface"};
    for (const QString &keyword : keywords)
        rules.push_back({QRegularExpression(QString("\\b%1\\b").arg(keyword)), keywordFormat});
    rules.push_back({QRegularExpression("\".*?\""), stringFormat});
    rules.push_back({QRegularExpression("//[^\n]*"), commentFormat});
}

void ProjectHighlighter::setupXmlRules() {
    rules.push_back({QRegularExpression("<[^>]+>"), xmlTagFormat});
    rules.push_back({QRegularExpression("\".*?\""), stringFormat});
    rules.push_back({QRegularExpression("<!--.*?-->"), commentFormat});
}

void ProjectHighlighter::setupMarkdownRules() {
    rules.push_back({QRegularExpression("(^#+.*$)"), keywordFormat});
    rules.push_back({QRegularExpression("(```[\\s\\S]*?```)"), stringFormat});
}

void ProjectHighlighter::setupShellRules() {
    QStringList keywords = {"if", "then", "else", "elif", "fi", "for", "in", "do", "done", "while", "until", "case", "esac", "function"};
    for (const QString &kw : keywords)
        rules.push_back({QRegularExpression(QString("\\b%1\\b").arg(kw)), keywordFormat});
    rules.push_back({QRegularExpression("#.*"), commentFormat});
    rules.push_back({QRegularExpression("\".*?\""), stringFormat});
    rules.push_back({QRegularExpression("\\$([a-zA-Z_][a-zA-Z0-9_]*)"), xmlTagFormat});
}

void ProjectHighlighter::setupJsRules() {
    QStringList keywords = {"async", "await", "break", "case", "catch", "class", "const", "continue", "default", "function", "if", "import", "return", "var", "let", "export", "from", "of"};
    for (const QString &kw : keywords)
        rules.push_back({QRegularExpression(QString("\\b%1\\b").arg(kw)), keywordFormat});
    rules.push_back({QRegularExpression("//.*"), commentFormat});
    rules.push_back({QRegularExpression("\".*?\""), stringFormat});
}

void ProjectHighlighter::setupCssRules() {
    rules.push_back({QRegularExpression("^[^{]+(?=\\{)"), keywordFormat});
    rules.push_back({QRegularExpression("[a-zA-Z\\- ]+(?=:)"), xmlTagFormat});
    rules.push_back({QRegularExpression(":[^;\\}]+"), stringFormat});
    rules.push_back({QRegularExpression("/\\*[\\s\\S]*?\\*/"), commentFormat});
}

void ProjectHighlighter::setupCppRules() {
    QStringList keywords = {"auto", "bool", "break", "case", "catch", "char", "class", "const", "continue", "default", "delete", "do", "double", "else", "enum", "extern", "false", "float", "for", "goto", "if", "inline", "int", "long", "namespace", "new", "nullptr", "operator", "private", "protected", "public", "return", "short", "sizeof", "static", "struct", "switch", "template", "this", "throw", "true", "try", "typedef", "typeid", "typename", "union", "unsigned", "using", "virtual", "void", "while", "fn", "let", "mut", "pub", "match", "use", "mod"};
    for (const QString &keyword : keywords)
        rules.push_back({QRegularExpression(QString("\\b%1\\b").arg(keyword)), keywordFormat});
    rules.push_back({QRegularExpression("^#[^\n]*"), xmlTagFormat});
    rules.push_back({QRegularExpression("\".*?\""), stringFormat});
    rules.push_back({QRegularExpression("//[^\n]*"), commentFormat});
    rules.push_back({QRegularExpression("/\\*.*?\\*/"), commentFormat});
}

void ProjectHighlighter::setupPythonRules() {
    QStringList keywords = {"def", "class", "if", "else", "elif", "for", "while", "return", "import", "from", "as", "try", "except", "finally", "with", "lambda", "yield", "pass", "break", "continue"};
    for (const QString &kw : keywords)
        rules.push_back({QRegularExpression(QString("\\b%1\\b").arg(kw)), keywordFormat});
    rules.push_back({QRegularExpression("\".*?\"|'.*?'"), stringFormat});
    rules.push_back({QRegularExpression("#.*"), commentFormat});
}

void ProjectHighlighter::setupPhpRules() {
    QStringList keywords = {"function", "class", "if", "else", "elseif", "for", "foreach", "while", "return", "public", "private", "protected", "static", "new", "echo", "try", "catch"};
    for (const QString &kw : keywords)
        rules.push_back({QRegularExpression(QString("\\b%1\\b").arg(kw)), keywordFormat});
    rules.push_back({QRegularExpression("\".*?\"|'.*?'"), stringFormat});
    rules.push_back({QRegularExpression("//.*|#.*|/\\*.*?\\*/"), commentFormat});
    rules.push_back({QRegularExpression("\\$([a-zA-Z_][a-zA-Z0-9_]*)"), xmlTagFormat});
}

void ProjectHighlighter::setupRubyRules() {
    QStringList keywords = {"def", "class", "if", "else", "elsif", "end", "for", "while", "return", "module", "yield", "require", "begin", "rescue"};
    for (const QString &kw : keywords)
        rules.push_back({QRegularExpression(QString("\\b%1\\b").arg(kw)), keywordFormat});
    rules.push_back({QRegularExpression("\".*?\"|'.*?'"), stringFormat});
    rules.push_back({QRegularExpression("#.*"), commentFormat});
}

void ProjectHighlighter::setupSqlRules() {
    QStringList keywords = {"SELECT", "FROM", "WHERE", "INSERT", "INTO", "UPDATE", "SET", "DELETE", "CREATE", "TABLE", "DROP", "ALTER", "JOIN", "ON", "GROUP", "BY", "ORDER", "LIMIT"};
    for (const QString &kw : keywords)
        rules.push_back({QRegularExpression(QString("\\b%1\\b").arg(kw), QRegularExpression::CaseInsensitiveOption), keywordFormat});
    rules.push_back({QRegularExpression("\".*?\"|'.*?'"), stringFormat});
    rules.push_back({QRegularExpression("--.*|/\\*.*?\\*/"), commentFormat});
}

void ProjectHighlighter::setupYamlRules() {
    rules.push_back({QRegularExpression("^[\\s-]*[a-zA-Z0-9_-]+(?=:)"), xmlTagFormat});
    rules.push_back({QRegularExpression(":\\s*.*$"), stringFormat});
    rules.push_back({QRegularExpression("#.*"), commentFormat});
}

void ProjectHighlighter::setupJsonRules() {
    rules.push_back({QRegularExpression("\".*?\"\\s*:"), xmlTagFormat});
    rules.push_back({QRegularExpression(":\\s*\".*?\""), stringFormat});
    rules.push_back({QRegularExpression("\\b(true|false|null)\\b"), keywordFormat});
    rules.push_back({QRegularExpression("\\b-?\\d+(\\.\\d+)?([eE][+-]?\\d+)?\\b"), stringFormat});
}

void ProjectHighlighter::setupMakefileRules() {
    rules.push_back({QRegularExpression("^[^#\\t]*:"), keywordFormat});
    rules.push_back({QRegularExpression("\\$\\([a-zA-Z0-9_]+\\)"), xmlTagFormat});
    rules.push_back({QRegularExpression("#.*"), commentFormat});
}

void ProjectHighlighter::setupCmakeRules() {
    QStringList keywords = {"set", "project", "cmake_minimum_required", "add_executable", "add_library", "target_link_libraries", "find_package", "if", "endif", "foreach", "endforeach"};
    for (const QString &kw : keywords)
        rules.push_back({QRegularExpression(QString("\\b%1\\b").arg(kw), QRegularExpression::CaseInsensitiveOption), keywordFormat});
    rules.push_back({QRegularExpression("\\$\\{[a-zA-Z0-9_]+\\}"), xmlTagFormat});
    rules.push_back({QRegularExpression("#.*"), commentFormat});
    rules.push_back({QRegularExpression("\".*?\""), stringFormat});
}

void ProjectHighlighter::setupDesktopRules() {
    rules.push_back({QRegularExpression("^\\[.*\\]"), keywordFormat});
    rules.push_back({QRegularExpression("^[a-zA-Z0-9\\-]+(?==)"), xmlTagFormat});
    rules.push_back({QRegularExpression("=.*"), stringFormat});
    rules.push_back({QRegularExpression("#.*"), commentFormat});
}

void ProjectHighlighter::setupQtProjectRules() {
    rules.push_back({QRegularExpression("^[A-Z_]+\\s*(\\+|=)"), keywordFormat});
    rules.push_back({QRegularExpression("\\\\\\s*$"), xmlTagFormat});
    rules.push_back({QRegularExpression("#.*"), commentFormat});
    rules.push_back({QRegularExpression("\\b[a-zA-Z0-9_\\-\\./]+\\.[a-zA-Z0-9]+\\b"), stringFormat});
}

void ProjectHighlighter::setupGenericListRules() {
    rules.push_back({QRegularExpression("^#.*"), commentFormat});
    rules.push_back({QRegularExpression("\\b[a-zA-Z0-9_\\-\\./]+\\b"), stringFormat});
}

void ProjectHighlighter::highlightBlock(const QString &text) {
    for (const HighlightingRule &rule : rules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}

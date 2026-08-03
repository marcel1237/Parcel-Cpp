#ifndef PROJECTHIGHLIGHTER_HPP
#define PROJECTHIGHLIGHTER_HPP

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <vector>

namespace Parcel::Editor {

    class ProjectHighlighter : public QSyntaxHighlighter {
        Q_OBJECT

    public:
        explicit ProjectHighlighter(QTextDocument *parent = nullptr);
        void setLanguage(const QString &fileName);
        void setLanguageByExtension(const QString &extension);

    protected:
        void highlightBlock(const QString &text) override;

    private:
        struct HighlightingRule {
            QRegularExpression pattern;
            QTextCharFormat format;
        };

        std::vector<HighlightingRule> rules;

        QTextCharFormat keywordFormat;
        QTextCharFormat commentFormat;
        QTextCharFormat stringFormat;
        QTextCharFormat xmlTagFormat;

        void setupJavaRules();
        void setupXmlRules();
        void setupMarkdownRules();
        void setupShellRules();
        void setupJsRules();
        void setupCssRules();
        void setupCppRules();
        void setupPythonRules();
        void setupPhpRules();
        void setupRubyRules();
        void setupSqlRules();
        void setupYamlRules();
        void setupJsonRules();
        void setupMakefileRules();
        void setupCmakeRules();
        void setupDesktopRules();
        void setupQtProjectRules();
        void setupGenericListRules();
    };
}
#endif

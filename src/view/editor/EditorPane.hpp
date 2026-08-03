#ifndef EDITOR_PANE_HPP
#define EDITOR_PANE_HPP

#include <string>
#include <filesystem>
#include <QMainWindow>
#include <QTextEdit>
#include <QMenu>
#include <QDesktopServices>
#include <QUrl>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QShortcut>
#include <QCheckBox>
#include <QCompleter>
#include <QStringListModel>
#include <QTimer>
#include <QProcess>
#include <QDir>
#include <QFile>
#include "ProjectHighlighter.hpp"

namespace Parcel::View {

    struct InspectionIssue {
        int line;
        QString type;
        QString message;
    };

    class CodeEditor : public QTextEdit {
        Q_OBJECT
    public:
        explicit CodeEditor(QWidget* parent = nullptr);
        void setCompleter(QCompleter* completer);
        QCompleter* completer() const;
        void setInspectionIssues(const QList<InspectionIssue>& issues);

    protected:
        void keyPressEvent(QKeyEvent* e) override;
        void focusInEvent(QFocusEvent* e) override;
        bool event(QEvent* e) override;

    private slots:
        void insertCompletion(const QString& completion);

    private:
        QString textUnderCursor() const;

        QCompleter* m_completer = nullptr;
        QList<InspectionIssue> m_issues;
    };

    class EditorPane : public QMainWindow {
        Q_OBJECT
    public:
        static constexpr long MAX_FILE_SIZE = 1024 * 1024; // 1MB

        EditorPane(const std::string& filePath, const std::string& content);

        std::string getText() const;
        std::filesystem::path getFile() const { return currentFile; }

    private slots:
        void showContextMenu(const QPoint& pos);
        void handleAutonomousRefactor();
        void handleQuickFix();
        void toggleFindReplace();
        void findNext();
        void findPrevious();
        void replace();
        void replaceAll();
        void updateSearchHighlights();
        void runAutoInspection();
        void handleInspectionResults();

    signals:
        void requestBrowserUrl(const QString& url);
        void requestGoogleUrl(const QString& url);

    private:
        std::filesystem::path currentFile;
        CodeEditor* codeArea;
        Parcel::Editor::ProjectHighlighter* highlighter;
        QCompleter* m_completer = nullptr;

        // Find & Replace UI
        QWidget* findReplaceBar;
        QLineEdit* findField;
        QLineEdit* replaceField;
        QCheckBox* caseSensitiveCheck;
        QTimer* inspectionTimer;
        QProcess* linterProcess;

        void setupFindReplaceBar();
        void setupCompleter();
        void runHighlightingAsync();
    };
}
#endif

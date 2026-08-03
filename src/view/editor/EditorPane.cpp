#include "EditorPane.hpp"
#include <QTextEdit>
#include <filesystem>
#include <QMenu>
#include <QDesktopServices>
#include <QUrl>
#include <QUrlQuery>
#include <QShortcut>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QKeyEvent>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QToolTip>
#include <QHelpEvent>

using namespace Parcel::View;

// --- CodeEditor Implementation ---

CodeEditor::CodeEditor(QWidget* parent) : QTextEdit(parent) {
    setStyleSheet("background-color: #1e1e1e; color: #d4d4d4; font-family: 'Monospace'; font-size: 13px;");
}

void CodeEditor::setCompleter(QCompleter* completer) {
    if (m_completer)
        m_completer->disconnect(this);

    m_completer = completer;

    if (!m_completer)
        return;

    m_completer->setWidget(this);
    m_completer->setCompletionMode(QCompleter::PopupCompletion);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    connect(m_completer, QOverload<const QString&>::of(&QCompleter::activated),
            this, &CodeEditor::insertCompletion);
}

QCompleter* CodeEditor::completer() const {
    return m_completer;
}

void CodeEditor::insertCompletion(const QString& completion) {
    if (m_completer->widget() != this)
        return;
    QTextCursor tc = textCursor();
    tc.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, m_completer->completionPrefix().length());
    tc.removeSelectedText();
    tc.insertText(completion);
    setTextCursor(tc);
}

QString CodeEditor::textUnderCursor() const {
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void CodeEditor::focusInEvent(QFocusEvent* e) {
    if (m_completer)
        m_completer->setWidget(this);
    QTextEdit::focusInEvent(e);
}

void CodeEditor::keyPressEvent(QKeyEvent* e) {
    if (m_completer && m_completer->popup()->isVisible()) {
        switch (e->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return;
        default:
            break;
        }
    }

    const bool isShortcut = (e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_Space;
    if (!m_completer || !isShortcut)
        QTextEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (!m_completer || (ctrlOrShift && e->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-=");
    bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor();

    if (!isShortcut && (hasModifier || e->text().isEmpty() || completionPrefix.length() < 2
                      || eow.contains(e->text().right(1)))) {
        m_completer->popup()->hide();
        return;
    }

    if (completionPrefix != m_completer->completionPrefix()) {
        m_completer->setCompletionPrefix(completionPrefix);
        m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));
    }
    QRect cr = cursorRect();
    cr.setWidth(m_completer->popup()->sizeHintForColumn(0)
                + m_completer->popup()->verticalScrollBar()->sizeHint().width());
    m_completer->complete(cr);
}

void CodeEditor::setInspectionIssues(const QList<InspectionIssue>& issues) {
    m_issues = issues;
}

bool CodeEditor::event(QEvent* e) {
    if (e->type() == QEvent::ToolTip) {
        QHelpEvent* helpEvent = static_cast<QHelpEvent*>(e);
        QTextCursor cursor = cursorForPosition(helpEvent->pos());
        int line = cursor.blockNumber();

        QStringList messages;
        for (const auto& issue : m_issues) {
            if (issue.line == line) {
                QString color = (issue.type == "error") ? "#e74c3c" : "#f1c40f";
                messages << QString("<b style='color:%1'>%2:</b> %3")
                            .arg(color, issue.type.toUpper(), issue.message);
            }
        }

        if (!messages.isEmpty()) {
            messages << "<br><i style='color:#888'>💡 Pressione <b>Ctrl+Alt+F</b> para Quick Fix via AI</i>";
            QToolTip::showText(helpEvent->globalPos(), messages.join("<br>"));
        } else {
            QToolTip::hideText();
        }
        return true;
    }
    return QTextEdit::event(e);
}

// --- EditorPane Implementation ---

EditorPane::EditorPane(const std::string& filePath, const std::string& content)
    : currentFile(filePath) {

    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    setupFindReplaceBar();
    mainLayout->addWidget(findReplaceBar);

    codeArea = new CodeEditor(this);
    codeArea->setPlainText(QString::fromStdString(content));
    codeArea->setContextMenuPolicy(Qt::CustomContextMenu);
    mainLayout->addWidget(codeArea);

    setupCompleter();

    highlighter = new Parcel::Editor::ProjectHighlighter(codeArea->document());
    QString fileName = QString::fromStdString(currentFile.filename().string());
    highlighter->setLanguage(fileName);

    setCentralWidget(centralWidget);

    connect(codeArea, &QTextEdit::customContextMenuRequested, this, &EditorPane::showContextMenu);
    new QShortcut(QKeySequence("Ctrl+I"), this, SLOT(handleAutonomousRefactor()));
    new QShortcut(QKeySequence("Ctrl+Alt+F"), this, SLOT(handleQuickFix()));
    new QShortcut(QKeySequence::Find, this, SLOT(toggleFindReplace()));

    inspectionTimer = new QTimer(this);
    inspectionTimer->setSingleShot(true);
    inspectionTimer->setInterval(2000);

    linterProcess = new QProcess(this);
    connect(codeArea, &QTextEdit::textChanged, [this]() { inspectionTimer->start(); });
    connect(inspectionTimer, &QTimer::timeout, this, &EditorPane::runAutoInspection);
    connect(linterProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &EditorPane::handleInspectionResults);
}

void EditorPane::handleAutonomousRefactor() {
    QTextCursor cursor = codeArea->textCursor();
    if (cursor.hasSelection()) {
        emit requestGoogleUrl("REF_AUTO:" + cursor.selectedText());
    }
}

void EditorPane::handleQuickFix() {
    QTextCursor cursor = codeArea->textCursor();
    Q_UNUSED(cursor);

    QString issueDetails;
    // Pega todos os problemas desta linha do CodeEditor
    // Mas precisamos de acesso a m_issues do CodeEditor.
    // Como EditorPane não tem acesso direto a m_issues (é private no CodeEditor),
    // deveríamos ter um getter ou o EditorPane deveria gerenciar os issues.

    // Simplificando: vamos enviar a linha e pedir correção.
    cursor.select(QTextCursor::LineUnderCursor);
    QString lineText = cursor.selectedText();

    emit requestGoogleUrl("QUICK_FIX:" + lineText);
}

void EditorPane::setupFindReplaceBar() {
    findReplaceBar = new QWidget(this);
    findReplaceBar->setStyleSheet("background-color: #2b2b2b; color: #d4d4d4; border-bottom: 1px solid #3c3c3c;");
    findReplaceBar->setVisible(false);

    QVBoxLayout* barLayout = new QVBoxLayout(findReplaceBar);
    barLayout->setContentsMargins(10, 5, 10, 5);
    barLayout->setSpacing(5);

    QHBoxLayout* findRow = new QHBoxLayout();
    findField = new QLineEdit(this);
    findField->setPlaceholderText("Localizar...");
    findField->setStyleSheet("background-color: #3c3c3c; border: 1px solid #555; padding: 2px;");

    QPushButton* btnNext = new QPushButton("Próximo", this);
    QPushButton* btnPrev = new QPushButton("Anterior", this);
    caseSensitiveCheck = new QCheckBox("Case Sensitive", this);

    QPushButton* btnClose = new QPushButton("✕", this);
    btnClose->setFixedSize(20, 20);

    findRow->addWidget(new QLabel("🔍", this));
    findRow->addWidget(findField);
    findRow->addWidget(btnPrev);
    findRow->addWidget(btnNext);
    findRow->addWidget(caseSensitiveCheck);
    findRow->addStretch();
    findRow->addWidget(btnClose);

    QHBoxLayout* replaceRow = new QHBoxLayout();
    replaceField = new QLineEdit(this);
    replaceField->setPlaceholderText("Substituir por...");
    replaceField->setStyleSheet("background-color: #3c3c3c; border: 1px solid #555; padding: 2px;");

    QPushButton* btnReplace = new QPushButton("Substituir", this);
    QPushButton* btnReplaceAll = new QPushButton("Substituir Tudo", this);

    replaceRow->addWidget(new QLabel("♻️", this));
    replaceRow->addWidget(replaceField);
    replaceRow->addWidget(btnReplace);
    replaceRow->addWidget(btnReplaceAll);
    replaceRow->addStretch();

    barLayout->addLayout(findRow);
    barLayout->addLayout(replaceRow);

    connect(btnNext, &QPushButton::clicked, this, &EditorPane::findNext);
    connect(btnPrev, &QPushButton::clicked, this, &EditorPane::findPrevious);
    connect(btnReplace, &QPushButton::clicked, this, &EditorPane::replace);
    connect(btnReplaceAll, &QPushButton::clicked, this, &EditorPane::replaceAll);
    connect(btnClose, &QPushButton::clicked, this, &EditorPane::toggleFindReplace);
    connect(findField, &QLineEdit::returnPressed, this, &EditorPane::findNext);
    connect(findField, &QLineEdit::textChanged, this, &EditorPane::updateSearchHighlights);
    connect(caseSensitiveCheck, &QCheckBox::toggled, this, &EditorPane::updateSearchHighlights);
}

void EditorPane::toggleFindReplace() {
    findReplaceBar->setVisible(!findReplaceBar->isVisible());
    if (findReplaceBar->isVisible()) {
        findField->setFocus();
        findField->selectAll();
        updateSearchHighlights();
    } else {
        codeArea->setExtraSelections({});
    }
}

void EditorPane::findNext() {
    QString term = findField->text();
    if (term.isEmpty()) return;
    QTextDocument::FindFlags flags;
    if (caseSensitiveCheck->isChecked()) flags |= QTextDocument::FindCaseSensitively;
    if (!codeArea->find(term, flags)) {
        QTextCursor cursor = codeArea->textCursor();
        cursor.movePosition(QTextCursor::Start);
        codeArea->setTextCursor(cursor);
        codeArea->find(term, flags);
    }
}

void EditorPane::findPrevious() {
    QString term = findField->text();
    if (term.isEmpty()) return;
    QTextDocument::FindFlags flags = QTextDocument::FindBackward;
    if (caseSensitiveCheck->isChecked()) flags |= QTextDocument::FindCaseSensitively;
    if (!codeArea->find(term, flags)) {
        QTextCursor cursor = codeArea->textCursor();
        cursor.movePosition(QTextCursor::End);
        codeArea->setTextCursor(cursor);
        codeArea->find(term, flags);
    }
}

void EditorPane::replace() {
    QTextCursor cursor = codeArea->textCursor();
    if (cursor.hasSelection() && cursor.selectedText() == findField->text()) {
        cursor.insertText(replaceField->text());
    }
    findNext();
}

void EditorPane::replaceAll() {
    QString term = findField->text();
    if (term.isEmpty()) return;
    QString content = codeArea->toPlainText();
    Qt::CaseSensitivity cs = caseSensitiveCheck->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;
    content.replace(term, replaceField->text(), cs);
    codeArea->setPlainText(content);
    updateSearchHighlights();
}

void EditorPane::updateSearchHighlights() {
    QList<QTextEdit::ExtraSelection> extraSelections;
    QString term = findField->text();
    if (!term.isEmpty() && findReplaceBar->isVisible()) {
        QTextDocument::FindFlags flags;
        if (caseSensitiveCheck->isChecked()) flags |= QTextDocument::FindCaseSensitively;
        QTextCursor cursor(codeArea->document());
        while (!(cursor = codeArea->document()->find(term, cursor, flags)).isNull()) {
            QTextEdit::ExtraSelection selection;
            selection.format.setBackground(QColor("#4b4e00"));
            selection.format.setForeground(Qt::white);
            selection.cursor = cursor;
            extraSelections.append(selection);
        }
    }
    codeArea->setExtraSelections(extraSelections);
}

std::string EditorPane::getText() const { return codeArea->toPlainText().toStdString(); }

void EditorPane::showContextMenu(const QPoint& pos) {
    QMenu* menu = codeArea->createStandardContextMenu();
    QTextCursor cursor = codeArea->textCursor();
    if (cursor.hasSelection()) {
        menu->addSeparator();
        QString selectedText = cursor.selectedText();
        QAction* assistAction = menu->addAction(QIcon::fromTheme("help-browser"), "Code Assist");
        connect(assistAction, &QAction::triggered, [this, selectedText]() {
            emit requestBrowserUrl("https://chat.openai.com/?q=" + QUrl::toPercentEncoding(selectedText));
        });

        QAction* quickFixAction = menu->addAction(QIcon::fromTheme("tools-wizard"), "Quick Fix (AI)");
        connect(quickFixAction, &QAction::triggered, this, &EditorPane::handleQuickFix);

        QString ext = QFileInfo(QString::fromStdString(currentFile.string())).suffix().toLower();
        if (ext == "sh" || ext == "bash") {
            QAction* explainAction = menu->addAction(QIcon::fromTheme("help-about"), "ExplainShell (Web)");
            connect(explainAction, &QAction::triggered, [this, selectedText]() {
                QString url = "https://explainshell.com/explain?cmd=" + QUrl::toPercentEncoding(selectedText);
                emit requestBrowserUrl(url);
            });

            QAction* explainAiAction = menu->addAction(QIcon::fromTheme("help-about"), "Explicar Comando (IA)");
            connect(explainAiAction, &QAction::triggered, [this, selectedText]() {
                emit requestGoogleUrl("REF_AUTO:Explique o que este comando shell faz em detalhes: " + selectedText);
            });
        }
    }
    menu->exec(codeArea->mapToGlobal(pos));
    delete menu;
}

void EditorPane::runAutoInspection() {
    QString fileName = QString::fromStdString(currentFile.filename().string()).toLower();
    QString ext = QFileInfo(fileName).suffix();
    QString tempPath = QDir::tempPath() + "/parcel_inspect_" + fileName;

    QFile tempFile(tempPath);
    if (tempFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        tempFile.write(codeArea->toPlainText().toUtf8());
        tempFile.close();
    }

    QStringList args;
    if (ext == "cpp" || ext == "hpp" || ext == "h" || ext == "cc") {
        args << "-fsyntax-only" << "-Wall" << "-Wextra" << "-Wunused" << tempPath;
        linterProcess->start("g++", args);
    } else if (ext == "py") {
        args << "-m" << "py_compile" << tempPath;
        linterProcess->start("python3", args);
    } else if (ext == "js" || ext == "ts") {
        args << "--check" << tempPath;
        linterProcess->start("node", args);
    } else if (ext == "php") {
        args << "-l" << tempPath;
        linterProcess->start("php", args);
    } else if (ext == "sh" || ext == "bash") {
        args << "-n" << tempPath;
        linterProcess->start("bash", args);
    }
}

void EditorPane::handleInspectionResults() {
    QString output = linterProcess->readAllStandardError();
    if (output.isEmpty()) output = linterProcess->readAllStandardOutput();

    QList<QTextEdit::ExtraSelection> selections = codeArea->extraSelections();
    selections.erase(std::remove_if(selections.begin(), selections.end(), [](const QTextEdit::ExtraSelection& s) {
        return s.format.underlineStyle() == QTextCharFormat::SpellCheckUnderline ||
               s.format.underlineStyle() == QTextCharFormat::DashUnderline;
    }), selections.end());

    QList<InspectionIssue> issues;
    QRegularExpression re("(error|warning|Parse error|Fatal error)[:\\s]+(.*?)on line (\\d+)|:(\\d+):(\\d+):\\s+(error|warning):\\s+(.+)$", QRegularExpression::MultilineOption);
    auto it = re.globalMatch(output);

    while (it.hasNext()) {
        auto match = it.next();
        int line = (match.captured(3).isEmpty() ? match.captured(4) : match.captured(3)).toInt() - 1;
        if (line < 0) continue;

        QString type = match.captured(1).isEmpty() ? match.captured(6) : match.captured(1);
        QString msg = match.captured(2).isEmpty() ? match.captured(7) : match.captured(2);

        issues.append({line, type.toLower(), msg});

        QTextEdit::ExtraSelection selection;
        selection.cursor = QTextCursor(codeArea->document()->findBlockByLineNumber(line));

        QTextCharFormat format;
        if (type.contains("error", Qt::CaseInsensitive)) {
            format.setUnderlineColor(Qt::red);
            format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
        } else {
            format.setUnderlineColor(QColor("#f1c40f"));
            format.setUnderlineStyle(QTextCharFormat::DashUnderline);
        }
        selection.format = format;
        selection.cursor.select(QTextCursor::LineUnderCursor);
        selections.append(selection);
    }
    codeArea->setInspectionIssues(issues);
    codeArea->setExtraSelections(selections);
}

void EditorPane::setupCompleter() {
    QString ext = QFileInfo(QString::fromStdString(currentFile.string())).suffix().toLower();
    QStringList keywords;

    if (ext == "cpp" || ext == "hpp" || ext == "h" || ext == "cc") {
        keywords << "int" << "float" << "double" << "char" << "bool" << "void" << "class" << "struct" << "public" << "private" << "protected" << "template" << "typename" << "if" << "else" << "for" << "while" << "switch" << "case" << "break" << "continue" << "return" << "new" << "delete" << "this" << "std::string" << "std::vector" << "std::cout" << "std::endl";
    } else if (ext == "py") {
        keywords << "def" << "class" << "if" << "else" << "elif" << "for" << "while" << "return" << "import" << "from" << "as" << "try" << "except" << "finally" << "print" << "len" << "range" << "True" << "False" << "None";
    } else if (ext == "java" || ext == "kt") {
        keywords << "public" << "private" << "protected" << "static" << "final" << "class" << "interface" << "void" << "int" << "String" << "if" << "else" << "for" << "while" << "return" << "new" << "package" << "import" << "override" << "fun" << "val" << "var";
    } else if (ext == "js" || ext == "ts") {
        keywords << "function" << "const" << "let" << "var" << "if" << "else" << "for" << "while" << "return" << "import" << "export" << "class" << "extends" << "async" << "await" << "console.log" << "window" << "document";
    } else if (ext == "php") {
        keywords << "echo" << "function" << "class" << "public" << "private" << "protected" << "if" << "else" << "elseif" << "foreach" << "while" << "return" << "new" << "array" << "isset" << "empty";
    } else if (ext == "sh" || ext == "bash") {
        keywords << "if" << "then" << "else" << "elif" << "fi" << "for" << "in" << "do" << "done" << "while" << "until" << "case" << "esac" << "function" << "local" << "export" << "alias" << "source" << "trap" << "echo" << "printf" << "read" << "return" << "exit" << "break" << "continue";
    } else if (ext == "sql") {
        keywords << "SELECT" << "FROM" << "WHERE" << "INSERT" << "INTO" << "UPDATE" << "SET" << "DELETE" << "CREATE" << "TABLE" << "JOIN" << "ON" << "GROUP BY" << "ORDER BY" << "LIMIT";
    }

    if (!keywords.isEmpty()) {
        m_completer = new QCompleter(keywords, this);
        m_completer->setCaseSensitivity(Qt::CaseInsensitive);
        m_completer->setFilterMode(Qt::MatchContains);
        codeArea->setCompleter(m_completer);
    }
}
void EditorPane::runHighlightingAsync() {}

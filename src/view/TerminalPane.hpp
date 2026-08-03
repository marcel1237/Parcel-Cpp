#ifndef TERMINAL_PANE_HPP
#define TERMINAL_PANE_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QScrollBar>
#include <QDateTime>

namespace Parcel::View {

    class TerminalPane : public QWidget {
        Q_OBJECT
    public:
        explicit TerminalPane(QWidget* parent = nullptr) : QWidget(parent) {
            auto* layout = new QVBoxLayout(this);
            layout->setContentsMargins(0, 0, 0, 0);
            layout->setSpacing(0);

            outputArea = new QTextEdit(this);
            outputArea->setReadOnly(true);
            outputArea->setStyleSheet(
                "QTextEdit { background-color: #0c0c0c; color: #cccccc; border: none; font-family: 'Monospace'; font-size: 13px; }"
            );
            layout->addWidget(outputArea);
        }

    public slots:
        void appendOutput(const QString& text) {
            outputArea->moveCursor(QTextCursor::End);
            outputArea->insertPlainText(text);
            outputArea->verticalScrollBar()->setValue(outputArea->verticalScrollBar()->maximum());
        }

        void appendError(const QString& text) {
            outputArea->moveCursor(QTextCursor::End);
            QTextCharFormat format;
            format.setForeground(QColor("#e74c3c"));
            outputArea->setCurrentCharFormat(format);
            outputArea->insertPlainText(text);
            outputArea->setCurrentCharFormat(QTextCharFormat()); // Reset
            outputArea->verticalScrollBar()->setValue(outputArea->verticalScrollBar()->maximum());
        }

        void logSystem(const QString& msg) {
            QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
            appendOutput(QString("[%1] [SISTEMA] %2\n").arg(timestamp, msg));
        }

        void clear() {
            outputArea->clear();
        }

    private:
        QTextEdit* outputArea;
    };
}

#endif

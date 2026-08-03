#ifndef DIFF_DIALOG_HPP
#define DIFF_DIALOG_HPP

#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QScrollBar>
#include <QTextBlock>
#include "ProjectHighlighter.hpp"

namespace Parcel::View {

    class DiffDialog : public QDialog {
        Q_OBJECT
    public:
        DiffDialog(const QString& fileName, const QString& currentContent, const QString& oldContent, QWidget* parent = nullptr)
            : QDialog(parent) {
            setWindowTitle("Comparar Versões: " + fileName);
            resize(1200, 800);

            auto* mainLayout = new QVBoxLayout(this);
            mainLayout->setContentsMargins(10, 10, 10, 10);

            auto* hLayout = new QHBoxLayout();

            // Lado Esquerdo: Atual
            auto* currentLayout = new QVBoxLayout();
            auto* lblCurrent = new QLabel("<b>Versão Atual (Disco)</b>");
            lblCurrent->setStyleSheet("color: #27ae60;");
            currentLayout->addWidget(lblCurrent);

            currentEditor = new QTextEdit(this);
            currentEditor->setPlainText(currentContent);
            currentEditor->setReadOnly(true);
            currentEditor->setLineWrapMode(QTextEdit::NoWrap);
            currentEditor->setStyleSheet("QTextEdit { background-color: #1e1f22; color: #bcbec4; font-family: 'Monospace'; font-size: 12px; border: 1px solid #323232; }");
            currentLayout->addWidget(currentEditor);
            hLayout->addLayout(currentLayout);

            // Lado Direito: Antiga (Backup)
            auto* oldLayout = new QVBoxLayout();
            auto* lblOld = new QLabel("<b>Versão do Backup</b>");
            lblOld->setStyleSheet("color: #e74c3c;");
            oldLayout->addWidget(lblOld);

            oldEditor = new QTextEdit(this);
            oldEditor->setPlainText(oldContent);
            oldEditor->setReadOnly(true);
            oldEditor->setLineWrapMode(QTextEdit::NoWrap);
            oldEditor->setStyleSheet("QTextEdit { background-color: #1a1a1a; color: #bcbec4; font-family: 'Monospace'; font-size: 12px; border: 1px solid #323232; }");
            oldLayout->addWidget(oldEditor);
            hLayout->addLayout(oldLayout);

            mainLayout->addLayout(hLayout);

            // Aplica Realce de Sintaxe (Keywords, etc)
            currentHighlighter = new Parcel::Editor::ProjectHighlighter(currentEditor->document());
            oldHighlighter = new Parcel::Editor::ProjectHighlighter(oldEditor->document());
            currentHighlighter->setLanguage(fileName);
            oldHighlighter->setLanguage(fileName);

            // Aplica Realce de Diferenças (Background)
            applyDiffHighlighting(currentContent, oldContent);

            // Sincronizar Scroll Vertical e Horizontal
            connect(currentEditor->verticalScrollBar(), &QScrollBar::valueChanged,
                    oldEditor->verticalScrollBar(), &QScrollBar::setValue);
            connect(oldEditor->verticalScrollBar(), &QScrollBar::valueChanged,
                    currentEditor->verticalScrollBar(), &QScrollBar::setValue);
            connect(currentEditor->horizontalScrollBar(), &QScrollBar::valueChanged,
                    oldEditor->horizontalScrollBar(), &QScrollBar::setValue);
            connect(oldEditor->horizontalScrollBar(), &QScrollBar::valueChanged,
                    currentEditor->horizontalScrollBar(), &QScrollBar::setValue);

            auto* btnLayout = new QHBoxLayout();
            auto* closeBtn = new QPushButton("Fechar Comparação", this);
            closeBtn->setMinimumWidth(150);
            btnLayout->addStretch();
            btnLayout->addWidget(closeBtn);
            mainLayout->addLayout(btnLayout);

            connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
        }

    private:
        void applyDiffHighlighting(const QString& current, const QString& old) {
            QStringList currentLines = current.split('\n');
            QStringList oldLines = old.split('\n');

            QList<QTextEdit::ExtraSelection> currentSels;
            QList<QTextEdit::ExtraSelection> oldSels;

            int max = std::max(currentLines.size(), oldLines.size());

            for (int i = 0; i < max; ++i) {
                QString cLine = (i < currentLines.size()) ? currentLines[i] : "";
                QString oLine = (i < oldLines.size()) ? oldLines[i] : "";

                if (cLine != oLine) {
                    // Highlight na versão atual (Verde suave para adição/mudança)
                    if (i < currentLines.size()) {
                        QTextEdit::ExtraSelection selection;
                        selection.format.setBackground(QColor(35, 60, 35)); // Deep Green
                        selection.cursor = QTextCursor(currentEditor->document()->findBlockByLineNumber(i));
                        selection.cursor.select(QTextCursor::BlockUnderCursor);
                        currentSels.append(selection);
                    }

                    // Highlight na versão antiga (Vermelho suave para remoção/mudança)
                    if (i < oldLines.size()) {
                        QTextEdit::ExtraSelection selection;
                        selection.format.setBackground(QColor(60, 35, 35)); // Deep Red
                        selection.cursor = QTextCursor(oldEditor->document()->findBlockByLineNumber(i));
                        selection.cursor.select(QTextCursor::BlockUnderCursor);
                        oldSels.append(selection);
                    }
                }
            }

            currentEditor->setExtraSelections(currentSels);
            oldEditor->setExtraSelections(oldSels);
        }

        QTextEdit* currentEditor;
        QTextEdit* oldEditor;
        Parcel::Editor::ProjectHighlighter* currentHighlighter;
        Parcel::Editor::ProjectHighlighter* oldHighlighter;
    };
}

#endif

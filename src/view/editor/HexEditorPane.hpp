#ifndef HEX_EDITOR_PANE_HPP
#define HEX_EDITOR_PANE_HPP

#include <QWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QFileInfo>
#include <filesystem>

namespace Parcel::View {

    class HexEditorPane : public QWidget {
        Q_OBJECT
    public:
        explicit HexEditorPane(const std::string& filePath, const QByteArray& data, QWidget* parent = nullptr)
            : QWidget(parent), currentFile(filePath), binaryData(data) {

            auto* layout = new QVBoxLayout(this);
            layout->setContentsMargins(0, 0, 0, 0);
            layout->setSpacing(0);

            // Search Bar
            auto* searchLayout = new QHBoxLayout();
            searchLayout->setContentsMargins(10, 5, 10, 5);

            searchField = new QLineEdit(this);
            searchField->setPlaceholderText("Buscar string ou hex...");
            searchField->setStyleSheet("background-color: #2d2d2d; color: #ccc; border: 1px solid #444; border-radius: 3px; padding: 3px;");

            auto* prevBtn = new QPushButton("Anterior", this);
            prevBtn->setStyleSheet("background-color: #3c3c3c; color: white; border-radius: 3px; padding: 3px 10px;");

            auto* nextBtn = new QPushButton("Próximo", this);
            nextBtn->setStyleSheet("background-color: #0e639c; color: white; border-radius: 3px; padding: 3px 10px;");

            searchLayout->addWidget(new QLabel("🔍", this));
            searchLayout->addWidget(searchField);
            searchLayout->addWidget(prevBtn);
            searchLayout->addWidget(nextBtn);

            layout->addLayout(searchLayout);

            hexArea = new QTextEdit(this);
            hexArea->setReadOnly(true);
            hexArea->setFont(QFont("Monospace", 10));
            hexArea->setStyleSheet("background-color: #1e1e1e; color: #d4d4d4; border: none;");

            layout->addWidget(hexArea);

            displayHex();

            connect(nextBtn, &QPushButton::clicked, this, &HexEditorPane::findNext);
            connect(prevBtn, &QPushButton::clicked, this, &HexEditorPane::findPrevious);
            connect(searchField, &QLineEdit::returnPressed, this, &HexEditorPane::findNext);
        }

        std::filesystem::path getFile() const { return currentFile; }
        QByteArray getData() const { return binaryData; }

    private slots:
        void findNext() {
            QString term = searchField->text();
            if (term.isEmpty()) return;

            if (!hexArea->find(term)) {
                // Wrap around to start
                QTextCursor cursor = hexArea->textCursor();
                cursor.movePosition(QTextCursor::Start);
                hexArea->setTextCursor(cursor);
                hexArea->find(term);
            }
        }

        void findPrevious() {
            QString term = searchField->text();
            if (term.isEmpty()) return;

            if (!hexArea->find(term, QTextDocument::FindBackward)) {
                // Wrap around to end
                QTextCursor cursor = hexArea->textCursor();
                cursor.movePosition(QTextCursor::End);
                hexArea->setTextCursor(cursor);
                hexArea->find(term, QTextDocument::FindBackward);
            }
        }

    private:
        void displayHex() {
            QString display;
            const int bytesPerLine = 16;

            for (int i = 0; i < binaryData.size(); i += bytesPerLine) {
                display += QString("%1  ").arg(i, 8, 16, QChar('0')).toUpper();

                QString hexPart;
                QString asciiPart;
                for (int j = 0; j < bytesPerLine; ++j) {
                    if (i + j < binaryData.size()) {
                        unsigned char b = static_cast<unsigned char>(binaryData[i + j]);
                        hexPart += QString("%1 ").arg(b, 2, 16, QChar('0')).toUpper();
                        asciiPart += (b >= 32 && b <= 126) ? static_cast<char>(b) : '.';
                    } else {
                        hexPart += "   ";
                    }
                }
                display += hexPart + " | " + asciiPart + "\n";
            }
            hexArea->setPlainText(display);
        }

        std::filesystem::path currentFile;
        QByteArray binaryData;
        QTextEdit* hexArea;
        QLineEdit* searchField;
    };
}

#endif

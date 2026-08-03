#ifndef PDF_COMPOSER_PANE_HPP
#define PDF_COMPOSER_PANE_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QToolBar>
#include <QFileDialog>
#include <QPdfWriter>
#include <QPainter>
#include <filesystem>

namespace Parcel::View {

    class PdfComposerPane : public QWidget {
        Q_OBJECT
    public:
        explicit PdfComposerPane(const std::string& filePath = "", QWidget* parent = nullptr)
            : QWidget(parent), m_filePath(filePath) {

            auto* layout = new QVBoxLayout(this);
            layout->setContentsMargins(0, 0, 0, 0);
            layout->setSpacing(0);

            auto* toolBar = new QToolBar(this);
            auto* savePdfAction = toolBar->addAction(QIcon::fromTheme("document-save"), "Export to PDF");
            layout->addWidget(toolBar);

            m_editor = new QTextEdit(this);
            m_editor->setPlaceholderText("Escreva seu conteúdo aqui para exportar como PDF...");
            m_editor->setStyleSheet("background-color: white; color: black;");
            layout->addWidget(m_editor);

            connect(savePdfAction, &QAction::triggered, this, &PdfComposerPane::exportToPdf);
        }

        std::filesystem::path getFile() const { return m_filePath; }

    private slots:
        void exportToPdf() {
            QString path = QString::fromStdString(m_filePath);
            if (path.isEmpty() || !path.endsWith(".pdf", Qt::CaseInsensitive)) {
                path = QFileDialog::getSaveFileName(this, "Export to PDF", "", "PDF Files (*.pdf)");
            }

            if (!path.isEmpty()) {
                QPdfWriter writer(path);
                writer.setPageSize(QPageSize(QPageSize::A4));
                m_editor->print(&writer);
            }
        }

    private:
        QTextEdit* m_editor;
        std::string m_filePath;
    };
}

#endif

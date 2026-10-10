// KDE KTextEditor Embedding C++ Template
#include <KTextEditor/Document>
#include <KTextEditor/View>
#include <KTextEditor/Editor>
#include <QVBoxLayout>
#include <QWidget>
#include <QUrl>

QWidget* createKEditorWidget(QWidget* parent, const QString& filePath) {
    KTextEditor::Editor* editor = KTextEditor::Editor::instance();
    KTextEditor::Document* doc = editor->createDocument(parent);

    if (!filePath.isEmpty()) {
        doc->openUrl(QUrl::fromLocalFile(filePath));
    }

    KTextEditor::View* view = doc->createView(parent);
    view->setHighlightingMode("C++");

    return view;
}

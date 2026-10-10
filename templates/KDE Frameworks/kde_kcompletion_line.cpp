// KDE KCompletionLineEdit Auto-Completion Input Template
#include <KCompletionLineEdit>
#include <KCompletion>
#include <QVBoxLayout>
#include <QWidget>

QWidget* createAutoCompletionInput(QWidget* parent) {
    QWidget* container = new QWidget(parent);
    QVBoxLayout* layout = new QVBoxLayout(container);

    KCompletionLineEdit* input = new KCompletionLineEdit(container);
    KCompletion* completion = input->completionObject();

    QStringList suggestions = {"Qt6", "Kirigami", "KConfig", "KIO", "KTextEditor", "KCoreAddons"};
    completion->setItems(suggestions);
    input->setCompletionMode(KCompletion::CompletionAuto);

    layout->addWidget(input);
    return container;
}

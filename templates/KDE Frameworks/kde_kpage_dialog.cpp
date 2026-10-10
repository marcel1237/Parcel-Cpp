// KDE KPageDialog Multi-Page Settings Dialog Template
#include <KPageDialog>
#include <KPageWidgetItem>
#include <QLabel>
#include <QVBoxLayout>

void showKdeMultiPageDialog(QWidget* parent) {
    KPageDialog dialog(parent);
    dialog.setFaceType(KPageDialog::List);
    dialog.setWindowTitle("Parcel C++ Settings");

    // Page 1: General
    QWidget* generalWidget = new QWidget(&dialog);
    QVBoxLayout* genLayout = new QVBoxLayout(generalWidget);
    genLayout->addWidget(new QLabel("General IDE Preferences"));
    KPageWidgetItem* genPage = dialog.addPage(generalWidget, "General");
    genPage->setIcon(QIcon::fromTheme("preferences-system"));

    // Page 2: Editor
    QWidget* editorWidget = new QWidget(&dialog);
    QVBoxLayout* edLayout = new QVBoxLayout(editorWidget);
    edLayout->addWidget(new QLabel("Code Editor Preferences"));
    KPageWidgetItem* edPage = dialog.addPage(editorWidget, "Editor");
    edPage->setIcon(QIcon::fromTheme("accessories-text-editor"));

    dialog.exec();
}

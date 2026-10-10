// KDE KMessageBox Standard Dialogs Template
#include <KMessageBox>
#include <QWidget>

void showKdeInfoDialog(QWidget* parent) {
    KMessageBox::information(parent, "Project saved successfully!", "Parcel C++ KDE Status");
}

bool askKdeConfirmationDialog(QWidget* parent) {
    auto result = KMessageBox::questionTwoActions(
        parent,
        "Are you sure you want to delete this resource?",
        "Confirm Deletion",
        KStandardGuiItem::del(),
        KStandardGuiItem::cancel()
    );
    return (result == KMessageBox::PrimaryAction);
}

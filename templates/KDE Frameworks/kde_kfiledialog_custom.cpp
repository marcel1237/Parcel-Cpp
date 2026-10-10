// KDE KUrlRequester & File Selection Template
#include <KUrlRequester>
#include <QVBoxLayout>
#include <QWidget>
#include <QUrl>
#include <QDebug>

QWidget* createKUrlRequesterWidget(QWidget* parent) {
    QWidget* container = new QWidget(parent);
    QVBoxLayout* layout = new QVBoxLayout(container);

    KUrlRequester* requester = new KUrlRequester(container);
    requester->setMode(KFile::File | KFile::ExistingOnly);
    requester->setFilter("*.cpp *.hpp *.qml");

    QObject::connect(requester, &KUrlRequester::textChanged, [](const QString& text) {
        qDebug() << "Selected URL:" << text;
    });

    layout->addWidget(requester);
    return container;
}

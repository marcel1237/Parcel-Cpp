// KDE KIO Network Transparent File Transfer Template
#include <KIO/CopyJob>
#include <KIO/Job>
#include <QUrl>
#include <QDebug>

void startKioFileCopy(const QString& sourcePath, const QString& destinationPath) {
    QUrl srcUrl = QUrl::fromLocalFile(sourcePath);
    QUrl destUrl = QUrl::fromLocalFile(destinationPath);

    KIO::CopyJob* job = KIO::copy(srcUrl, destUrl, KIO::HideProgressInfo);

    QObject::connect(job, &KIO::Job::result, [](KJob* j) {
        if (j->error() == 0) {
            qDebug() << "KIO File Copy completed successfully!";
        } else {
            qDebug() << "KIO File Copy error:" << j->errorString();
        }
    });
}

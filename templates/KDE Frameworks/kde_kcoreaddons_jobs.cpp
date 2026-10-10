// KDE KJob Async Tracking Template
#include <KJob>
#include <QObject>
#include <QDebug>

class ParcelKdeJobTracker : public QObject {
    Q_OBJECT
public:
    explicit ParcelKdeJobTracker(QObject* parent = nullptr) : QObject(parent) {}

public slots:
    void trackJob(KJob* job) {
        connect(job, &KJob::result, this, [](KJob* j) {
            if (j->error() == 0) {
                qDebug() << "KDE Job completed successfully!";
            } else {
                qDebug() << "KDE Job failed:" << j->errorString();
            }
        });
        job->start();
    }
};

#ifndef DOTNET_SERVICE_HPP
#define DOTNET_SERVICE_HPP

#include <QString>
#include <QStringList>
#include <QProcess>
#include <QObject>
#include <functional>
#include <QList>

namespace Parcel::Service {

    class DotNetService : public QObject {
        Q_OBJECT
    public:
        static DotNetService& getInstance() {
            static DotNetService instance;
            return instance;
        }

        // Check if .NET SDK is available
        bool isDotNetAvailable() {
            QProcess process;
            process.start("dotnet", QStringList() << "--version");
            process.waitForFinished(3000);
            return process.exitCode() == 0;
        }

        // Execute .NET CLI command asynchronously
        void runDotNetCommand(const QStringList& args, const QString& workingDir = QString(), std::function<void(QString)> onOutput = nullptr) {
            auto* process = new QProcess(this);
            if (!workingDir.isEmpty()) {
                process->setWorkingDirectory(workingDir);
            }

            connect(process, &QProcess::readyReadStandardOutput, [process, onOutput]() {
                QString out = QString::fromUtf8(process->readAllStandardOutput());
                if (onOutput) onOutput(out);
            });

            connect(process, &QProcess::readyReadStandardError, [process, onOutput]() {
                QString err = QString::fromUtf8(process->readAllStandardError());
                if (onOutput) onOutput("[ERROR] " + err);
            });

            process->start("dotnet", args);
        }

        // Entity Framework Core Migration helper
        void addEfMigration(const QString& migrationName, const QString& projectDir) {
            runDotNetCommand(QStringList() << "ef" << "migrations" << "add" << migrationName, projectDir);
        }

        void updateDatabase(const QString& projectDir) {
            runDotNetCommand(QStringList() << "ef" << "database" << "update", projectDir);
        }

        // LINQ simulation helper in C++ (lambda-based filtering)
        template<typename T, typename Predicate>
        QList<T> linqWhere(const QList<T>& source, Predicate pred) {
            QList<T> result;
            for (const auto& item : source) {
                if (pred(item)) {
                    result.append(item);
                }
            }
            return result;
        }

    private:
        DotNetService() = default;
        ~DotNetService() = default;
        DotNetService(const DotNetService&) = delete;
        DotNetService& operator=(const DotNetService&) = delete;
    };

}

#endif

#ifndef BACKUP_SERVICE_HPP
#define BACKUP_SERVICE_HPP

#include <string>
#include <vector>
#include <filesystem>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QMap>
#include <set>

namespace Parcel::Service {

    class BackupService {
    public:
        static BackupService& getInstance() {
            static BackupService instance;
            return instance;
        }

        void init(const std::string& projectPath) {
            m_projectPath = projectPath;
            m_dbPath = QString::fromStdString(projectPath) + "/.parcel_backups.db";

            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "backup_db");
            db.setDatabaseName(m_dbPath);

            if (db.open()) {
                QSqlQuery query(db);
                // Tabela de Builds
                query.exec("CREATE TABLE IF NOT EXISTS builds ("
                           "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                           "timestamp DATETIME, "
                           "description TEXT)");

                // Tabela de Arquivos por Build
                query.exec("CREATE TABLE IF NOT EXISTS build_files ("
                           "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                           "build_id INTEGER, "
                           "file_path TEXT, "
                           "content BLOB, "
                           "FOREIGN KEY(build_id) REFERENCES builds(id))");

                // Nova Tabela de Saves Individuais
                query.exec("CREATE TABLE IF NOT EXISTS saves ("
                           "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                           "file_name TEXT, "
                           "file_path TEXT, "
                           "timestamp DATETIME, "
                           "change_summary TEXT, "
                           "content BLOB)");
            }
        }

        void performBuildBackup(const QString& manualDesc = "") {
            QSqlDatabase db = QSqlDatabase::database("backup_db");
            if (!db.isOpen()) return;

            QString diffDesc = generateDiffDescription();
            QString finalDesc = manualDesc.isEmpty() ? diffDesc : manualDesc + "\n" + diffDesc;

            db.transaction();
            QSqlQuery query(db);
            query.prepare("INSERT INTO builds (timestamp, description) VALUES (?, ?)");
            query.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
            query.addBindValue(finalDesc);

            if (!query.exec()) {
                db.rollback();
                return;
            }

            qlonglong buildId = query.lastInsertId().toLongLong();
            std::filesystem::path root(m_projectPath);
            try {
                for (const auto& entry : std::filesystem::recursive_directory_iterator(root)) {
                    if (entry.is_regular_file()) {
                        std::string path = entry.path().string();
                        if (path.find("/build/") != std::string::npos ||
                            path.find("/.git/") != std::string::npos ||
                            path.find(".parcel_backups.db") != std::string::npos) continue;

                        QFile file(QString::fromStdString(path));
                        if (file.open(QIODevice::ReadOnly)) {
                            QByteArray content = file.readAll();
                            file.close();
                            QSqlQuery fileQuery(db);
                            fileQuery.prepare("INSERT INTO build_files (build_id, file_path, content) VALUES (?, ?, ?)");
                            fileQuery.addBindValue(buildId);
                            fileQuery.addBindValue(QString::fromStdString(path).replace(QString::fromStdString(m_projectPath), ""));
                            fileQuery.addBindValue(content);
                            fileQuery.exec();
                        }
                    }
                }
                db.commit();
            } catch (...) { db.rollback(); }
        }

        void performSaveLog(const QString& fullPath, const QByteArray& newContent) {
            QSqlDatabase db = QSqlDatabase::database("backup_db");
            if (!db.isOpen()) return;

            QFileInfo info(fullPath);
            QString relPath = fullPath;
            relPath.replace(QString::fromStdString(m_projectPath), "");

            // Gera o sumário comparando com o último save deste arquivo
            QString summary = generateSingleFileDiff(relPath, newContent);

            QSqlQuery query(db);
            query.prepare("INSERT INTO saves (file_name, file_path, timestamp, change_summary, content) "
                          "VALUES (?, ?, ?, ?, ?)");
            query.addBindValue(info.fileName());
            query.addBindValue(relPath);
            query.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
            query.addBindValue(summary);
            query.addBindValue(newContent);
            query.exec();
        }

        struct FileVersion {
            int id;
            QString timestamp;
            QString summary;
        };

        QList<FileVersion> getFileHistory(const QString& fullPath) {
            QList<FileVersion> history;
            QSqlDatabase db = QSqlDatabase::database("backup_db");
            if (!db.isOpen()) return history;

            QString relPath = fullPath;
            relPath.replace(QString::fromStdString(m_projectPath), "");

            QSqlQuery q(db);
            q.prepare("SELECT id, timestamp, change_summary FROM saves WHERE file_path = ? ORDER BY id DESC");
            q.addBindValue(relPath);
            if (q.exec()) {
                while (q.next()) {
                    history.append({q.value(0).toInt(), q.value(1).toString(), q.value(2).toString()});
                }
            }
            return history;
        }

        bool restoreFileFromSave(int saveId, const QString& fullPath) {
            QByteArray content = getSaveContent(saveId);
            if (!content.isEmpty()) {
                QFile file(fullPath);
                if (file.open(QIODevice::WriteOnly)) {
                    file.write(content);
                    file.close();
                    return true;
                }
            }
            return false;
        }

        QByteArray getSaveContent(int saveId) {
            QSqlDatabase db = QSqlDatabase::database("backup_db");
            if (!db.isOpen()) return QByteArray();

            QSqlQuery q(db);
            q.prepare("SELECT content FROM saves WHERE id = ?");
            q.addBindValue(saveId);
            if (q.exec() && q.next()) {
                return q.value(0).toByteArray();
            }
            return QByteArray();
        }

    private:
        BackupService() {}
        std::string m_projectPath;
        QString m_dbPath;

        QString generateSingleFileDiff(const QString& relPath, const QByteArray& newContent) {
            QSqlDatabase db = QSqlDatabase::database("backup_db");
            QSqlQuery q(db);
            q.prepare("SELECT content FROM saves WHERE file_path = ? ORDER BY id DESC LIMIT 1");
            q.addBindValue(relPath);
            q.exec();

            if (!q.next()) return "Primeira versão salva (Incremental)";

            QByteArray oldContent = q.value(0).toByteArray();
            if (oldContent == newContent) return "Nenhuma alteração detectada no conteúdo.";

            // Log de alteração simplificado (tamanho e hash/comparação básica)
            int diffSize = newContent.size() - oldContent.size();
            QString trend = (diffSize >= 0) ? QString("+%1 bytes").arg(diffSize) : QString("%1 bytes").arg(diffSize);
            return QString("Arquivo modificado (%1).").arg(trend);
        }

        QString generateDiffDescription() {
            QSqlDatabase db = QSqlDatabase::database("backup_db");
            QSqlQuery q("SELECT id FROM builds ORDER BY id DESC LIMIT 1", db);
            if (!q.next()) return "Initial Project State (Full Backup)";

            int lastId = q.value(0).toInt();
            QMap<QString, QByteArray> lastFiles;
            QSqlQuery fq(db);
            fq.prepare("SELECT file_path, content FROM build_files WHERE build_id = ?");
            fq.addBindValue(lastId);
            fq.exec();
            while (fq.next()) lastFiles[fq.value(0).toString()] = fq.value(1).toByteArray();

            QStringList added, modified, deleted;
            std::set<QString> currentPaths;
            std::filesystem::path root(m_projectPath);

            try {
                for (const auto& entry : std::filesystem::recursive_directory_iterator(root)) {
                    if (entry.is_regular_file()) {
                        QString relPath = QString::fromStdString(entry.path().string()).replace(QString::fromStdString(m_projectPath), "");
                        if (relPath.contains("/build/") || relPath.contains("/.git/") || relPath.contains(".parcel_backups.db")) continue;

                        currentPaths.insert(relPath);
                        QFile file(QString::fromStdString(entry.path().string()));
                        if (file.open(QIODevice::ReadOnly)) {
                            QByteArray content = file.readAll();
                            if (!lastFiles.contains(relPath)) {
                                added << relPath;
                            } else if (lastFiles[relPath] != content) {
                                modified << relPath;
                            }
                        }
                    }
                }
            } catch (...) {}

            for (auto it = lastFiles.begin(); it != lastFiles.end(); ++it) {
                if (currentPaths.find(it.key()) == currentPaths.end()) deleted << it.key();
            }

            QString report = "Changes compared to previous build:\n";
            if (!added.isEmpty()) report += "➕ New Files: " + added.join(", ") + "\n";
            if (!modified.isEmpty()) report += "📝 Modified: " + modified.join(", ") + "\n";
            if (!deleted.isEmpty()) report += "🗑️ Deleted: " + deleted.join(", ") + "\n";
            if (added.isEmpty() && modified.isEmpty() && deleted.isEmpty()) report = "No file changes detected since last build.";

            return report;
        }
    };
}

#endif

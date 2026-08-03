#ifndef DBMS_EXPLORER_HPP
#define DBMS_EXPLORER_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QHeaderView>
#include <QFileInfo>
#include <QDir>
#include <QMenu>
#include <QMessageBox>
#include <QDateTime>
#include <QFileDialog>
#include <filesystem>

namespace Parcel::View {

    class DBMSExplorer : public QWidget {
        Q_OBJECT
    public:
        explicit DBMSExplorer(const std::string& projectPath, QWidget* parent = nullptr)
            : QWidget(parent), m_projectPath(projectPath) {
            auto* layout = new QVBoxLayout(this);
            layout->setContentsMargins(0, 0, 0, 0);

            treeWidget = new QTreeWidget(this);
            treeWidget->setHeaderLabels({"Database", "Tipo"});
            treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
            treeWidget->setStyleSheet("QTreeWidget { background-color: #1e1f22; border: none; color: #bcbec4; } "
                                    "QHeaderView::section { background-color: #2b2d30; color: #aaa; border: none; padding: 4px; }");

            layout->addWidget(treeWidget);

            connect(treeWidget, &QTreeWidget::customContextMenuRequested, this, &DBMSExplorer::showContextMenu);
            connect(treeWidget, &QTreeWidget::itemDoubleClicked, this, &DBMSExplorer::onItemDoubleClicked);

            refresh();
        }

        void refresh() {
            treeWidget->clear();
            QDir resourceDir(QString::fromStdString(m_projectPath) + "/resource");

            struct DBType { QString ext; QString label; };
            QList<DBType> types = {
                {".db", "SQLite"}, {".sqlite", "SQLite"}, {".sqlite3", "SQLite"},
                {".duckdb", "DuckDB"}, {".csv", "DuckDB (CSV)"},
                {".rdb", "Redis"}, {".ldb", "LevelDB"}, {".rocksdb", "RocksDB"}
            };

            for (const auto& entry : resourceDir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot)) {
                if (entry.fileName() == ".trash") continue;

                QString ext = entry.suffix().isEmpty() ? "" : "." + entry.suffix().toLower();
                QString typeLabel = "Unknown";
                bool isDB = false;

                for (const auto& type : types) {
                    if (ext == type.ext || entry.fileName().contains(type.label, Qt::CaseInsensitive)) {
                        typeLabel = type.label;
                        isDB = true;
                        break;
                    }
                }

                if (isDB) {
                    auto* item = new QTreeWidgetItem(treeWidget);
                    item->setText(0, entry.fileName());
                    item->setText(1, typeLabel);
                    item->setIcon(0, QIcon::fromTheme("network-server"));
                    item->setData(0, Qt::UserRole, entry.absoluteFilePath());
                }
            }
        }

    signals:
        void databaseSelected(const QString& path);

    private slots:
        void showContextMenu(const QPoint& pos) {
            QTreeWidgetItem* item = treeWidget->itemAt(pos);

            QMenu menu(this);
            if (item) {
                auto* backupAction = menu.addAction(QIcon::fromTheme("document-save-as"), "Fazer Back Up");
                auto* restoreAction = menu.addAction(QIcon::fromTheme("document-revert"), "Restaurar Back Up");
                menu.addSeparator();
                auto* trashAction = menu.addAction(QIcon::fromTheme("user-trash"), "Mover para Lixeira");

                QAction* selected = menu.exec(treeWidget->mapToGlobal(pos));
                if (selected == backupAction) {
                    performBackup(item->data(0, Qt::UserRole).toString());
                } else if (selected == restoreAction) {
                    performRestore(item->data(0, Qt::UserRole).toString());
                } else if (selected == trashAction) {
                    performMoveToTrash(item->data(0, Qt::UserRole).toString());
                }
            } else {
                auto* restoreFromTrashAction = menu.addAction(QIcon::fromTheme("user-trash"), "Restaurar da Lixeira...");
                auto* emptyTrashAction = menu.addAction(QIcon::fromTheme("user-trash-full"), "Esvaziar Lixeira");
                menu.addSeparator();
                auto* refreshAction = menu.addAction(QIcon::fromTheme("view-refresh"), "Atualizar Lista");

                QAction* selected = menu.exec(treeWidget->mapToGlobal(pos));
                if (selected == restoreFromTrashAction) {
                    performRestoreFromTrash();
                } else if (selected == emptyTrashAction) {
                    performEmptyTrash();
                } else if (selected == refreshAction) {
                    refresh();
                }
            }
        }

        void onItemDoubleClicked(QTreeWidgetItem* item, int column) {
            (void)column;
            emit databaseSelected(item->data(0, Qt::UserRole).toString());
        }

        void performBackup(const QString& sourcePath) {
            QFileInfo info(sourcePath);
            QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
            QString backupName = info.baseName() + "_backup_" + timestamp + "." + info.suffix();
            QString destPath = info.absolutePath() + "/" + backupName;

            try {
                if (info.isDir()) {
                    std::filesystem::copy(sourcePath.toStdString(), destPath.toStdString(), std::filesystem::copy_options::recursive);
                } else {
                    QFile::copy(sourcePath, destPath);
                }
                QMessageBox::information(this, "Back Up", "Back up concluído com sucesso:\n" + backupName);
                refresh();
            } catch (const std::exception& e) {
                QMessageBox::critical(this, "Erro no Back Up", e.what());
            }
        }

        void performRestore(const QString& targetPath) {
            QFileInfo info(targetPath);
            QString filter = "Backup Files (*_backup_*." + info.suffix() + ");;All Files (*)";
            QString backupPath = QFileDialog::getOpenFileName(this, "Selecionar Arquivo de Back Up", info.absolutePath(), filter);

            if (backupPath.isEmpty()) return;

            auto result = QMessageBox::warning(this, "Confirmar Restauração",
                "Tem certeza que deseja restaurar este backup? Os dados atuais em '" + info.fileName() + "' serão sobrescritos!",
                QMessageBox::Yes | QMessageBox::No);

            if (result == QMessageBox::Yes) {
                try {
                    if (info.isDir()) {
                        std::filesystem::remove_all(targetPath.toStdString());
                        std::filesystem::copy(backupPath.toStdString(), targetPath.toStdString(), std::filesystem::copy_options::recursive);
                    } else {
                        QFile::remove(targetPath);
                        QFile::copy(backupPath, targetPath);
                    }
                    QMessageBox::information(this, "Sucesso", "Backup restaurado com sucesso!");
                    refresh();
                } catch (const std::exception& e) {
                    QMessageBox::critical(this, "Erro na Restauração", e.what());
                }
            }
        }

        void performMoveToTrash(const QString& sourcePath) {
            QFileInfo info(sourcePath);
            QString trashPath = QString::fromStdString(m_projectPath) + "/resource/.trash";
            QDir().mkpath(trashPath);

            QString destPath = trashPath + "/" + info.fileName() + "_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");

            try {
                std::filesystem::rename(sourcePath.toStdString(), destPath.toStdString());
                QMessageBox::information(this, "Lixeira", "Arquivo movido para a lixeira oculta (.trash)");
                refresh();
            } catch (...) {
                // Fallback para cópia se estiver em sistemas de arquivos diferentes
                try {
                    if (info.isDir()) {
                        std::filesystem::copy(sourcePath.toStdString(), destPath.toStdString(), std::filesystem::copy_options::recursive);
                        std::filesystem::remove_all(sourcePath.toStdString());
                    } else {
                        QFile::copy(sourcePath, destPath);
                        QFile::remove(sourcePath);
                    }
                    refresh();
                } catch (const std::exception& e) {
                    QMessageBox::critical(this, "Erro ao mover", e.what());
                }
            }
        }

        void performEmptyTrash() {
            QString trashPath = QString::fromStdString(m_projectPath) + "/resource/.trash";
            if (!QDir(trashPath).exists()) return;

            auto result = QMessageBox::warning(this, "Esvaziar Lixeira",
                "Tem certeza que deseja apagar permanentemente todos os arquivos da lixeira?",
                QMessageBox::Yes | QMessageBox::No);

            if (result == QMessageBox::Yes) {
                try {
                    std::filesystem::remove_all(trashPath.toStdString());
                    QMessageBox::information(this, "Sucesso", "Lixeira esvaziada!");
                    refresh();
                } catch (const std::exception& e) {
                    QMessageBox::critical(this, "Erro", e.what());
                }
            }
        }

        void performRestoreFromTrash() {
            QString trashPath = QString::fromStdString(m_projectPath) + "/resource/.trash";
            if (!QDir(trashPath).exists()) {
                QMessageBox::information(this, "Lixeira", "A lixeira está vazia.");
                return;
            }

            QString backupPath = QFileDialog::getOpenFileName(this, "Restaurar da Lixeira", trashPath, "Todos os Arquivos (*)");
            if (backupPath.isEmpty()) return;

            QFileInfo info(backupPath);
            QString originalName = info.fileName();
            // Tenta remover o sufixo de timestamp _yyyyMMdd_HHmmss (15 caracteres: _ + 8 + _ + 6)
            if (originalName.length() > 16 && originalName.at(originalName.length() - 16) == '_') {
                originalName = originalName.left(originalName.length() - 16);
            }

            QString destPath = QString::fromStdString(m_projectPath) + "/resource/" + originalName;

            try {
                if (std::filesystem::exists(destPath.toStdString())) {
                    auto result = QMessageBox::question(this, "Arquivo Existente",
                        "O arquivo '" + originalName + "' já existe na pasta resource. Deseja sobrescrever?",
                        QMessageBox::Yes | QMessageBox::No);
                    if (result == QMessageBox::No) return;
                    std::filesystem::remove_all(destPath.toStdString());
                }

                std::filesystem::rename(backupPath.toStdString(), destPath.toStdString());
                QMessageBox::information(this, "Sucesso", "Arquivo restaurado: " + originalName);
                refresh();
            } catch (const std::exception& e) {
                QMessageBox::critical(this, "Erro ao restaurar", e.what());
            }
        }

    private:
        QTreeWidget* treeWidget;
        std::string m_projectPath;
    };
}

#endif

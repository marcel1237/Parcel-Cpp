#ifndef SAVES_PANE_HPP
#define SAVES_PANE_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QPushButton>
#include <QLabel>
#include <QFileInfo>

namespace Parcel::View {

    class SavesPane : public QWidget {
        Q_OBJECT
    public:
        explicit SavesPane(QWidget* parent = nullptr) : QWidget(parent) {
            auto* layout = new QVBoxLayout(this);
            layout->setContentsMargins(5, 5, 5, 5);

            auto* header = new QLabel("💾 Log de Alterações (Saves Individuais)", this);
            header->setStyleSheet("font-weight: bold; color: #dfe1e5; padding: 5px;");
            layout->addWidget(header);

            table = new QTableWidget(this);
            table->setColumnCount(4);
            table->setHorizontalHeaderLabels({"Arquivo", "Data/Hora", "Mudança", "Caminho"});
            table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
            table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
            table->setStyleSheet("QTableWidget { background-color: #1e1f22; color: #bcbec4; }");
            table->setSelectionBehavior(QAbstractItemView::SelectRows);
            layout->addWidget(table);

            auto* btnLayout = new QHBoxLayout();
            auto* btnRefresh = new QPushButton("Atualizar Log", this);
            auto* btnPreview = new QPushButton("👁️ Ver Código Antigo", this);

            btnLayout->addWidget(btnRefresh);
            btnLayout->addWidget(btnPreview);
            layout->addLayout(btnLayout);

            connect(btnRefresh, &QPushButton::clicked, this, &SavesPane::loadSaves);
            connect(btnPreview, &QPushButton::clicked, this, &SavesPane::handlePreview);
            connect(table, &QTableWidget::itemDoubleClicked, this, &SavesPane::handlePreview);

            loadSaves();
        }

    signals:
        void previewRequested(const QString& fileName, const QString& content);

    public slots:
        void loadSaves() {
            table->setRowCount(0);
            QSqlDatabase db = QSqlDatabase::database("backup_db");
            if (!db.isOpen()) return;

            QSqlQuery query("SELECT file_name, timestamp, change_summary, file_path, id FROM saves ORDER BY id DESC LIMIT 200", db);
            while (query.next()) {
                int row = table->rowCount();
                table->insertRow(row);
                table->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
                table->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
                table->setItem(row, 2, new QTableWidgetItem(query.value(2).toString()));
                table->setItem(row, 3, new QTableWidgetItem(query.value(3).toString()));
                table->item(row, 0)->setData(Qt::UserRole, query.value(4).toInt()); // Store ID
            }
        }

    private slots:
        void handlePreview() {
            int row = table->currentRow();
            if (row < 0) return;

            int saveId = table->item(row, 0)->data(Qt::UserRole).toInt();
            QString fileName = table->item(row, 0)->text();
            QString timestamp = table->item(row, 1)->text();

            QSqlDatabase db = QSqlDatabase::database("backup_db");
            QSqlQuery query(db);
            query.prepare("SELECT content FROM saves WHERE id = ?");
            query.addBindValue(saveId);

            if (query.exec() && query.next()) {
                QString content = QString::fromUtf8(query.value(0).toByteArray());
                emit previewRequested(fileName + " (Backup " + timestamp + ")", content);
            }
        }

    private:
        QTableWidget* table;
    };
}

#endif

#ifndef VERSIONS_PANE_HPP
#define VERSIONS_PANE_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>

namespace Parcel::View {

    class VersionsPane : public QWidget {
        Q_OBJECT
    public:
        explicit VersionsPane(const std::string& projectPath, QWidget* parent = nullptr)
            : QWidget(parent), m_projectPath(projectPath) {
            auto* layout = new QVBoxLayout(this);
            layout->setContentsMargins(5, 5, 5, 5);

            auto* header = new QLabel("🕰️ Histórico de Builds & Versionamento", this);
            header->setStyleSheet("font-weight: bold; color: #dfe1e5; padding: 5px;");
            layout->addWidget(header);

            table = new QTableWidget(this);
            table->setColumnCount(3);
            table->setHorizontalHeaderLabels({"ID", "Data/Hora", "Descrição"});
            table->horizontalHeader()->setStretchLastSection(true);
            table->setStyleSheet("QTableWidget { background-color: #1e1f22; color: #bcbec4; }");
            table->setSelectionBehavior(QAbstractItemView::SelectRows);
            layout->addWidget(table);

            auto* btnRefresh = new QPushButton("Atualizar", this);
            connect(btnRefresh, &QPushButton::clicked, this, &VersionsPane::loadVersions);
            layout->addWidget(btnRefresh);

            loadVersions();
        }

        void loadVersions() {
            table->setRowCount(0);
            QSqlDatabase db = QSqlDatabase::database("backup_db");
            if (!db.isOpen()) return;

            QSqlQuery query("SELECT id, timestamp, description FROM builds ORDER BY id DESC", db);
            while (query.next()) {
                int row = table->rowCount();
                table->insertRow(row);
                table->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
                table->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
                table->setItem(row, 2, new QTableWidgetItem(query.value(2).toString()));
            }
        }

    private:
        QTableWidget* table;
        std::string m_projectPath;
    };
}

#endif

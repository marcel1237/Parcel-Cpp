#ifndef DATABASE_PANE_HPP
#define DATABASE_PANE_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QStandardItemModel>
#include <QTableView>
#include <QHeaderView>
#include <QFileDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QMessageBox>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QCompleter>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QDialog>
#include <QStringListModel>
#include "../service/PdfService.hpp"

namespace Parcel::View {

    class DatabasePane : public QWidget {
        Q_OBJECT
    public:
        explicit DatabasePane(QWidget* parent = nullptr) : QWidget(parent) {
            auto* layout = new QVBoxLayout(this);
            layout->setContentsMargins(6, 6, 6, 6);
            layout->setSpacing(5);

            // Toolbar
            auto* toolbar = new QHBoxLayout();
            dbTypeCombo = new QComboBox(this);
            dbTypeCombo->addItems({
                "SQLite",
                "DuckDB (CLI)",
                "Redis (CLI)",
                "LevelDB (CLI)",
                "RocksDB (CLI)",
                "BerkeleyDB (CLI)",
                "MongoDB (CLI)",
                "Firebird (Planned)"
            });
            dbTypeCombo->setStyleSheet("background-color: #2b2d30; color: #bcbec4; padding: 4px;");

            auto* openBtn = new QPushButton("Abrir DB", this);
            openBtn->setIcon(QIcon::fromTheme("document-open"));

            auto* refreshBtn = new QPushButton("Refresh", this);
            refreshBtn->setIcon(QIcon::fromTheme("view-refresh"));

            auto* chartBtn = new QPushButton("Gráfico", this);
            chartBtn->setIcon(QIcon::fromTheme("office-chart-bar"));
            chartBtn->setStyleSheet("background-color: #8e44ad; color: white; font-weight: bold;");

            toolbar->addWidget(new QLabel("Tipo:", this));
            toolbar->addWidget(dbTypeCombo);
            toolbar->addWidget(openBtn);
            toolbar->addWidget(refreshBtn);
            toolbar->addWidget(chartBtn);

            tableSelector = new QComboBox(this);
            tableSelector->setPlaceholderText("Selecione uma tabela...");
            toolbar->addStretch();
            toolbar->addWidget(new QLabel("Tabela:", this));
            toolbar->addWidget(tableSelector);

            layout->addLayout(toolbar);

            // SQL Execution Area
            auto* sqlLayout = new QHBoxLayout();
            sqlInput = new QLineEdit(this);
            sqlInput->setPlaceholderText("Comando SQL... (SQLite ou DuckDB)");
            sqlInput->setStyleSheet("background-color: #1e1f22; color: #bcbec4; border: 1px solid #3c3f41; padding: 6px;");

            auto* runBtn = new QPushButton("Executar SQL", this);
            runBtn->setIcon(QIcon::fromTheme("system-run"));
            runBtn->setStyleSheet("background-color: #0e639c; color: white; font-weight: bold; padding: 6px 12px;");

            sqlLayout->addWidget(sqlInput, 1);
            sqlLayout->addWidget(runBtn);
            layout->addLayout(sqlLayout);

            // SQL Result Area
            m_tableView = new QTableView(this);
            m_tableView->setAlternatingRowColors(true);
            m_tableView->setStyleSheet("QTableView { background-color: #1e1f22; gridline-color: #3c3f41; color: #bcbec4; } "
                                     "QHeaderView::section { background-color: #2b2d30; color: #aaa; border: none; padding: 4px; }");
            m_tableView->horizontalHeader()->setStretchLastSection(true);

            layout->addWidget(m_tableView);

            m_model = new QSqlTableModel(this);
            m_queryModel = new QSqlQueryModel(this);
            m_duckModel = new QStandardItemModel(this);
            m_duckProcess = new QProcess(this);

            setupCompleter();

            connect(openBtn, &QPushButton::clicked, this, &DatabasePane::handleOpenDB);
            connect(refreshBtn, &QPushButton::clicked, this, &DatabasePane::refreshData);
            connect(runBtn, &QPushButton::clicked, this, &DatabasePane::handleExecuteSQL);
            connect(sqlInput, &QLineEdit::returnPressed, this, &DatabasePane::handleExecuteSQL);
            connect(chartBtn, &QPushButton::clicked, this, &DatabasePane::showChartMenu);
            connect(tableSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DatabasePane::loadTable);

            m_tableView->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(m_tableView, &QTableView::customContextMenuRequested, this, &DatabasePane::showTableContextMenu);
        }

        void openSpecificDB(const QString& path) {
            m_currentDbPath = path;
            QFileInfo info(path);
            QString ext = info.suffix().toLower();

            if (ext == "db" || ext == "sqlite" || ext == "sqlite3") {
                dbTypeCombo->setCurrentText("SQLite");
                QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "current_db");
                db.setDatabaseName(path);
                if (db.open()) {
                    updateTableList(db);
                    QMessageBox::information(this, "SQLite", "Conectado ao banco: " + info.fileName());
                } else {
                    QMessageBox::critical(this, "Erro", "Não foi possível abrir o banco SQLite.");
                }
            } else if (ext == "duckdb" || ext == "csv" || ext == "parquet") {
                dbTypeCombo->setCurrentText("DuckDB (CLI)");
                QMessageBox::information(this, "DuckDB", "Arquivo pronto para análise via DuckDB: " + info.fileName() +
                    "\n\nExemplo: SELECT * FROM '" + path + "' LIMIT 10");
                sqlInput->setText("SELECT * FROM '" + path + "' LIMIT 10");
            }
        }

    private slots:
        void showChartMenu() {
            QMenu menu(this);
            auto* barAction = menu.addAction(QIcon::fromTheme("office-chart-bar"), "Gráfico de Barras");
            auto* pieAction = menu.addAction(QIcon::fromTheme("office-chart-pie"), "Gráfico de Pizza");
            menu.addSeparator();
            auto* pdfAction = menu.addAction(QIcon::fromTheme("document-new"), "Gerar Relatório PDF Completo");

            QAction* selected = menu.exec(QCursor::pos());
            if (selected == barAction) showBarChart();
            else if (selected == pieAction) showPieChart();
            else if (selected == pdfAction) exportFullPdfReport();
        }

        void showBarChart() {
            QAbstractItemModel* model = m_tableView->model();
            if (!model || model->rowCount() == 0) return;

            QDialog* chartDialog = new QDialog(this);
            chartDialog->setWindowTitle("Gráfico de Barras");
            chartDialog->resize(800, 500);
            auto* layout = new QVBoxLayout(chartDialog);

            auto *set0 = new QBarSet("Valores");
            QStringList categories;
            for (int i = 0; i < qMin(model->rowCount(), 20); ++i) {
                categories << model->data(model->index(i, 0)).toString();
                bool ok;
                double val = model->data(model->index(i, 1)).toDouble(&ok);
                *set0 << (ok ? val : 0.0);
            }

            auto *series = new QBarSeries();
            series->append(set0);

            auto *chart = new QChart();
            chart->addSeries(series);
            chart->setTheme(QChart::ChartThemeDark);
            chart->setAnimationOptions(QChart::SeriesAnimations);

            auto *axisX = new QBarCategoryAxis();
            axisX->append(categories);
            chart->addAxis(axisX, Qt::AlignBottom);
            series->attachAxis(axisX);

            auto *axisY = new QValueAxis();
            chart->addAxis(axisY, Qt::AlignLeft);
            series->attachAxis(axisY);

            auto *chartView = new QChartView(chart);
            chartView->setRenderHint(QPainter::Antialiasing);
            layout->addWidget(chartView);

            auto* btnExport = new QPushButton("💾 Salvar como Imagem", chartDialog);
            btnExport->setStyleSheet("background-color: #27ae60; color: white; padding: 10px; font-weight: bold;");
            layout->addWidget(btnExport);

            connect(btnExport, &QPushButton::clicked, [this, chartView, chartDialog]() {
                QString path = QFileDialog::getSaveFileName(chartDialog, "Exportar Gráfico", "grafico_barras.png", "Images (*.png *.jpg)");
                if (!path.isEmpty()) {
                    QPixmap pixmap = chartView->grab();
                    pixmap.save(path);
                    QMessageBox::information(chartDialog, "Sucesso", "Gráfico exportado!");
                }
            });

            chartDialog->exec();
        }

        void showPieChart() {
            QAbstractItemModel* model = m_tableView->model();
            if (!model || model->rowCount() == 0) return;

            QDialog* chartDialog = new QDialog(this);
            chartDialog->setWindowTitle("Gráfico de Pizza");
            chartDialog->resize(600, 600);
            auto* layout = new QVBoxLayout(chartDialog);

            auto *series = new QPieSeries();
            for (int i = 0; i < qMin(model->rowCount(), 15); ++i) {
                QString label = model->data(model->index(i, 0)).toString();
                bool ok;
                double val = model->data(model->index(i, 1)).toDouble(&ok);
                if (ok) series->append(label, val);
            }

            if (series->count() > 0) {
                series->setLabelsVisible(true);
                series->setLabelsPosition(QPieSlice::LabelOutside);

                auto *chart = new QChart();
                chart->addSeries(series);
                chart->setTitle("Distribuição de Dados");
                chart->setTheme(QChart::ChartThemeDark);
                chart->setAnimationOptions(QChart::AllAnimations);

                auto *chartView = new QChartView(chart);
                chartView->setRenderHint(QPainter::Antialiasing);
                layout->addWidget(chartView);

                auto* btnExport = new QPushButton("💾 Salvar como Imagem", chartDialog);
                btnExport->setStyleSheet("background-color: #27ae60; color: white; padding: 10px; font-weight: bold;");
                layout->addWidget(btnExport);

                connect(btnExport, &QPushButton::clicked, [this, chartView, chartDialog]() {
                    QString path = QFileDialog::getSaveFileName(chartDialog, "Exportar Gráfico", "grafico_pizza.png", "Images (*.png *.jpg)");
                    if (!path.isEmpty()) {
                        QPixmap pixmap = chartView->grab();
                        pixmap.save(path);
                        QMessageBox::information(chartDialog, "Sucesso", "Gráfico exportado!");
                    }
                });

                chartDialog->exec();
            } else {
                QMessageBox::warning(this, "Aviso", "Não há dados numéricos na segunda coluna para gerar pizza.");
            }
        }

        void exportFullPdfReport() {
            QAbstractItemModel* model = m_tableView->model();
            if (!model || model->rowCount() == 0) return;

            QString path = QFileDialog::getSaveFileName(this, "Salvar Relatório PDF", "relatorio_projeto.pdf", "PDF Files (*.pdf)");
            if (path.isEmpty()) return;

            // 1. Criar Gráfico Temporário para o PDF
            auto *set0 = new QBarSet("Dados");
            QStringList categories;
            for (int r = 0; r < qMin(model->rowCount(), 15); ++r) {
                categories << model->data(model->index(r, 0)).toString();
                *set0 << model->data(model->index(r, 1)).toDouble();
            }

            auto *series = new QBarSeries();
            series->append(set0);
            auto *chart = new QChart();
            chart->addSeries(series);
            chart->setTitle("Gráfico de Resumo");
            chart->setTheme(QChart::ChartThemeLight); // Fundo branco para o PDF

            QChartView chartView(chart);
            chartView.setRenderHint(QPainter::Antialiasing);
            chartView.resize(1000, 600);
            QPixmap pixmap = chartView.grab();

            // 2. Gerar Tabela HTML
            QString html = "<table><tr>";
            for (int c = 0; c < model->columnCount(); ++c) {
                html += "<th>" + model->headerData(c, Qt::Horizontal).toString() + "</th>";
            }
            html += "</tr>";

            for (int r = 0; r < qMin(model->rowCount(), 100); ++r) {
                html += "<tr>";
                for (int c = 0; c < model->columnCount(); ++c) {
                    html += "<td>" + model->data(model->index(r, c)).toString() + "</td>";
                }
                html += "</tr>";
            }
            html += "</table>";

            // 3. Chamar Serviço
            Service::PdfService pdf;
            pdf.generateDatabaseReport(path, "Relatório Parcel C++: " + tableSelector->currentText(), html, pixmap);

            QMessageBox::information(this, "Sucesso", "Relatório PDF gerado com sucesso!");
        }
        void handleOpenDB() {
            QString filter = "Database Files (*.db *.sqlite *.sqlite3 *.duckdb *.csv *.parquet);;All Files (*)";
            QString path = QFileDialog::getOpenFileName(this, "Selecionar Banco de Dados", "", filter);
            if (path.isEmpty()) return;

            m_currentDbPath = path;

            if (dbTypeCombo->currentText() == "SQLite") {
                QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "current_db");
                db.setDatabaseName(path);
                if (!db.open()) {
                    QMessageBox::critical(this, "Erro", "Erro SQLite: " + db.lastError().text());
                    return;
                }
                updateTableList(db);
                QMessageBox::information(this, "Sucesso", "SQLite conectado!");
            } else {
                QMessageBox::information(this, "DuckDB", "DuckDB (CLI) selecionado. Use o console SQL para consultas no arquivo: " + QFileInfo(path).fileName());
            }
        }

        void handleExecuteSQL() {
            QString queryText = sqlInput->text().trimmed();
            if (queryText.isEmpty()) return;

            QString type = dbTypeCombo->currentText();
            if (type == "DuckDB (CLI)") {
                executeDuckQuery(queryText);
                return;
            } else if (type == "Redis (CLI)") {
                executeRedisQuery(queryText);
                return;
            } else if (type == "LevelDB (CLI)") {
                executeLevelDBQuery(queryText);
                return;
            } else if (type == "RocksDB (CLI)") {
                executeRocksDBQuery(queryText);
                return;
            } else if (type == "BerkeleyDB (CLI)") {
                executeBerkeleyDBQuery(queryText);
                return;
            } else if (type == "MongoDB (CLI)") {
                executeMongoQuery(queryText);
                return;
            }

            QSqlDatabase db = QSqlDatabase::database("current_db");
            if (!db.isOpen()) {
                QMessageBox::warning(this, "Aviso", "Abra um SQLite primeiro!");
                return;
            }

            if (queryText.toUpper().startsWith("SELECT")) {
                m_queryModel->setQuery(queryText, db);
                m_tableView->setModel(m_queryModel);
                tableSelector->setCurrentIndex(-1);
            } else {
                QSqlQuery query(db);
                if (query.exec(queryText)) QMessageBox::information(this, "Sucesso", "SQL executado.");
                else QMessageBox::critical(this, "Erro", query.lastError().text());
            }
        }

        void updateTableList(QSqlDatabase db) {
            tableSelector->clear();
            tableSelector->addItems(db.tables());
        }

        void loadTable(int index) {
            if (index < 0) return;
            QSqlDatabase db = QSqlDatabase::database("current_db");
            m_model->setTable(tableSelector->currentText());
            m_model->select();
            m_tableView->setModel(m_model);
        }

        void refreshData() {
            if (dbTypeCombo->currentText() == "SQLite") m_model->select();
        }

        void executeDuckQuery(const QString& sql) {
            m_duckModel->clear();
            QStringList args;
            // Executa em modo JSON para facilitar o parsing automático das colunas
            args << "-json" << "-c" << sql;

            if (!m_currentDbPath.isEmpty() && !m_currentDbPath.endsWith(".csv")) {
                // Se for um arquivo .duckdb, passamos como banco principal
                args.prepend(m_currentDbPath);
            }

            m_duckProcess->start("duckdb", args);
            if (!m_duckProcess->waitForFinished(5000)) {
                QMessageBox::critical(this, "Erro DuckDB", "O comando demorou muito ou o 'duckdb' CLI não está instalado.");
                return;
            }

            QByteArray output = m_duckProcess->readAllStandardOutput();
            QJsonDocument doc = QJsonDocument::fromJson(output);

            if (doc.isArray()) {
                QJsonArray rootArr = doc.array();
                if (rootArr.isEmpty()) return;

                // Headers
                QJsonObject first = rootArr.at(0).toObject();
                QStringList keys = first.keys();
                m_duckModel->setHorizontalHeaderLabels(keys);

                // Data
                for (int i = 0; i < rootArr.size(); ++i) {
                    QJsonObject obj = rootArr.at(i).toObject();
                    QList<QStandardItem*> rowItems;
                    for (const QString& key : keys) {
                        rowItems << new QStandardItem(obj.value(key).toVariant().toString());
                    }
                    m_duckModel->appendRow(rowItems);
                }
                m_tableView->setModel(m_duckModel);
            } else {
                QString error = m_duckProcess->readAllStandardError();
                if (!error.isEmpty()) QMessageBox::critical(this, "Erro DuckDB", error);
            }
        }

        void executeRedisQuery(const QString& command) {
            m_duckModel->clear();
            m_duckModel->setHorizontalHeaderLabels({"Output"});

            QProcess redis;
            redis.start("redis-cli", command.split(" ", Qt::SkipEmptyParts));
            if (redis.waitForFinished()) {
                QString out = redis.readAllStandardOutput();
                QString err = redis.readAllStandardError();
                if (!err.isEmpty()) {
                    m_duckModel->appendRow(new QStandardItem("Error: " + err));
                } else {
                    for (const auto& line : out.split("\n")) {
                        if (!line.isEmpty()) m_duckModel->appendRow(new QStandardItem(line));
                    }
                }
                m_tableView->setModel(m_duckModel);
            }
        }

        void executeLevelDBQuery(const QString& command) {
            m_duckModel->clear();
            m_duckModel->setHorizontalHeaderLabels({"Status"});
            m_duckModel->appendRow(new QStandardItem("LevelDB CLI integration via 'leveldbutil' or similar utility."));
            m_duckModel->appendRow(new QStandardItem("Command sent: " + command));
            m_tableView->setModel(m_duckModel);
        }

        void executeRocksDBQuery(const QString& command) {
            m_duckModel->clear();
            m_duckModel->setHorizontalHeaderLabels({"Output"});

            // RocksDB usa o utilitário 'ldb'
            QProcess rocks;
            QStringList args = command.split(" ", Qt::SkipEmptyParts);
            if (!m_currentDbPath.isEmpty()) {
                args.prepend("--db=" + m_currentDbPath);
            }

            rocks.start("ldb", args);
            if (rocks.waitForFinished()) {
                QString out = rocks.readAllStandardOutput();
                QString err = rocks.readAllStandardError();
                if (!err.isEmpty()) {
                    m_duckModel->appendRow(new QStandardItem("Error: " + err));
                } else {
                    for (const auto& line : out.split("\n")) {
                        if (!line.isEmpty()) m_duckModel->appendRow(new QStandardItem(line));
                    }
                }
                m_tableView->setModel(m_duckModel);
            } else {
                m_duckModel->appendRow(new QStandardItem("Erro: 'ldb' (RocksDB tool) não encontrado ou falhou."));
                m_tableView->setModel(m_duckModel);
            }
        }

        void executeBerkeleyDBQuery(const QString& command) {
            m_duckModel->clear();
            m_duckModel->setHorizontalHeaderLabels({"Output"});

            QProcess bdb;
            // BerkeleyDB geralmente usa utilitários db_dump, db_load, db_stat
            QString tool = "db_stat";
            if (command.startsWith("dump")) tool = "db_dump";

            bdb.start(tool, QStringList() << "-d" << m_currentDbPath);
            if (bdb.waitForFinished()) {
                QString out = bdb.readAllStandardOutput();
                for (const auto& line : out.split("\n")) {
                    if (!line.isEmpty()) m_duckModel->appendRow(new QStandardItem(line));
                }
                m_tableView->setModel(m_duckModel);
            }
        }

        void executeMongoQuery(const QString& command) {
            m_duckModel->clear();
            m_duckModel->setHorizontalHeaderLabels({"JSON Output"});

            QProcess mongo;
            QStringList args;
            args << "--eval" << command << "--quiet";

            // Tenta usar mongosh ou mongo
            mongo.start("mongosh", args);
            if (!mongo.waitForStarted()) {
                mongo.start("mongo", args);
            }

            if (mongo.waitForFinished()) {
                QString out = mongo.readAllStandardOutput();
                for (const auto& line : out.split("\n")) {
                    if (!line.isEmpty()) m_duckModel->appendRow(new QStandardItem(line));
                }
                m_tableView->setModel(m_duckModel);
            } else {
                m_duckModel->appendRow(new QStandardItem("Erro: 'mongosh' ou 'mongo' não encontrado."));
                m_tableView->setModel(m_duckModel);
            }
        }

        void showTableContextMenu(const QPoint& pos) {
            QMenu menu(this);
            auto* exportAction = menu.addAction(QIcon::fromTheme("document-save"), "Exportar para CSV");
            auto* importAction = menu.addAction(QIcon::fromTheme("document-import"), "Importar de CSV");

            QAction* selected = menu.exec(m_tableView->mapToGlobal(pos));
            if (selected == exportAction) exportToCSV();
            else if (selected == importAction) importFromCSV();
        }

        void exportToCSV() {
            QAbstractItemModel* model = m_tableView->model();
            if (!model) return;

            QString path = QFileDialog::getSaveFileName(this, "Exportar para CSV", "dados.csv", "CSV Files (*.csv)");
            if (path.isEmpty()) return;

            QFile file(path);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                for (int c = 0; c < model->columnCount(); ++c) {
                    out << model->headerData(c, Qt::Horizontal).toString() << (c < model->columnCount() - 1 ? "," : "");
                }
                out << "\n";
                for (int r = 0; r < model->rowCount(); ++r) {
                    for (int c = 0; c < model->columnCount(); ++c) {
                        out << model->data(model->index(r, c)).toString() << (c < model->columnCount() - 1 ? "," : "");
                    }
                    out << "\n";
                }
                QMessageBox::information(this, "Sucesso", "Dados exportados para CSV!");
            }
        }

        void importFromCSV() {
            QMessageBox::information(this, "Importar", "Para importar, use comandos nativos do banco:\nSQLite: .import file.csv table\nDuckDB: COPY table FROM 'file.csv'");
        }

        void setupCompleter() {
            QStringList keywords = {
                "SELECT", "FROM", "WHERE", "INSERT", "INTO", "UPDATE", "SET", "DELETE",
                "CREATE", "TABLE", "DROP", "ALTER", "JOIN", "LEFT", "RIGHT", "INNER",
                "ON", "GROUP", "BY", "ORDER", "ASC", "DESC", "LIMIT", "OFFSET",
                "AND", "OR", "NOT", "IN", "IS", "NULL", "LIKE", "DISTINCT", "COUNT",
                "SUM", "AVG", "MIN", "MAX", "AS", "VALUES", "INTEGER", "TEXT", "REAL", "BLOB",
                "SET", "GET", "DEL", "EXISTS", "KEYS", "EXPIRE", "HSET", "HGET", "LPUSH", "LPOP",
                "put", "get", "delete", "scan", "compact", "reduce_levels",
                "db.collection.find()", "db.collection.insertOne()", "db.collection.updateMany()",
                "db.collection.deleteOne()", "show dbs", "show collections", "use"
            };

            QCompleter* completer = new QCompleter(keywords, this);
            completer->setCaseSensitivity(Qt::CaseInsensitive);
            completer->setFilterMode(Qt::MatchContains);
            sqlInput->setCompleter(completer);
        }

    private:
        QComboBox* dbTypeCombo;
        QComboBox* tableSelector;
        QLineEdit* sqlInput;
        QTableView* m_tableView;
        QSqlTableModel* m_model;
        QSqlQueryModel* m_queryModel;
        QStandardItemModel* m_duckModel;
        QProcess* m_duckProcess;
        QString m_currentDbPath;
    };
}

#endif

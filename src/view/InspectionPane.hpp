#ifndef INSPECTION_PANE_HPP
#define INSPECTION_PANE_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QHeaderView>
#include <QProcess>
#include <QRegularExpression>
#include <QFileInfo>
#include <QLabel>
#include <QProgressBar>
#include <filesystem>
#include <vector>

namespace Parcel::View {

    class InspectionPane : public QWidget {
        Q_OBJECT
    public:
        explicit InspectionPane(QWidget* parent = nullptr) : QWidget(parent) {
            auto* layout = new QVBoxLayout(this);
            layout->setContentsMargins(0, 0, 0, 0);
            layout->setSpacing(0);

            headerLabel = new QLabel("   Problemas Detectados", this);
            headerLabel->setFixedHeight(30);
            headerLabel->setStyleSheet("background-color: #2b2d30; color: #dfe1e5; font-weight: bold; border-bottom: 1px solid #3c3f41;");
            layout->addWidget(headerLabel);

            progressBar = new QProgressBar(this);
            progressBar->setFixedHeight(3);
            progressBar->setTextVisible(false);
            progressBar->setVisible(false);
            layout->addWidget(progressBar);

            treeWidget = new QTreeWidget(this);
            treeWidget->setColumnCount(4);
            treeWidget->setHeaderLabels({"Gravidade", "Arquivo", "Linha", "Descrição"});
            treeWidget->setAlternatingRowColors(true);
            treeWidget->setStyleSheet("QTreeWidget { background-color: #1e1f22; border: none; color: #bcbec4; } "
                                    "QHeaderView::section { background-color: #2b2d30; color: #aaa; border: none; padding: 4px; }");

            treeWidget->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
            treeWidget->header()->setSectionResizeMode(3, QHeaderView::Stretch);

            layout->addWidget(treeWidget);

            process = new QProcess(this);
            connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &InspectionPane::onFinished);
            connect(treeWidget, &QTreeWidget::itemDoubleClicked, this, &InspectionPane::onItemDoubleClicked);
        }

        ~InspectionPane() {
            if (process && process->state() != QProcess::NotRunning) {
                process->terminate();
                if (!process->waitForFinished(500)) {
                    process->kill();
                }
            }
        }

        void inspectPath(const QString& path) {
            treeWidget->clear();
            filesToInspect.clear();

            std::filesystem::path fsPath(path.toStdString());
            if (std::filesystem::is_directory(fsPath)) {
                for (const auto& entry : std::filesystem::recursive_directory_iterator(fsPath)) {
                    if (entry.is_regular_file()) {
                        std::string ext = entry.path().extension().string();
                        if (ext == ".cpp" || ext == ".c" || ext == ".cc" || ext == ".hpp" || ext == ".h") {
                            filesToInspect.push_back(QString::fromStdString(entry.path().string()));
                        }
                    }
                }
            } else {
                filesToInspect.push_back(path);
            }

            if (filesToInspect.empty()) {
                headerLabel->setText("   Nenhum arquivo C++ encontrado para inspeção.");
                return;
            }

            currentFileIndex = 0;
            progressBar->setVisible(true);
            progressBar->setMaximum(filesToInspect.size());
            progressBar->setValue(0);

            runNextInspection();
        }

    signals:
        void openFileRequested(const std::string& path, int line);

    private slots:
        void onFinished(int exitCode) {
            (void)exitCode;
            QString output = process->readAllStandardError();
            parseOutput(output);

            currentFileIndex++;
            runNextInspection();
        }

        void onItemDoubleClicked(QTreeWidgetItem* item, int column) {
            (void)column;
            std::string path = item->data(1, Qt::UserRole).toString().toStdString();
            int line = item->text(2).toInt();
            emit openFileRequested(path, line);
        }

    private:
        void runNextInspection() {
            if (currentFileIndex >= filesToInspect.size()) {
                progressBar->setVisible(false);
                headerLabel->setText(QString("   Inspeção completa. %1 problema(s) encontrado(s) em %2 arquivo(s).")
                                    .arg(treeWidget->topLevelItemCount()).arg(filesToInspect.size()));
                return;
            }

            QString filePath = filesToInspect[currentFileIndex];
            headerLabel->setText(QString("   Inspecionando (%1/%2): %3")
                                .arg(currentFileIndex + 1).arg(filesToInspect.size()).arg(QFileInfo(filePath).fileName()));

            progressBar->setValue(currentFileIndex);

            QStringList args;
            args << "-fsyntax-only" << "-Wall" << "-Wextra" << "-Wpedantic" << filePath;
            process->start("g++", args);
        }

        void parseOutput(const QString& output) {
            QRegularExpression re("^(.+):(\\d+):(\\d+):\\s+(error|warning|note):\\s+(.+)$", QRegularExpression::MultilineOption);
            auto it = re.globalMatch(output);

            while (it.hasNext()) {
                auto match = it.next();
                QString file = match.captured(1);
                QString line = match.captured(2);
                QString severity = match.captured(4);
                QString message = match.captured(5);

                auto* item = new QTreeWidgetItem(treeWidget);

                if (severity == "error") {
                    item->setIcon(0, QIcon::fromTheme("dialog-error"));
                    item->setForeground(0, QColor("#e74c3c"));
                } else if (severity == "warning") {
                    item->setIcon(0, QIcon::fromTheme("dialog-warning"));
                    item->setForeground(0, QColor("#f1c40f"));
                } else {
                    item->setIcon(0, QIcon::fromTheme("dialog-information"));
                }

                item->setText(0, severity.toUpper());
                item->setText(1, QFileInfo(file).fileName());
                item->setText(2, line);
                item->setText(3, message);

                item->setData(1, Qt::UserRole, file);
            }
        }

        QLabel* headerLabel;
        QProgressBar* progressBar;
        QTreeWidget* treeWidget;
        QProcess* process;

        std::vector<QString> filesToInspect;
        size_t currentFileIndex = 0;
    };
}

#endif

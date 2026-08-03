#ifndef LOGCAT_PANE_HPP
#define LOGCAT_PANE_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QProcess>
#include <QScrollBar>
#include <QLabel>
#include <QComboBox>

namespace Parcel::View {

    class LogcatPane : public QWidget {
        Q_OBJECT
    public:
        explicit LogcatPane(QWidget* parent = nullptr) : QWidget(parent) {
            auto* layout = new QVBoxLayout(this);
            layout->setContentsMargins(0, 0, 0, 0);
            layout->setSpacing(0);

            // Toolbar
            auto* toolbar = new QWidget(this);
            toolbar->setFixedHeight(35);
            toolbar->setStyleSheet("background-color: #2b2d30; border-bottom: 1px solid #3c3f41;");
            auto* toolbarLayout = new QHBoxLayout(toolbar);
            toolbarLayout->setContentsMargins(10, 0, 10, 0);

            logLevelCombo = new QComboBox(this);
            logLevelCombo->addItems({"All Logs", "Errors Only", "Warnings", "Kernel"});
            logLevelCombo->setStyleSheet("background-color: #1e1f22; color: #bcbec4; font-size: 11px;");

            filterInput = new QLineEdit(this);
            filterInput->setPlaceholderText("Filtrar logs...");
            filterInput->setStyleSheet("background-color: #1e1f22; color: #bcbec4; border: 1px solid #3c3f41; padding: 2px 8px; border-radius: 4px;");

            auto* clearBtn = new QPushButton("Limpar", this);
            clearBtn->setIcon(QIcon::fromTheme("edit-clear"));

            toolbarLayout->addWidget(new QLabel("Filtro:", this));
            toolbarLayout->addWidget(logLevelCombo);
            toolbarLayout->addWidget(filterInput, 1);
            toolbarLayout->addWidget(clearBtn);
            layout->addWidget(toolbar);

            // Log Area
            logArea = new QTextEdit(this);
            logArea->setReadOnly(true);
            logArea->setStyleSheet("QTextEdit { background-color: #0c0c0c; color: #aaaaaa; font-family: 'Monospace'; font-size: 12px; border: none; }");
            layout->addWidget(logArea);

            process = new QProcess(this);
            connect(process, &QProcess::readyReadStandardOutput, this, &LogcatPane::readOutput);
            connect(clearBtn, &QPushButton::clicked, logArea, &QTextEdit::clear);
            connect(filterInput, &QLineEdit::textChanged, this, &LogcatPane::restartLogProcess);
            connect(logLevelCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &LogcatPane::restartLogProcess);

            startLogProcess();
        }

        ~LogcatPane() {
            if (process->state() != QProcess::NotRunning) {
                process->terminate();
                process->waitForFinished(500);
            }
        }

    private slots:
        void startLogProcess() {
            if (process->state() != QProcess::NotRunning) process->terminate();

            QStringList args;
            args << "-f" << "-n" << "100" << "--no-hostname";

            QString level = logLevelCombo->currentText();
            if (level == "Errors Only") args << "-p" << "err";
            else if (level == "Warnings") args << "-p" << "warning";
            else if (level == "Kernel") args << "-k";

            process->start("journalctl", args);
        }

        void restartLogProcess() {
            startLogProcess();
        }

        void readOutput() {
            QString output = process->readAllStandardOutput();
            QString filter = filterInput->text().trimmed();

            for (const QString& line : output.split('\n')) {
                if (line.isEmpty()) continue;
                if (!filter.isEmpty() && !line.contains(filter, Qt::CaseInsensitive)) continue;

                // Colorização básica
                QString formattedLine = line;
                if (line.contains("error", Qt::CaseInsensitive) || line.contains("fail", Qt::CaseInsensitive)) {
                    formattedLine = "<span style='color: #e74c3c;'>" + line + "</span>";
                } else if (line.contains("warn", Qt::CaseInsensitive)) {
                    formattedLine = "<span style='color: #f1c40f;'>" + line + "</span>";
                }

                logArea->append(formattedLine);
            }
            logArea->verticalScrollBar()->setValue(logArea->verticalScrollBar()->maximum());
        }

    private:
        QTextEdit* logArea;
        QLineEdit* filterInput;
        QComboBox* logLevelCombo;
        QProcess* process;
    };
}

#endif

#ifndef GIT_PANE_HPP
#define GIT_PANE_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QProcess>
#include <QIcon>
#include <QInputDialog>
#include <QMessageBox>

namespace Parcel::View {

    class GitPane : public QWidget {
        Q_OBJECT
    public:
        explicit GitPane(QWidget* parent = nullptr) : QWidget(parent) {
            auto* layout = new QVBoxLayout(this);
            layout->setContentsMargins(10, 10, 10, 10);
            layout->setSpacing(10);

            // Header - Branch Info
            auto* branchLayout = new QHBoxLayout();
            branchLabel = new QLabel("🌿 Branch: <b>unknown</b>", this);
            branchLabel->setStyleSheet("font-size: 14px; color: #4285F4;");

            auto* btnRefresh = new QPushButton(QIcon::fromTheme("view-refresh"), "", this);
            btnRefresh->setFixedSize(30, 30);

            branchLayout->addWidget(branchLabel);
            branchLayout->addStretch();
            branchLayout->addWidget(btnRefresh);
            layout->addLayout(branchLayout);

            // Commit History
            layout->addWidget(new QLabel("📜 Commit History:", this));
            commitList = new QListWidget(this);
            commitList->setStyleSheet("QListWidget { background-color: #1e1f22; border: 1px solid #3c3f41; color: #bcbec4; } "
                                    "QListWidget::item { padding: 5px; border-bottom: 1px solid #2b2d30; }");
            layout->addWidget(commitList, 1);

            // Actions
            auto* actionsLayout = new QHBoxLayout();
            auto* btnCommit = new QPushButton(QIcon::fromTheme("document-save"), " Commit", this);
            auto* btnPush = new QPushButton(QIcon::fromTheme("network-server"), " Push", this);
            auto* btnPull = new QPushButton(QIcon::fromTheme("view-refresh"), " Pull", this);

            QString btnStyle = "QPushButton { background: #2b2d30; border: 1px solid #3c3f41; color: #bcbec4; padding: 6px; } "
                               "QPushButton:hover { background: #393b40; }";
            btnCommit->setStyleSheet(btnStyle);
            btnPush->setStyleSheet(btnStyle);
            btnPull->setStyleSheet(btnStyle);

            actionsLayout->addWidget(btnCommit);
            actionsLayout->addWidget(btnPush);
            actionsLayout->addWidget(btnPull);
            layout->addLayout(actionsLayout);

            connect(btnRefresh, &QPushButton::clicked, this, &GitPane::refresh);
            connect(btnCommit, &QPushButton::clicked, this, &GitPane::onCommitClicked);
            connect(btnPush, &QPushButton::clicked, this, &GitPane::onPushClicked);
            connect(btnPull, &QPushButton::clicked, this, &GitPane::onPullClicked);
        }

        void setProjectPath(const QString& path) {
            projectPath = path;
            refresh();
        }

        void refresh() {
            if (projectPath.isEmpty()) return;

            // Get Current Branch
            QProcess branchProc;
            branchProc.setWorkingDirectory(projectPath);
            branchProc.start("git", {"rev-parse", "--abbrev-ref", "HEAD"});
            if (branchProc.waitForFinished() && branchProc.exitCode() == 0) {
                QString branch = QString::fromUtf8(branchProc.readAllStandardOutput()).trimmed();
                branchLabel->setText("🌿 Branch: <b>" + branch + "</b>");
            } else {
                branchLabel->setText("🌿 Branch: <b>Not a git repository</b>");
            }

            // Get History
            commitList->clear();
            QProcess logProc;
            logProc.setWorkingDirectory(projectPath);
            logProc.start("git", {"log", "--oneline", "-n", "20"});
            if (logProc.waitForFinished() && logProc.exitCode() == 0) {
                QStringList lines = QString::fromUtf8(logProc.readAllStandardOutput()).split("\n", Qt::SkipEmptyParts);
                for (const auto& line : lines) {
                    commitList->addItem(line);
                }
            }
        }

    private slots:
        void onCommitClicked() {
            bool ok;
            QString message = QInputDialog::getText(this, "Commit Changes", "Message:", QLineEdit::Normal, "WIP", &ok);
            if (ok && !message.isEmpty()) {
                QProcess addProc;
                addProc.setWorkingDirectory(projectPath);
                addProc.start("git", {"add", "."});
                addProc.waitForFinished();

                QProcess commitProc;
                commitProc.setWorkingDirectory(projectPath);
                commitProc.start("git", {"commit", "-m", message});
                if (commitProc.waitForFinished() && commitProc.exitCode() == 0) {
                    QMessageBox::information(this, "Git", "Changes committed successfully.");
                    refresh();
                } else {
                    QMessageBox::critical(this, "Git", "Commit failed:\n" + commitProc.readAllStandardError());
                }
            }
        }

        void onPushClicked() {
            QProcess* pushProc = new QProcess(this);
            pushProc->setWorkingDirectory(projectPath);
            pushProc->start("git", {"push"});
            connect(pushProc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [this, pushProc](int code) {
                if (code == 0) QMessageBox::information(this, "Git", "Push successful.");
                else QMessageBox::warning(this, "Git", "Push failed (check terminal for details).");
                pushProc->deleteLater();
                refresh();
            });
        }

        void onPullClicked() {
            QProcess* pullProc = new QProcess(this);
            pullProc->setWorkingDirectory(projectPath);
            pullProc->start("git", {"pull"});
            connect(pullProc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [this, pullProc](int code) {
                if (code == 0) QMessageBox::information(this, "Git", "Pull successful.");
                else QMessageBox::warning(this, "Git", "Pull failed.");
                pullProc->deleteLater();
                refresh();
            });
        }

    private:
        QString projectPath;
        QLabel* branchLabel;
        QListWidget* commitList;
    };
}

#endif

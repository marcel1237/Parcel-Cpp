#ifndef BROWSER_PANE_HPP
#define BROWSER_PANE_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QUrl>
#include <QWebEngineView>
#include <QMenu>
#include <QProcess>
#include <QMessageBox>

namespace Parcel::View {

    class BrowserPane : public QWidget {
        Q_OBJECT
    public:
        explicit BrowserPane(const QString& initialUrl = "https://www.google.com", QWidget* parent = nullptr) : QWidget(parent) {
            auto* layout = new QVBoxLayout(this);
            layout->setContentsMargins(6, 6, 6, 6);
            layout->setSpacing(4);
            setStyleSheet("background-color: #1e1e1e; border-top: 1px solid #333;");

            // Top bar (URL)
            auto* top = new QHBoxLayout();
            m_urlBar = new QLineEdit(this);
            m_urlBar->setPlaceholderText("Digite uma URL e pressione Enter");
            m_urlBar->setStyleSheet("background-color: #2b2d30; color: #bcbec4; border: 1px solid #4e5157; border-radius: 4px; padding: 4px;");
            top->addWidget(m_urlBar);

            layout->addLayout(top);

            // Web view dentro do painel
            m_view = new QWebEngineView(this);
            m_view->setContextMenuPolicy(Qt::CustomContextMenu);
            layout->addWidget(m_view, 1);

            // Carregar ao apertar Enter
            connect(m_urlBar, &QLineEdit::returnPressed, this, [this]() {
                loadUrl(m_urlBar->text());
            });

            // Menu de contexto personalizado
            connect(m_view, &QWebEngineView::customContextMenuRequested, this, &BrowserPane::showContextMenu);

            // Atualiza barra quando a página mudar
            connect(m_view, &QWebEngineView::urlChanged, this, [this](const QUrl& url) {
                m_urlBar->setText(url.toString());
            });

            // Página inicial
            m_view->load(QUrl(initialUrl));
        }

        void loadUrl(const QString& input) {
            QUrl url = QUrl::fromUserInput(input);
            if (!url.isValid()) return;
            m_view->load(url);
        }

        void setProjectPath(const QString& path) {
            m_projectPath = path;
        }

    signals:
        void scriptStarted(const QString& command);
        void scriptOutput(const QString& text);
        void scriptError(const QString& text);
        void scriptFinished(int exitCode);

    private slots:
        void showContextMenu(const QPoint& pos) {
            QMenu menu(this);

            QString selectedText = m_view->selectedText().trimmed();
            if (!selectedText.isEmpty()) {
                QAction* runAction = menu.addAction(QIcon::fromTheme("system-run"), "Executar Script Shell");
                connect(runAction, &QAction::triggered, this, [this, selectedText]() {
                    executeShellScript(selectedText);
                });
            }

            menu.exec(m_view->mapToGlobal(pos));
        }

    private:
        QLineEdit* m_urlBar;
        QWebEngineView* m_view;
        QString m_projectPath;

        void executeShellScript(const QString& script) {
            QProcess* process = new QProcess(this);
            QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
            process->setProcessEnvironment(env);

            if (!m_projectPath.isEmpty()) {
                process->setWorkingDirectory(m_projectPath);
            }

            emit scriptStarted(script);

            connect(process, &QProcess::readyReadStandardOutput, this, [this, process]() {
                emit scriptOutput(process->readAllStandardOutput());
            });

            connect(process, &QProcess::readyReadStandardError, this, [this, process]() {
                emit scriptError(process->readAllStandardError());
            });

            connect(process, &QProcess::finished, this, [this, process](int exitCode) {
                emit scriptFinished(exitCode);
                process->deleteLater();
            });

            process->start("bash", QStringList() << "-c" << script);
        }
    };
}

#endif

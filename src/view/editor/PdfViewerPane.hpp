#ifndef PDF_VIEWER_PANE_HPP
#define PDF_VIEWER_PANE_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QWebEngineView>
#include <QWebEngineSettings>
#include <QUrl>
#include <QFileInfo>

namespace Parcel::View {

    class PdfViewerPane : public QWidget {
        Q_OBJECT
    public:
        explicit PdfViewerPane(const QString& filePath, QWidget* parent = nullptr)
            : QWidget(parent), m_filePath(filePath) {

            auto* layout = new QVBoxLayout(this);
            layout->setContentsMargins(0, 0, 0, 0);

            m_view = new QWebEngineView(this);

            // Habilita plugins (necessário para o visualizador de PDF interno do Chromium)
            m_view->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
            m_view->settings()->setAttribute(QWebEngineSettings::PdfViewerEnabled, true);

            layout->addWidget(m_view);

            loadPdf();
        }

        void loadPdf() {
            // No Qt WebEngine, arquivos locais devem usar o prefixo file://
            m_view->load(QUrl::fromLocalFile(m_filePath));
        }

        QString getFilePath() const { return m_filePath; }

    private:
        QWebEngineView* m_view;
        QString m_filePath;
    };
}

#endif

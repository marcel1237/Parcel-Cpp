#ifndef PDF_SERVICE_HPP
#define PDF_SERVICE_HPP

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <QString>
#include <QPdfWriter>
#include <QPainter>
#include <QTextDocument>
#include <QDateTime>
#include <QCoreApplication>
#include <QDir>

namespace Parcel::Service {

    class PdfService {
    public:
        void updateProjectPdf(const std::string& projectPath) {
            if (projectPath.empty() || !std::filesystem::exists(projectPath)) return;

            QString outputFilePath = QString::fromStdString(projectPath) + "/Project_Knowledge_Base.pdf";
            QString fullContent = "<h1>Project Knowledge Base</h1>";
            fullContent += "<p>Generated on: " + QDateTime::currentDateTime().toString() + "</p><hr>";

            std::vector<std::string> textExtensions = {
                ".java", ".cpp", ".hpp", ".h", ".c", ".cc", ".xml", ".fxml",
                ".json", ".md", ".txt", ".sh", ".bash", ".css", ".pro",
                ".cmake", ".desktop"
            };

            try {
                int fileCount = 0;
                for (const auto& entry : std::filesystem::recursive_directory_iterator(projectPath)) {
                    if (fileCount > 200) break; // Limite de segurança para evitar PDFs gigantes

                    if (entry.is_regular_file()) {
                        std::string path = entry.path().string();
                        std::string fileName = entry.path().filename().string();
                        std::string ext = entry.path().extension().string();

                        if (path.find("/build/") != std::string::npos ||
                            path.find("/.git/") != std::string::npos ||
                            path.find("/.idea/") != std::string::npos ||
                            fileName == "Project_Knowledge_Base.pdf")
                            continue;

                        bool isText = false;
                        for (const auto& tExt : textExtensions) {
                            if (ext == tExt || fileName == "CMakeLists.txt" || fileName == "Makefile") {
                                isText = true;
                                break;
                            }
                        }

                        if (isText) {
                            std::ifstream file(path);
                            if (file.is_open()) {
                                std::stringstream buffer;
                                buffer << file.rdbuf();
                                QString content = QString::fromStdString(buffer.str());
                                if (content.length() > 50000) content = content.left(50000) + "\n... [TRUNCATED]";

                                fullContent += "<h2>File: " + QString::fromStdString(fileName) + "</h2>";
                                fullContent += "<pre style='font-size: 8px;'>" + content.toHtmlEscaped() + "</pre><hr>";
                                fileCount++;
                            }
                        }
                    }
                }

                QTextDocument doc;
                doc.setHtml(fullContent);
                QPdfWriter writer(outputFilePath);
                writer.setPageSize(QPageSize(QPageSize::A4));
                doc.print(&writer);
            } catch (...) {}
        }

        void generateDatabaseReport(const QString& outputPath, const QString& title, const QString& htmlTable, const QPixmap& chartPixmap) {
            QPdfWriter writer(outputPath);
            writer.setPageSize(QPageSize(QPageSize::A4));
            writer.setPageMargins(QMarginsF(30, 30, 30, 30));

            QPainter painter(&writer);

            // 1. Logotipo no Topo
            QDir appDir(QCoreApplication::applicationDirPath());
            if (appDir.dirName() == "build") appDir.cdUp();
            QString logoPath = appDir.absolutePath() + "/resource/parcel-cpp-dark.png";

            QPixmap logo(logoPath);
            if (!logo.isNull()) {
                double logoSize = 800; // Tamanho no PDF
                painter.drawPixmap(QRectF(100, 100, logoSize, logoSize), logo, QRectF(logo.rect()));
            }

            // Estilo do Título
            painter.setFont(QFont("Sans Serif", 16, QFont::Bold));
            painter.drawText(QRectF(0, 0, writer.width(), 1000), Qt::AlignCenter, title);

            painter.setFont(QFont("Sans Serif", 8));
            painter.drawText(QRectF(0, 1000, writer.width(), 500), Qt::AlignRight, "Gerado por Parcel C++ em " + QDateTime::currentDateTime().toString());

            // Desenhar Gráfico se houver
            if (!chartPixmap.isNull()) {
                double targetWidth = writer.width() * 0.8;
                double factor = targetWidth / chartPixmap.width();
                double targetHeight = chartPixmap.height() * factor;

                painter.drawPixmap(QRectF((writer.width() - targetWidth)/2, 2000, targetWidth, targetHeight), chartPixmap, QRectF(chartPixmap.rect()));
            }

            // Tabela de Dados (via QTextDocument para renderizar HTML)
            QTextDocument doc;
            doc.setHtml("<style>table { border-collapse: collapse; width: 100%; } th, td { border: 1px solid black; padding: 5px; text-align: left; font-size: 10pt; }</style>" + htmlTable);
            doc.setPageSize(QPageSize(QPageSize::A4).size(QPageSize::Point));

            painter.translate(0, 7000); // Move para baixo do gráfico
            doc.drawContents(&painter);

            painter.end();
        }
    };
}

#endif

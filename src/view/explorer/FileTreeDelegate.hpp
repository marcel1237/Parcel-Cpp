#ifndef FILE_TREE_DELEGATE_HPP
#define FILE_TREE_DELEGATE_HPP

#include <QStyledItemDelegate>
#include <QPainter>
#include <QFileInfo>

namespace Parcel::View {

    class FileTreeDelegate : public QStyledItemDelegate {
    public:
        using QStyledItemDelegate::QStyledItemDelegate;

        void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const override {
            QStyledItemDelegate::initStyleOption(option, index);

            QString fileName = index.data().toString().toLower();

            // Aplicar cores baseadas na extensão (Realce de Sintaxe no Explorer)
            if (fileName.endsWith(".java")) {
                option->palette.setColor(QPalette::Text, QColor("#e67e22")); // Laranja Java
            } else if (fileName.endsWith(".cpp") || fileName.endsWith(".hpp") || fileName.endsWith(".h")) {
                option->palette.setColor(QPalette::Text, QColor("#3498db")); // Azul C++
            } else if (fileName.endsWith(".xml") || fileName.endsWith(".fxml")) {
                option->palette.setColor(QPalette::Text, QColor("#e74c3c")); // Vermelho XML
            } else if (fileName.endsWith(".md")) {
                option->palette.setColor(QPalette::Text, QColor("#2ecc71")); // Verde Markdown
            } else if (fileName.endsWith(".sh") || fileName.endsWith(".bash")) {
                option->palette.setColor(QPalette::Text, QColor("#f1c40f")); // Amarelo Script
            } else if (fileName.endsWith(".json")) {
                option->palette.setColor(QPalette::Text, QColor("#9b59b6")); // Roxo JSON
            } else if (fileName.startsWith(".")) {
                option->palette.setColor(QPalette::Text, QColor("#7f8c8d")); // Cinza para arquivos ocultos/config
            }

            // Pastas específicas
            if (fileName == "src") {
                option->font.setBold(true);
            }
        }
    };
}

#endif

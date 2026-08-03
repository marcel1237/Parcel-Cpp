#ifndef DESIGNER_PANE_HPP
#define DESIGNER_PANE_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QQuickWidget>
#include <QQmlContext>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QSplitter>
#include <QDir>
#include <QFile>
#include <QAbstractListModel>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QShortcut>
#include <QFileDialog>
#include <QMessageBox>
#include <QCheckBox>
#include <QMap>
#include <QTabWidget>
#include <QMenu>
#include <QToolButton>

namespace Parcel::View {

    struct DesignerElement {
        QString id;
        QString type;
        int x = 50;
        int y = 50;
        int width = 160;
        int height = 40;
        QString text = "Componente";
        QString color = "steelblue";
        QString source = "";
        double value = 0.5;
        int radius = 0;
        QString placeholder = "";
        QString items = "Opção 1,Opção 2,Opção 3";
        bool checked = false;
        int columns = 2;
        int fontSize = 14;
        int borderWidth = 0;
        QString borderColor = "#000000";
        QString fillMode = "PreserveAspectFit";
    };

    class DesignerModel : public QAbstractListModel {
        Q_OBJECT
    public:
        enum Roles {
            TypeRole = Qt::UserRole + 1, XRole, YRole, WidthRole, HeightRole,
            TextRole, IdRole, ColorRole, SourceRole, ValueRole, RadiusRole,
            PlaceholderRole, ItemsRole, CheckedRole, ColumnsRole, FontSizeRole,
            BorderWidthRole, BorderColorRole, FillModeRole
        };

        explicit DesignerModel(QObject* parent = nullptr) : QAbstractListModel(parent) {}

        int rowCount(const QModelIndex& parent = QModelIndex()) const override {
            Q_UNUSED(parent);
            return m_elements.size();
        }

        QVariant data(const QModelIndex& index, int role) const override {
            if (!index.isValid() || index.row() >= m_elements.size()) return QVariant();
            const auto& e = m_elements[index.row()];
            switch (role) {
                case TypeRole: return e.type;
                case XRole: return e.x;
                case YRole: return e.y;
                case WidthRole: return e.width;
                case HeightRole: return e.height;
                case TextRole: return e.text;
                case IdRole: return e.id;
                case ColorRole: return e.color;
                case SourceRole: return e.source;
                case ValueRole: return e.value;
                case RadiusRole: return e.radius;
                case PlaceholderRole: return e.placeholder;
                case ItemsRole: return e.items;
                case CheckedRole: return e.checked;
                case ColumnsRole: return e.columns;
                case FontSizeRole: return e.fontSize;
                case BorderWidthRole: return e.borderWidth;
                case BorderColorRole: return e.borderColor;
                case FillModeRole: return e.fillMode;
            }
            return QVariant();
        }

        QHash<int, QByteArray> roleNames() const override {
            return {
                {TypeRole, "type"}, {XRole, "posX"}, {YRole, "posY"}, {WidthRole, "elementWidth"},
                {HeightRole, "elementHeight"}, {TextRole, "text"}, {IdRole, "elementId"},
                {ColorRole, "color"}, {SourceRole, "source"}, {ValueRole, "value"},
                {RadiusRole, "radius"}, {PlaceholderRole, "placeholder"},
                {ItemsRole, "items"}, {CheckedRole, "checked"}, {ColumnsRole, "columns"},
                {FontSizeRole, "fontSize"}, {BorderWidthRole, "borderWidth"}, {BorderColorRole, "borderColor"},
                {FillModeRole, "fillMode"}
            };
        }

        Q_INVOKABLE void setElementPosition(int index, int x, int y) {
            if (index >= 0 && index < m_elements.size()) {
                if (m_elements[index].x == x && m_elements[index].y == y) return;
                saveState();
                m_elements[index].x = x;
                m_elements[index].y = y;
                emit dataChanged(this->index(index), this->index(index), {XRole, YRole});
                emit modelUpdated();
            }
        }

        Q_INVOKABLE void setElementSize(int index, int w, int h) {
            if (index >= 0 && index < m_elements.size()) {
                if (m_elements[index].width == w && m_elements[index].height == h) return;
                saveState();
                m_elements[index].width = w;
                m_elements[index].height = h;
                emit dataChanged(this->index(index), this->index(index), {WidthRole, HeightRole});
                emit modelUpdated();
            }
        }

        Q_INVOKABLE void selectElement(int index) { emit elementSelected(index); }

        Q_INVOKABLE void removeElement(int index) {
            if (index >= 0 && index < m_elements.size()) {
                saveState();
                beginRemoveRows(QModelIndex(), index, index);
                m_elements.removeAt(index);
                endRemoveRows();
                emit modelUpdated();
            }
        }

        Q_INVOKABLE void undo() {
            if (m_undoStack.isEmpty()) return;
            m_redoStack.append(m_elements);
            beginResetModel();
            m_elements = m_undoStack.takeLast();
            endResetModel();
            emit modelUpdated();
        }

        Q_INVOKABLE void redo() {
            if (m_redoStack.isEmpty()) return;
            m_undoStack.append(m_elements);
            beginResetModel();
            m_elements = m_redoStack.takeLast();
            endResetModel();
            emit modelUpdated();
        }

        Q_INVOKABLE void copyElement(int index) {
            if (index >= 0 && index < m_elements.size()) {
                m_clipboard = m_elements[index];
            }
        }

        Q_INVOKABLE void pasteElement(int mouseX = -1, int mouseY = -1) {
            if (m_clipboard.type.isEmpty()) return;
            saveState();
            beginInsertRows(QModelIndex(), m_elements.size(), m_elements.size());
            DesignerElement newElem = m_clipboard;
            newElem.id = newElem.type.toLower().replace(".", "_") + "_copy_" + QString::number(m_elements.size());
            if (mouseX != -1 && mouseY != -1) {
                newElem.x = mouseX;
                newElem.y = mouseY;
            } else {
                newElem.x += 20;
                newElem.y += 20;
            }
            m_elements.append(newElem);
            endInsertRows();
            emit modelUpdated();
        }

        void addElement(const QString& type) {
            saveState();
            beginInsertRows(QModelIndex(), m_elements.size(), m_elements.size());
            QString id = type.toLower().replace(".", "_") + "_" + QString::number(m_elements.size());

            DesignerElement e;
            e.id = id; e.type = type; e.x = 100; e.y = 100; e.width = 160; e.height = 40; e.text = "Novo " + type;

            if (type.contains("Card")) { e.width = 300; e.height = 150; }
            else if (type == "Rectangle") { e.width = 100; e.height = 100; e.color = "steelblue"; e.radius = 8; }
            else if (type == "Image") { e.width = 100; e.height = 100; e.source = "https://picsum.photos/200"; }
            else if (type.contains("TextArea") || type == "ScrollView") { e.width = 200; e.height = 100; e.placeholder = "Digite aqui..."; }
            else if (type == "TextField") { e.placeholder = "Placeholder..."; }
            else if (type == "ProgressBar" || type == "Slider" || type == "Dial" || type == "SpinBox") { e.width = 200; e.height = 20; e.value = 0.5; if (type == "Dial" || type == "SpinBox") { e.width = 100; e.height = 100; } }
            else if (type == "Kirigami.InlineMessage") { e.width = 350; e.height = 80; }
            else if (type == "Kirigami.FormLayout") { e.width = 300; e.height = 200; }
            else if (type == "Kirigami.Icon") { e.width = 32; e.height = 32; e.source = "document-save"; }
            else if (type == "Kirigami.Separator") { e.width = 200; e.height = 2; }

            m_elements.append(e);
            endInsertRows();
            emit modelUpdated();
        }

        void updateElementProperty(int index, const QString& prop, const QVariant& value) {
            if (index >= 0 && index < m_elements.size()) {
                saveState();
                if (prop == "text") m_elements[index].text = value.toString();
                else if (prop == "color") m_elements[index].color = value.toString();
                else if (prop == "source") m_elements[index].source = value.toString();
                else if (prop == "value") m_elements[index].value = value.toDouble();
                else if (prop == "radius") m_elements[index].radius = value.toInt();
                else if (prop == "placeholder") m_elements[index].placeholder = value.toString();
                else if (prop == "items") m_elements[index].items = value.toString();
                else if (prop == "checked") m_elements[index].checked = value.toBool();
                else if (prop == "columns") m_elements[index].columns = value.toInt();
                else if (prop == "fontSize") m_elements[index].fontSize = value.toInt();
                else if (prop == "borderWidth") m_elements[index].borderWidth = value.toInt();
                else if (prop == "borderColor") m_elements[index].borderColor = value.toString();
                else if (prop == "fillMode") m_elements[index].fillMode = value.toString();
                else if (prop == "width") m_elements[index].width = value.toInt();
                else if (prop == "height") m_elements[index].height = value.toInt();
                else if (prop == "x") m_elements[index].x = value.toInt();
                else if (prop == "y") m_elements[index].y = value.toInt();

                emit dataChanged(this->index(index), this->index(index));
                emit modelUpdated();
            }
        }

        DesignerElement getElement(int index) const { return (index >= 0 && index < m_elements.size()) ? m_elements[index] : DesignerElement(); }

        QString generateQmlCode() const {
            QString qml = "import QtQuick\nimport QtQuick.Controls\nimport org.kde.kirigami as Kirigami\n\nApplicationWindow {\n    width: 800; height: 600; visible: true\n\n";
            for (const auto& e : m_elements) {
                qml += QString("    %1 {\n        id: %2; x: %3; y: %4; width: %5; height: %6\n").arg(e.type, e.id).arg(e.x).arg(e.y).arg(e.width).arg(e.height);
                if (e.type.contains("Button") || e.type.contains("Heading") || e.type.contains("Label") || e.type.contains("Text") || e.type == "CheckBox" || e.type == "Switch" || e.type == "Kirigami.UrlButton" || e.type == "Kirigami.PlaceholderMessage" || e.type == "Kirigami.InlineMessage" || e.type == "Kirigami.Action" || e.type == "Kirigami.AbstractApplicationHeader") {
                    qml += QString("        text: \"%1\"\n").arg(e.text);
                }
                if (e.type == "GroupBox") { qml += QString("        title: \"%1\"\n").arg(e.text); }
                if (e.type == "Rectangle") { qml += QString("        color: \"%1\"; radius: %2; border.width: %3; border.color: \"%4\"\n").arg(e.color).arg(e.radius).arg(e.borderWidth).arg(e.borderColor); }
                else if (e.type == "Text") { qml += QString("        color: \"%1\"; font.pixelSize: %2\n").arg(e.color).arg(e.fontSize); }
                else if (e.type == "Label") { qml += QString("        font.pixelSize: %1\n").arg(e.fontSize); }
                else if (e.type == "Image") { qml += QString("        source: \"%1\"; fillMode: Image.%2\n").arg(e.source, e.fillMode); }
                else if (e.type == "Kirigami.Icon") { qml += QString("        source: \"%1\"; color: \"%2\"\n").arg(e.source, e.color); }
                else if (e.type == "Kirigami.Action") { qml += QString("        icon.name: \"%1\"; checked: %2\n").arg(e.source).arg(e.checked ? "true" : "false"); }
                else if (e.type == "Kirigami.UrlButton") { qml += QString("        url: \"%1\"\n").arg(e.source); }
                else if (e.type == "ProgressBar" || e.type == "Slider" || e.type == "Dial" || e.type == "SpinBox") { qml += QString("        value: %1\n").arg(e.value * (e.type == "SpinBox" ? 100 : 1)); }
                else if (e.type == "TextField" || e.type == "TextArea") { qml += QString("        placeholderText: \"%1\"\n").arg(e.placeholder); }
                else if (e.type == "ComboBox" || e.type == "TabBar" || e.type == "ToolBar") {
                    if (e.type == "ToolBar" || e.type == "TabBar") {
                        QString childType = (e.type == "ToolBar") ? "ToolButton" : "TabButton";
                        qml += QString("        Row {\n            anchors.fill: parent\n            Repeater {\n                model: [%1]\n                %2 { text: modelData }\n            }\n        }\n").arg(e.items.split(",").join("\", \"").prepend("\"").append("\""), childType);
                    } else { qml += QString("        model: [%1]\n").arg(e.items.split(",").join("\", \"").prepend("\"").append("\"")); }
                } else if (e.type == "CheckBox" || e.type == "Switch" || e.type == "RadioButton") { qml += QString("        checked: %1\n").arg(e.checked ? "true" : "false"); }
                else if (e.type == "BusyIndicator") { qml += QString("        running: %1\n").arg(e.checked ? "true" : "false"); }
                else if (e.type == "Row" || e.type == "Column" || e.type == "Grid" || e.type == "Flow") { qml += QString("        spacing: %1\n").arg(e.value); if (e.type == "Grid") qml += QString("        columns: %1\n").arg(e.columns); }
                else if (e.type == "Kirigami.PlaceholderMessage") { qml += QString("        icon.name: \"%1\"\n").arg(e.source); }
                else if (e.type == "Kirigami.Card") { qml += QString("        header: Kirigami.Heading { text: \"%1\" }\n").arg(e.text); }
                else if (e.type == "Kirigami.Separator") { /* Basic separator */ }
                else if (e.type == "Kirigami.FormLayout") {
                    qml += QString("        wideMode: %1\n").arg(e.checked ? "true" : "false");
                    for (const QString& label : e.items.split(",")) { if (!label.trimmed().isEmpty()) { qml += QString("        TextField { Kirigami.FormData.label: \"%1\" }\n").arg(label.trimmed()); } }
                } else if (e.type == "ScrollView") {
                    qml += QString("        Column {\n            spacing: 5; width: parent.width\n            Repeater {\n                model: [%1]\n                Label { text: modelData; width: parent.width }\n            }\n        }\n").arg(e.items.split(",").join("\", \"").prepend("\"").append("\""));
                }
                qml += "    }\n\n";
            }
            qml += "}\n";
            return qml;
        }

    signals:
        void modelUpdated();
        void elementSelected(int index);

    private:
        void saveState() { m_undoStack.append(m_elements); if (m_undoStack.size() > 50) m_undoStack.removeFirst(); m_redoStack.clear(); }
        QList<DesignerElement> m_elements;
        QList<QList<DesignerElement>> m_undoStack;
        QList<QList<DesignerElement>> m_redoStack;
        DesignerElement m_clipboard;
    };

    class BlueprintPane : public QQuickWidget {
        Q_OBJECT
    public:
        explicit BlueprintPane(QWidget* parent = nullptr) : QQuickWidget(parent) {
            setResizeMode(QQuickWidget::SizeRootObjectToView);
            QString qml =
                "import QtQuick\nimport QtQuick.Shapes\n"
                "Rectangle {\n"
                "    id: root; color: '#14141f'; anchors.fill: parent\n"
                "    Rectangle {\n"
                "        anchors.fill: parent\n"
                "        gradient: Gradient {\n"
                "            GradientStop { position: 0.0; color: '#1a1a2e' }\n"
                "            GradientStop { position: 1.0; color: '#0f0f1a' }\n"
                "        }\n"
                "    }\n"
                "    Canvas {\n"
                "        anchors.fill: parent\n"
                "        onPaint: {\n"
                "            var ctx = getContext('2d'); ctx.strokeStyle = '#1e1e30'; ctx.lineWidth = 1\n"
                "            for (var x = 0; x < width; x += 40) { ctx.beginPath(); ctx.moveTo(x, 0); ctx.lineTo(x, height); ctx.stroke() }\n"
                "            for (var y = 0; y < height; y += 40) { ctx.beginPath(); ctx.moveTo(0, y); ctx.lineTo(width, y); ctx.stroke() }\n"
                "        }\n"
                "    }\n"
                "    Text { text: 'PARCEL BLUEPRINT ENGINE v2.0 - NEON GLASS'; color: '#00BFFF'; font.pixelSize: 10; anchors { right: parent.right; bottom: parent.bottom; margins: 20 } opacity: 0.6 }\n"
                "    Rectangle {\n"
                "        id: node1; x: 150; y: 150; width: 220; height: 160; color: '#D91a1a2e'; radius: 12; border.color: '#00BFFF'; border.width: 1\n"
                "        Rectangle {\n"
                "            width: parent.width; height: 35; color: '#4000BFFF'; radius: 12\n"
                "            Text { text: 'Logic Processor'; color: 'white'; anchors.centerIn: parent; font.bold: true; font.pixelSize: 12 }\n"
                "            Rectangle { height: 1; width: parent.width; color: '#8000BFFF'; anchors.bottom: parent.bottom }\n"
                "        }\n"
                "        Column { anchors { top: parent.top; topMargin: 50; left: parent.left; leftMargin: 20 } spacing: 15\n"
                "            Row { spacing: 12; Rectangle { width: 12; height: 12; radius: 6; color: '#00FF7F'; border.color: 'white'; border.width: 1 } Text { text: 'Stream Input'; color: '#E0E0E0'; font.pixelSize: 12 } }\n"
                "            Row { spacing: 12; Rectangle { width: 12; height: 12; radius: 6; color: '#FFD700'; border.color: 'white'; border.width: 1 } Text { text: 'Transform'; color: '#E0E0E0'; font.pixelSize: 12 } }\n"
                "            Row { spacing: 12; Rectangle { width: 12; height: 12; radius: 6; color: '#FF4500'; border.color: 'white'; border.width: 1 } Text { text: 'Emit Event'; color: '#E0E0E0'; font.pixelSize: 12 } }\n"
                "        }\n"
                "        MouseArea { anchors.fill: parent; drag.target: node1 }\n"
                "    }\n"
                "    Shape {\n"
                "        anchors.fill: parent\n"
                "        ShapePath {\n"
                "            strokeColor: '#00BFFF'; strokeWidth: 3; fillColor: 'transparent'\n"
                "            startX: node1.x + 220; startY: node1.y + 60\n"
                "            PathCubic { x: 650; y: 400; control1X: node1.x + 400; control1Y: node1.y + 60; control2X: 350; control2Y: 400 }\n"
                "        }\n"
                "    }\n"
                "}\n";
            setSource(QUrl()); QString tempPath = QDir::tempPath() + "/blueprint_neon.qml";
            QFile f(tempPath); if (f.open(QIODevice::WriteOnly)) { f.write(qml.toUtf8()); f.close(); }
            setSource(QUrl::fromLocalFile(tempPath));
        }
    };

    class DesignerPane : public QWidget {
        Q_OBJECT
    public:
        explicit DesignerPane(QWidget* parent = nullptr) : QWidget(parent) {
            auto* mainHLayout = new QHBoxLayout(this);
            mainHLayout->setContentsMargins(0, 0, 0, 0);
            mainHLayout->setSpacing(0);

            // NAVIGATION RAIL
            auto* navRail = new QWidget(this);
            navRail->setFixedWidth(60);
            navRail->setStyleSheet("background-color: #1a1a2e; border-right: 1px solid #3c3f41;");
            auto* railLayout = new QVBoxLayout(navRail);
            railLayout->setContentsMargins(0, 10, 0, 10);
            railLayout->setSpacing(20);

            auto addRailBtn = [&](const QString& icon, const QString& tip) {
                auto* btn = new QToolButton(this);
                btn->setFixedSize(45, 45);
                btn->setIcon(QIcon::fromTheme(icon));
                btn->setToolTip(tip);
                btn->setStyleSheet("QToolButton { background: transparent; border: none; border-radius: 10px; } QToolButton:hover { background: #30304a; }");
                railLayout->addWidget(btn, 0, Qt::AlignCenter);
            };

            addRailBtn("project-development", "Projeto");
            addRailBtn("code-context", "Editor");
            addRailBtn("vcs-branch", "Git");
            addRailBtn("debug-run", "Debug");
            railLayout->addStretch();
            addRailBtn("settings-configure", "Configurações");

            mainHLayout->addWidget(navRail);

            auto* designerTabs = new QTabWidget(this);
            designerTabs->setStyleSheet("QTabWidget::pane { border: none; background: #1e1f22; } "
                                       "QTabBar::tab { background: #2b2d30; color: #888; padding: 10px 25px; border-top-left-radius: 6px; border-top-right-radius: 6px; margin-right: 4px; border: 1px solid #3c3f41; border-bottom: none; } "
                                       "QTabBar::tab:selected { background: #1e1f22; color: #4285F4; font-weight: bold; border-top: 2px solid #4285F4; }");

            auto* uiDesignerWidget = new QWidget(this);
            auto* uiLayout = new QHBoxLayout(uiDesignerWidget);
            uiLayout->setContentsMargins(0, 0, 0, 0);

            m_model = new DesignerModel(this);
            auto* horizontalSplitter = new QSplitter(Qt::Horizontal, uiDesignerWidget);

            auto* paletteContainer = new QWidget(this);
            paletteContainer->setFixedWidth(220);
            paletteContainer->setStyleSheet("background-color: #2b2d30;");
            auto* palLayout = new QVBoxLayout(paletteContainer);

            m_viewNameEdit = new QLineEdit("Nova View.qml", this);
            m_viewNameEdit->setStyleSheet("color: #4285F4; font-weight: bold; padding: 5px; background: #1e1f22; border: 1px solid #3c3f41; border-radius: 4px; margin-bottom: 5px;");
            palLayout->addWidget(m_viewNameEdit);

            auto* btnSaveAs = new QPushButton(QIcon::fromTheme("document-save-as"), " Salvar View como...", this);
            btnSaveAs->setStyleSheet("background-color: #34A853; color: white; font-weight: bold; padding: 8px; border-radius: 4px; margin-bottom: 10px; border: none;");
            palLayout->addWidget(btnSaveAs);

            m_palette = new QListWidget(this);
            m_palette->addItem("--- QT QUICK BASICS ---");
            m_palette->addItems({"Rectangle", "Text", "Image", "Item", "Row", "Column", "Grid", "Flow"});
            m_palette->addItem("--- QT QUICK CONTROLS ---");
            m_palette->addItems({"Button", "TextField", "TextArea", "Label", "CheckBox", "RadioButton", "ProgressBar",
                                "Slider", "Switch", "Dial", "ComboBox", "BusyIndicator", "SpinBox", "ScrollView",
                                "GroupBox", "Frame", "ToolBar", "TabBar", "TabButton"});
            m_palette->addItem("--- KDE KIRIGAMI ---");
            m_palette->addItems({"Kirigami.Heading", "Kirigami.Card", "Kirigami.Icon", "Kirigami.Separator",
                                "Kirigami.InlineMessage", "Kirigami.ActionTextField", "Kirigami.SearchField",
                                "Kirigami.UrlButton", "Kirigami.PlaceholderMessage", "Kirigami.FormLayout",
                                "Kirigami.AbstractApplicationHeader", "Kirigami.Action"});
            m_palette->setStyleSheet("QListWidget { background: transparent; color: #ccc; border: none; font-size: 11px; } QListWidget::item { padding: 5px; }");
            palLayout->addWidget(new QLabel("🎨 Componentes"));
            palLayout->addWidget(m_palette);

            auto* centerSplitter = new QSplitter(Qt::Vertical, this);
            m_quickWidget = new QQuickWidget(this);
            m_quickWidget->rootContext()->setContextProperty("designerModel", m_model);
            m_quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);

            m_codeEditor = new QTextEdit(this);
            m_codeEditor->setReadOnly(true);
            m_codeEditor->setStyleSheet("background-color: #1a1a1a; color: #888; font-family: 'Monospace'; font-size: 10px;");

            centerSplitter->addWidget(m_quickWidget);
            centerSplitter->addWidget(m_codeEditor);
            centerSplitter->setStretchFactor(0, 3);

            auto* propContainer = new QWidget(this);
            propContainer->setFixedWidth(240);
            propContainer->setStyleSheet("background-color: #2b2d30; border-left: 1px solid #3c3f41;");
            auto* propLayout = new QVBoxLayout(propContainer);
            propLayout->setContentsMargins(10, 10, 10, 10);
            propLayout->setSpacing(8);
            propLayout->addWidget(new QLabel("⚙️ <b>PROPRIEDADES</b>"));

            auto addPropField = [&](const QString& labelText, QLineEdit*& field) {
                auto* lbl = new QLabel(labelText); propLayout->addWidget(lbl);
                field = new QLineEdit(this); field->setStyleSheet("background: #3c3f41; color: #bcbec4; border: 1px solid #4e5157; padding: 4px;");
                propLayout->addWidget(field); lbl->setVisible(false); field->setVisible(false); m_propLabels[field] = lbl;
            };

            addPropField("Texto / Título:", m_propText);
            addPropField("Posição X:", m_propX);
            addPropField("Posição Y:", m_propY);
            addPropField("Largura:", m_propWidth);
            addPropField("Altura:", m_propHeight);
            addPropField("Cor (Hex/Nome):", m_propColor);
            addPropField("Source (URL/Icon):", m_propSource);
            addPropField("Modo Redimensionamento:", m_propFillMode);
            addPropField("Valor (0.0 - 1.0):", m_propValue);
            addPropField("Raio (Borda):", m_propRadius);
            addPropField("Placeholder:", m_propPlaceholder);
            addPropField("Itens (CSV):", m_propItems);
            addPropField("Colunas (Grid):", m_propColumns);
            addPropField("Tam. Fonte:", m_propFontSize);
            addPropField("Largura Borda:", m_propBorderWidth);
            addPropField("Cor Borda:", m_propBorderColor);

            m_propChecked = new QCheckBox("Marcado / Ativo", this);
            m_propChecked->setStyleSheet("color: #bcbec4;");
            propLayout->addWidget(m_propChecked); m_propChecked->setVisible(false);
            propLayout->addStretch();

            horizontalSplitter->addWidget(paletteContainer);
            horizontalSplitter->addWidget(centerSplitter);
            horizontalSplitter->addWidget(propContainer);
            uiLayout->addWidget(horizontalSplitter);

            connect(m_palette, &QListWidget::itemDoubleClicked, [this](QListWidgetItem* item) { if (!item->text().startsWith("---")) m_model->addElement(item->text()); });
            connect(m_model, &DesignerModel::modelUpdated, [this]() { m_codeEditor->setPlainText(m_model->generateQmlCode()); });
            connect(m_model, &DesignerModel::elementSelected, this, &DesignerPane::onElementSelected);
            connect(btnSaveAs, &QPushButton::clicked, this, &DesignerPane::onSaveAsClicked);

            auto connectProp = [&](QLineEdit* field, const QString& prop) { connect(field, &QLineEdit::textChanged, [this, prop](const QString& val) { if (m_selectedIndex != -1) m_model->updateElementProperty(m_selectedIndex, prop, val); }); };
            connectProp(m_propText, "text"); connectProp(m_propX, "x"); connectProp(m_propY, "y"); connectProp(m_propWidth, "width"); connectProp(m_propHeight, "height");
            connectProp(m_propColor, "color"); connectProp(m_propSource, "source"); connectProp(m_propFillMode, "fillMode"); connectProp(m_propValue, "value");
            connectProp(m_propRadius, "radius"); connectProp(m_propPlaceholder, "placeholder"); connectProp(m_propItems, "items"); connectProp(m_propColumns, "columns");
            connectProp(m_propFontSize, "fontSize"); connectProp(m_propBorderWidth, "borderWidth"); connectProp(m_propBorderColor, "borderColor");

            connect(m_propChecked, &QCheckBox::toggled, [this](bool val) { if (m_selectedIndex != -1) m_model->updateElementProperty(m_selectedIndex, "checked", val); });

            auto* delShortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this); connect(delShortcut, &QShortcut::activated, [this]() { if (m_selectedIndex != -1) { m_model->removeElement(m_selectedIndex); m_selectedIndex = -1; } });
            auto* undoShortcut = new QShortcut(QKeySequence::Undo, this); connect(undoShortcut, &QShortcut::activated, [this]() { m_model->undo(); });
            auto* redoShortcut = new QShortcut(QKeySequence::Redo, this); connect(redoShortcut, &QShortcut::activated, [this]() { m_model->redo(); });
            auto* copyShortcut = new QShortcut(QKeySequence::Copy, this); connect(copyShortcut, &QShortcut::activated, [this]() { if (m_selectedIndex != -1) m_model->copyElement(m_selectedIndex); });
            auto* pasteShortcut = new QShortcut(QKeySequence::Paste, this); connect(pasteShortcut, &QShortcut::activated, [this]() { m_model->pasteElement(); });

            auto addMoveShortcut = [&](const QKeySequence& seq, int dx, int dy) {
                auto* s = new QShortcut(seq, this); connect(s, &QShortcut::activated, [this, dx, dy]() {
                    if (m_selectedIndex != -1) { auto e = m_model->getElement(m_selectedIndex); m_model->setElementPosition(m_selectedIndex, e.x + dx, e.y + dy); m_propX->setText(QString::number(e.x + dx)); m_propY->setText(QString::number(e.y + dy)); }
                });
            };
            addMoveShortcut(QKeySequence(Qt::Key_Left), -1, 0); addMoveShortcut(QKeySequence(Qt::Key_Right), 1, 0); addMoveShortcut(QKeySequence(Qt::Key_Up), 0, -1); addMoveShortcut(QKeySequence(Qt::Key_Down), 0, 1);
            addMoveShortcut(QKeySequence(Qt::ShiftModifier|Qt::Key_Left), -10, 0); addMoveShortcut(QKeySequence(Qt::ShiftModifier|Qt::Key_Right), 10, 0); addMoveShortcut(QKeySequence(Qt::ShiftModifier|Qt::Key_Up), 0, -10); addMoveShortcut(QKeySequence(Qt::ShiftModifier|Qt::Key_Down), 0, 10);

            setupCanvas();

            designerTabs->addTab(uiDesignerWidget, QIcon::fromTheme("view-preview"), "Visual Designer");
            auto* blueprintWidget = new BlueprintPane(this);
            designerTabs->addTab(blueprintWidget, QIcon::fromTheme("network-workgroup"), "Logic Blueprints");

            mainHLayout->addWidget(designerTabs);
        }

        void setViewName(const QString& name) { if (m_viewNameEdit) m_viewNameEdit->setText(name); }

    private slots:
        void onSaveAsClicked() {
            QString fileName = QFileDialog::getSaveFileName(this, "Salvar View QML", m_viewNameEdit->text(), "Arquivos QML (*.qml)");
            if (!fileName.isEmpty()) {
                QFile file(fileName);
                if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    file.write(m_model->generateQmlCode().toUtf8()); file.close();
                    QFileInfo info(fileName); m_viewNameEdit->setText(info.fileName());
                    QMessageBox::information(this, "Sucesso", "View salva com sucesso!");
                }
            }
        }

        void onElementSelected(int index) {
            m_selectedIndex = index; auto e = m_model->getElement(index);
            auto setFieldVisible = [&](QWidget* field, bool visible) { field->setVisible(visible); if (m_propLabels.contains(field)) m_propLabels[field]->setVisible(visible); };
            for (auto* f : m_propLabels.keys()) setFieldVisible(f, false);
            m_propChecked->setVisible(false);

            m_propX->setText(QString::number(e.x)); m_propY->setText(QString::number(e.y)); m_propWidth->setText(QString::number(e.width)); m_propHeight->setText(QString::number(e.height));
            setFieldVisible(m_propX, true); setFieldVisible(m_propY, true); setFieldVisible(m_propWidth, true); setFieldVisible(m_propHeight, true);

            if (e.type.contains("Button") || e.type.contains("Heading") || e.type.contains("Label") || e.type.contains("Text") || e.type == "CheckBox" || e.type == "Switch" || e.type == "GroupBox" || e.type == "Kirigami.UrlButton" || e.type == "Kirigami.PlaceholderMessage" || e.type == "Kirigami.InlineMessage" || e.type == "RadioButton" || e.type == "Kirigami.Card" || e.type == "Kirigami.Action" || e.type == "Kirigami.AbstractApplicationHeader" || e.type == "TabButton") { m_propText->setText(e.text); setFieldVisible(m_propText, true); }
            if (e.type.contains("Label") || e.type.contains("Text")) { m_propFontSize->setText(QString::number(e.fontSize)); setFieldVisible(m_propFontSize, true); }
            if (e.type == "CheckBox" || e.type == "Switch" || e.type == "RadioButton" || e.type == "BusyIndicator" || e.type == "Kirigami.Action" || e.type == "Kirigami.FormLayout") { m_propChecked->setChecked(e.checked); m_propChecked->setVisible(true); if (e.type == "Kirigami.FormLayout") m_propChecked->setText("Wide Mode (Largo)"); else m_propChecked->setText("Marcado / Ativo"); }
            if (e.type == "ComboBox" || e.type == "TabBar" || e.type == "ToolBar" || e.type == "Kirigami.FormLayout" || e.type == "ScrollView") { m_propItems->setText(e.items); setFieldVisible(m_propItems, true); }
            if (e.type == "Rectangle" || e.type == "Text" || e.type == "Kirigami.Icon") { m_propColor->setText(e.color); setFieldVisible(m_propColor, true); }
            if (e.type == "Rectangle") { m_propRadius->setText(QString::number(e.radius)); setFieldVisible(m_propRadius, true); m_propBorderWidth->setText(QString::number(e.borderWidth)); setFieldVisible(m_propBorderWidth, true); m_propBorderColor->setText(e.borderColor); setFieldVisible(m_propBorderColor, true); }
            if (e.type == "Grid") { m_propColumns->setText(QString::number(e.columns)); setFieldVisible(m_propColumns, true); }
            if (e.type == "Image" || e.type == "Kirigami.Icon" || e.type == "Kirigami.UrlButton" || e.type == "Kirigami.PlaceholderMessage" || e.type == "Kirigami.Action") { m_propSource->setText(e.source); setFieldVisible(m_propSource, true); if (e.type == "Image") { m_propFillMode->setText(e.fillMode); setFieldVisible(m_propFillMode, true); } }
            if (e.type == "ProgressBar" || e.type == "Slider" || e.type == "SpinBox" || e.type == "Dial" || e.type.contains("Row") || e.type.contains("Column") || e.type.contains("Grid") || e.type.contains("Flow")) { m_propValue->setText(QString::number(e.value)); setFieldVisible(m_propValue, true); }
            if (e.type == "TextField" || e.type == "TextArea" || e.type == "Kirigami.ActionTextField" || e.type == "Kirigami.SearchField") { m_propPlaceholder->setText(e.placeholder); setFieldVisible(m_propPlaceholder, true); }
        }

    private:
        void setupCanvas() {
            QString qml =
                "import QtQuick\nimport QtQuick.Controls\nimport org.kde.kirigami as Kirigami\n"
                "Rectangle {\n    id: canvas; color: '#1e1f22'; anchors.fill: parent\n    property int selectedIndex: -1\n"
                "    Repeater {\n        model: designerModel\n        delegate: Item {\n"
                "            id: wrapper; x: posX; y: posY; width: elementWidth; height: elementHeight\n"
                "            property bool isSelected: canvas.selectedIndex === index\n"
                "            property string eText: text; property string eItems: items; property var eValue: value; property string eColor: color; property int eRadius: radius; property string eSource: source; property string ePlaceholder: placeholder; property bool eChecked: checked; property int eColumns: columns; property int eFontSize: fontSize; property int eBorderWidth: borderWidth; property string eBorderColor: borderColor; property string eFillMode: fillMode\n"
                "            Rectangle { anchors.fill: parent; color: 'transparent'; border.color: (mouseArea.containsMouse || wrapper.isSelected) ? 'cyan' : 'transparent'; border.width: 2; z: 50 }\n"
                "            Rectangle {\n                id: resizeHandle; width: 10; height: 10; color: 'cyan'; anchors.right: parent.right; anchors.bottom: parent.bottom; z: 101\n                visible: mouseArea.containsMouse || wrapper.isSelected\n"
                "                MouseArea { anchors.fill: parent; cursorShape: Qt.SizeFDiagCursor\n                    onPressed: (mouse) => { mouseArea.drag.target = null }\n                    onReleased: (mouse) => { mouseArea.drag.target = wrapper }\n"
                "                    onPositionChanged: (mouse) => { if (pressed) { let newW = Math.max(20, wrapper.width + mouse.x); let newH = Math.max(20, wrapper.height + mouse.y); designerModel.setElementSize(index, newW, newH); } }\n"
                "                }\n            }\n"
                "            Loader {\n                id: loader; anchors.fill: parent\n"
                "                sourceComponent: {\n"
                "                    if (type === 'Button') return compButton\n                    if (type === 'Label') return compLabel\n                    if (type === 'Text') return compText\n                    if (type === 'TextField') return compTextField\n                    if (type === 'TextArea') return compTextArea\n                    if (type === 'CheckBox') return compCheckBox\n                    if (type === 'RadioButton') return compRadio\n                    if (type === 'ProgressBar') return compProgress\n                    if (type === 'Slider') return compSlider\n                    if (type === 'Switch') return compSwitch\n                    if (type === 'Dial') return compDial\n                    if (type === 'ComboBox') return compCombo\n                    if (type === 'BusyIndicator') return compBusy\n                    if (type === 'Rectangle') return compRect\n                    if (type === 'Image') return compImage\n                    if (type === 'Item') return compItem\n                    if (type === 'Row') return compRow\n                    if (type === 'Column') return compCol\n                    if (type === 'Grid') return compGrid\n                    if (type === 'Flow') return compFlow\n                    if (type === 'SpinBox') return compSpin\n                    if (type === 'ScrollView') return compScroll\n                    if (type === 'GroupBox') return compGroup\n                    if (type === 'Frame') return compFrame\n                    if (type === 'ToolBar') return compTool\n                    if (type === 'TabBar') return compTab\n                    if (type === 'TabButton') return compTabBtn\n                    if (type === 'Kirigami.Heading') return compKHeading\n                    if (type === 'Kirigami.Card') return compKCard\n                    if (type === 'Kirigami.Icon') return compKIcon\n                    if (type === 'Kirigami.Separator') return compKSep\n                    if (type === 'Kirigami.InlineMessage') return compKMsg\n                    if (type === 'Kirigami.ActionTextField') return compKActField\n                    if (type === 'Kirigami.SearchField') return compKSearch\n                    if (type === 'Kirigami.UrlButton') return compKUrl\n                    if (type === 'Kirigami.PlaceholderMessage') return compKPlaceholder\n                    if (type === 'Kirigami.FormLayout') return compKForm\n                    if (type === 'Kirigami.Action') return compKAction\n                    if (type === 'Kirigami.AbstractApplicationHeader') return compKAppHeader\n                    return compItem\n                }\n            }\n"
                "            Component { id: compButton; Button { text: wrapper.eText; anchors.fill: parent } }\n"
                "            Component { id: compLabel; Label { text: wrapper.eText; color: 'white'; font.pixelSize: wrapper.eFontSize; anchors.fill: parent } }\n"
                "            Component { id: compText; Text { text: wrapper.eText; color: wrapper.eColor; font.pixelSize: wrapper.eFontSize; anchors.fill: parent } }\n"
                "            Component { id: compTextField; TextField { anchors.fill: parent; placeholderText: wrapper.ePlaceholder } }\n"
                "            Component { id: compTextArea; TextArea { anchors.fill: parent; placeholderText: wrapper.ePlaceholder } }\n"
                "            Component { id: compCheckBox; CheckBox { text: wrapper.eText; checked: wrapper.eChecked; anchors.fill: parent } }\n"
                "            Component { id: compRadio; RadioButton { text: wrapper.eText; checked: wrapper.eChecked; anchors.fill: parent } }\n"
                "            Component { id: compProgress; ProgressBar { value: wrapper.eValue; anchors.fill: parent } }\n"
                "            Component { id: compSlider; Slider { value: wrapper.eValue; anchors.fill: parent } }\n"
                "            Component { id: compSwitch; Switch { text: wrapper.eText; checked: wrapper.eChecked; anchors.fill: parent } }\n"
                "            Component { id: compDial; Dial { value: wrapper.eValue; anchors.fill: parent } }\n"
                "            Component { id: compCombo; ComboBox { anchors.fill: parent; model: (wrapper.eItems || '').split(',') } }\n"
                "            Component { id: compBusy; BusyIndicator { running: wrapper.eChecked; anchors.fill: parent } }\n"
                "            Component { id: compSpin; SpinBox { value: wrapper.eValue * 100; anchors.fill: parent; from: 0; to: 100 } }\n"
                "            Component { id: compScroll; ScrollView { anchors.fill: parent; Column { width: parent.width; spacing: 5; Repeater { model: (wrapper.eItems || '').split(','); Label { text: modelData; color: 'white'; width: parent.width; elide: Text.ElideRight } } } } }\n"
                "            Component { id: compGroup; GroupBox { title: wrapper.eText; anchors.fill: parent } }\n"
                "            Component { id: compFrame; Frame { anchors.fill: parent } }\n"
                "            Component { id: compTool; ToolBar { anchors.fill: parent; Row { anchors.fill: parent; spacing: wrapper.eValue; Repeater { model: (wrapper.eItems || '').split(','); ToolButton { text: modelData; height: parent.height } } } } }\n"
                "            Component { id: compTab; TabBar { anchors.fill: parent; Repeater { model: (wrapper.eItems || '').split(','); TabButton { text: modelData; width: 80 } } } }\n"
                "            Component { id: compTabBtn; TabButton { text: wrapper.eText; anchors.fill: parent } }\n"
                "            Component { id: compRect; Rectangle { anchors.fill: parent; color: wrapper.eColor; radius: wrapper.eRadius; border.width: wrapper.eBorderWidth; border.color: wrapper.eBorderColor } }\n"
                "            Component { id: compItem; Rectangle { anchors.fill: parent; color: 'transparent'; border.color: 'gray'; border.width: 1; opacity: 0.5 } }\n"
                "            Component { id: compImage; Rectangle { anchors.fill: parent; color: '#2b2d30'; border.color: '#3c3f41'; border.width: 1; Image { id: img; anchors.fill: parent; source: wrapper.eSource; fillMode: Image[wrapper.eFillMode] } Kirigami.Icon { anchors.centerIn: parent; width: 48; height: 48; source: 'image-x-generic'; opacity: 0.5; visible: img.status !== Image.Ready } } }\n"
                "            Component { id: compRow; Row { spacing: wrapper.eValue; anchors.fill: parent; Rectangle { width: 40; height: parent.height; color: 'red' } Rectangle { width: 40; height: parent.height; color: 'blue' } } }\n"
                "            Component { id: compCol; Column { spacing: wrapper.eValue; anchors.fill: parent; Rectangle { width: parent.width; height: 40; color: 'red' } Rectangle { width: parent.width; height: 40; color: 'blue' } } }\n"
                "            Component { id: compGrid; Grid { columns: wrapper.eColumns; spacing: wrapper.eValue; anchors.fill: parent; Rectangle { width: 40; height: 40; color: 'red' } Rectangle { width: 40; height: 40; color: 'blue' } } }\n"
                "            Component { id: compFlow; Flow { spacing: wrapper.eValue; anchors.fill: parent; Rectangle { width: 40; height: 40; color: 'red' } Rectangle { width: 40; height: parent.height; color: 'blue' } } }\n"
                "            Component { id: compKHeading; Kirigami.Heading { text: wrapper.eText; color: 'white'; anchors.fill: parent } }\n"
                "            Component { id: compKCard; Kirigami.Card { anchors.fill: parent; header: Kirigami.Heading { text: wrapper.eText } } }\n"
                "            Component { id: compKIcon; Kirigami.Icon { source: wrapper.eSource; color: wrapper.eColor; anchors.fill: parent } }\n"
                "            Component { id: compKSep; Rectangle { anchors.centerIn: parent; width: parent.width; height: 2; color: '#444' } }\n"
                "            Component { id: compKMsg; Kirigami.InlineMessage { text: wrapper.eText; visible: true; anchors.fill: parent } }\n"
                "            Component { id: compKActField; Kirigami.ActionTextField { placeholderText: wrapper.ePlaceholder; anchors.fill: parent } }\n"
                "            Component { id: compKSearch; Kirigami.SearchField { placeholderText: wrapper.ePlaceholder; anchors.fill: parent } }\n"
                "            Component { id: compKUrl; Kirigami.UrlButton { text: wrapper.eText; url: wrapper.eSource; anchors.fill: parent } }\n"
                "            Component { id: compKPlaceholder; Kirigami.PlaceholderMessage { text: wrapper.eText; icon.name: wrapper.eSource; anchors.fill: parent } }\n"
                "            Component { id: compKForm; Kirigami.FormLayout { anchors.fill: parent; wideMode: wrapper.eChecked; Repeater { model: (wrapper.eItems || '').split(','); TextField { Kirigami.FormData.label: modelData; width: 150 } } } }\n"
                "            Component { id: compKAction; Rectangle { anchors.fill: parent; color: '#3c3f41'; radius: 4; border.color: '#4e5157'; Row { anchors.centerIn: parent; spacing: 8; Kirigami.Icon { source: wrapper.eSource; width: 16; height: 16; visible: wrapper.eSource !== '' } Label { text: wrapper.eText; color: 'white' } } } }\n"
                "            Component { id: compKAppHeader; Kirigami.AbstractApplicationHeader { anchors.fill: parent; Label { text: wrapper.eText; anchors.centerIn: parent; font.bold: true } } }\n"
                "            MouseArea {\n"
                "                id: mouseArea; anchors.fill: parent; drag.target: wrapper; z: 100\n"
                "                onPressed: { designerModel.selectElement(index); canvas.selectedIndex = index }\n"
                "                onReleased: designerModel.setElementPosition(index, wrapper.x, wrapper.y)\n"
                "            }\n"
                "        }\n"
                "    }\n"
                "}\n";
            m_quickWidget->setSource(QUrl()); m_quickWidget->engine()->clearComponentCache();
            QString tempPath = QDir::tempPath() + "/parcel_engine.qml";
            QFile f(tempPath); if (f.open(QIODevice::WriteOnly)) { f.write(qml.toUtf8()); f.close(); }
            m_quickWidget->setSource(QUrl::fromLocalFile(tempPath));
        }

        DesignerModel* m_model; QListWidget* m_palette; QQuickWidget* m_quickWidget; QTextEdit* m_codeEditor;
        QLineEdit* m_propText; QLineEdit* m_propX; QLineEdit* m_propY; QLineEdit* m_propWidth; QLineEdit* m_propHeight;
        QLineEdit* m_propColor; QLineEdit* m_propSource; QLineEdit* m_propFillMode; QLineEdit* m_propValue;
        QLineEdit* m_propRadius; QLineEdit* m_propPlaceholder; QLineEdit* m_propItems; QLineEdit* m_propColumns;
        QLineEdit* m_propFontSize; QLineEdit* m_propBorderWidth; QLineEdit* m_propBorderColor;
        QCheckBox* m_propChecked; QMap<QWidget*, QLabel*> m_propLabels; QLineEdit* m_viewNameEdit;
        int m_selectedIndex = -1;
    };
}

#endif

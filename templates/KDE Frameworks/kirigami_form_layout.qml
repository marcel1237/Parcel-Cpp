import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami

Kirigami.Page {
    title: "Kirigami Form Layout"

    Kirigami.FormLayout {
        anchors.fill: parent
        wideMode: true

        TextField {
            Kirigami.FormData.label: "Project Name:"
            placeholderText: "Enter project name..."
        }

        ComboBox {
            Kirigami.FormData.label: "Target Framework:"
            model: ["Qt 6", "KDE KF6", ".NET 8", "GTK 4"]
        }

        CheckBox {
            Kirigami.FormData.label: "Options:"
            text: "Enable Kirigami Theming"
            checked: true
        }
    }
}

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Kirigami.ScrollablePage {
    id: scrollPage
    title: "Kirigami Scrollable Page & Cards"

    ColumnLayout {
        spacing: Kirigami.Units.largeSpacing
        width: scrollPage.width

        Kirigami.Heading {
            text: "KDE Kirigami Card Layout"
            level: 2
        }

        Kirigami.Card {
            Layout.fillWidth: true
            header: Kirigami.Heading { text: "Card Title" }
            contentItem: Label {
                text: "This is a Kirigami card component designed for responsive layouts."
            }
            actions: [
                Kirigami.Action {
                    text: "Action 1"
                    icon.name: "dialog-ok"
                }
            ]
        }
    }
}

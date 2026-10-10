import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami

Kirigami.ApplicationWindow {
    id: root
    title: "Parcel C++ - Kirigami Application Window"
    width: 900
    height: 650
    visible: true

    globalDrawer: Kirigami.GlobalDrawer {
        title: "Navigation"
        titleIcon: "applications-system"
        actions: [
            Kirigami.Action {
                text: "Dashboard"
                icon.name: "go-home"
                onTriggered: pageStack.replace(dashboardPage)
            },
            Kirigami.Action {
                text: "Settings"
                icon.name: "preferences-system"
                onTriggered: pageStack.replace(settingsPage)
            }
        ]
    }

    pageStack.initialPage: Component {
        id: dashboardPage
        Kirigami.Page {
            title: "Kirigami Dashboard"
            Kirigami.Heading {
                text: "Welcome to KDE Kirigami 6"
                anchors.centerIn: parent
            }
        }
    }

    Component {
        id: settingsPage
        Kirigami.Page {
            title: "Settings"
            Label { text: "Kirigami Settings View"; anchors.centerIn: parent }
        }
    }
}

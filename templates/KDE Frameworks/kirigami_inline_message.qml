import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami

Item {
    width: 400
    height: 200

    Kirigami.InlineMessage {
        id: inlineMsg
        type: Kirigami.InlineMessage.Information
        text: "KDE Kirigami inline notification active."
        visible: true
        anchors.fill: parent

        actions: [
            Kirigami.Action {
                text: "Dismiss"
                icon.name: "dialog-close"
                onTriggered: inlineMsg.visible = false
            }
        ]
    }
}

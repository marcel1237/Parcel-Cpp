import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami

ApplicationWindow {
    width: 800; height: 600; visible: true

    Rectangle {
        id: rectangle_0; x: 282; y: 80; width: 100; height: 100
        color: "steelblue"; radius: 8; border.width: 0; border.color: "#000000"
    }

    Row {
        id: row_1; x: 470; y: 53; width: 160; height: 40
        spacing: 0.5
    }

    Image {
        id: image_2; x: 333; y: 167; width: 100; height: 100
        source: "https://picsum.photos/200"; fillMode: Image.PreserveAspectFit
    }

    TextField {
        id: textfield_3; x: 81; y: 339; width: 160; height: 40
        text: "Novo TextField"
        placeholderText: "Placeholder..."
    }

    Kirigami.Action {
        id: kirigami_action_4; x: 203; y: 23; width: 160; height: 40
        text: "Novo Kirigami.Action"
        icon.name: ""; checked: false
    }

    Kirigami.UrlButton {
        id: kirigami_urlbutton_5; x: 326; y: 122; width: 160; height: 40
        text: "Novo Kirigami.UrlButton"
        url: ""
    }

    Kirigami.Icon {
        id: kirigami_icon_6; x: 172; y: 90; width: 32; height: 32
        source: "document-save"; color: "steelblue"
    }

    Kirigami.Icon {
        id: kirigami_icon_7; x: 129; y: 29; width: 32; height: 32
        source: "document-save"; color: "steelblue"
    }

    Kirigami.Separator {
        id: kirigami_separator_8; x: 94; y: 223; width: 160; height: 40
    }

    Kirigami.Icon {
        id: kirigami_icon_9; x: 25; y: 121; width: 32; height: 32
        source: "document-save"; color: "steelblue"
    }

    Kirigami.Card {
        id: kirigami_card_10; x: 184; y: 354; width: 300; height: 150
        header: Kirigami.Heading { text: "Novo Kirigami.Card" }
    }

    Kirigami.Heading {
        id: kirigami_heading_11; x: 454; y: 263; width: 160; height: 40
        text: "Novo Kirigami.Heading"
    }

    Kirigami.Separator {
        id: kirigami_separator_12; x: 310; y: 342; width: 160; height: 40
    }

    Kirigami.Icon {
        id: kirigami_icon_13; x: 45; y: 54; width: 32; height: 32
        source: "document-save"; color: "steelblue"
    }

    Kirigami.Action {
        id: kirigami_action_14; x: 100; y: 100; width: 160; height: 40
        text: "Novo Kirigami.Action"
        icon.name: ""; checked: false
    }

    Kirigami.AbstractApplicationHeader {
        id: kirigami_abstractapplicationheader_15; x: 60; y: 166; width: 160; height: 40
        text: "Novo Kirigami.AbstractApplicationHeader"
    }

    Kirigami.FormLayout {
        id: kirigami_formlayout_16; x: 202; y: 141; width: 300; height: 200
        wideMode: true
        TextField { Kirigami.FormData.label: "Opção 1" }
        TextField { Kirigami.FormData.label: "Opção 2" }
        TextField { Kirigami.FormData.label: "Opção 3" }
    }

    Kirigami.InlineMessage {
        id: kirigami_inlinemessage_17; x: 276; y: 104; width: 350; height: 80
        text: "Novo Kirigami.InlineMessage"
    }

}

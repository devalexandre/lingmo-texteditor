import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LingmoUI.CompatibleModule 3.0 as LingmoUI

Popup {
    id: picker
    property var callback
    focus: true
    width: 340
    padding: LingmoUI.Units.largeSpacing

    readonly property var emojis: [
        "📄", "📝", "📒", "📚", "📌", "📎", "📅", "🗓️", "✅", "☑️", "💡", "⭐",
        "🔥", "🚀", "🎯", "🏆", "💼", "🏠", "🧠", "❤️", "🌱", "🌍", "☀️", "🌙",
        "⚠️", "❗", "❓", "ℹ️", "🔒", "🔑", "🛠️", "⚙️", "💻", "📱", "🎨", "🎵",
        "🍎", "☕", "🍕", "✈️", "🚗", "💰", "📈", "📊", "🧪", "🐛", "👋", "😀"
    ]

    function choose(value) {
        if (callback)
            callback(value)
        close()
    }

    contentItem: ColumnLayout {
        spacing: LingmoUI.Units.smallSpacing
        RowLayout {
            Layout.fillWidth: true
            Label {
                color: LingmoUI.Theme.textColor
                text: qsTr("Ícone")
                font.pointSize: LingmoUI.Units.fontSubheading
                font.weight: Font.DemiBold
                Layout.fillWidth: true
            }
            LingmoUI.ActionButton { text: qsTr("Remover"); destructive: true; subtle: true; onClicked: picker.choose("") }
        }
        Grid {
            columns: 8
            spacing: 2
            Repeater {
                model: picker.emojis
                delegate: Rectangle {
                    required property string modelData
                    width: 36
                    height: 36
                    radius: LingmoUI.Theme.smallRadius
                    color: emojiMouse.pressed ? LingmoUI.Theme.pressedColor
                         : emojiMouse.containsMouse ? LingmoUI.Theme.hoverColor : "transparent"
                    Behavior on color { ColorAnimation { duration: LingmoUI.Units.veryShortDuration } }
                    Text { anchors.centerIn: parent; text: modelData; font.pixelSize: 20 }
                    MouseArea { id: emojiMouse; anchors.fill: parent; hoverEnabled: true; onClicked: picker.choose(modelData) }
                }
            }
        }
    }
}

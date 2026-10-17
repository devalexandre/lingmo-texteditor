import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LingmoUI.CompatibleModule 3.0 as LingmoUI

// "Mover para…": pick a new parent (or the top level) for a page.
Popup {
    id: popup
    property string pageId: ""
    property var targets: []
    modal: true
    focus: true
    x: (parent.width - width) / 2
    y: Math.max(60, parent.height * 0.12)
    width: Math.min(460, parent.width - 80)
    height: Math.min(440, parent.height - y - 40)
    padding: LingmoUI.Units.smallSpacing

    function filtered() {
        const q = filter.text.toLowerCase()
        const all = [{ id: "", title: qsTr("Páginas (nível principal)"), icon: "", themeIcon: "go-home", depth: 0 }].concat(targets)
        return q === "" ? all : all.filter(t => t.title.toLowerCase().indexOf(q) >= 0)
    }
    function choose(id) {
        notes.move(pageId, id)
        close()
    }

    onAboutToShow: {
        targets = notes.moveTargets(pageId)
        filter.text = ""
        list.model = filtered()
        list.currentIndex = 0
        filter.forceActiveFocus()
    }

    contentItem: ColumnLayout {
        spacing: LingmoUI.Units.smallSpacing
        Label {
            color: LingmoUI.Theme.textColor
            text: qsTr("Mover “%1” para…").arg(notes.pageTitle(popup.pageId))
            font.pointSize: LingmoUI.Units.fontSubheading
            font.weight: Font.DemiBold
            elide: Text.ElideRight
            Layout.fillWidth: true
            leftPadding: LingmoUI.Units.smallSpacing
            topPadding: LingmoUI.Units.smallSpacing
        }
        TextField {
            id: filter
            Layout.fillWidth: true
            placeholderText: qsTr("Filtrar páginas…")
            onTextChanged: { list.model = popup.filtered(); list.currentIndex = 0 }
            Keys.onPressed: function(event) {
                if (event.key === Qt.Key_Down) { list.incrementCurrentIndex(); event.accepted = true }
                else if (event.key === Qt.Key_Up) { list.decrementCurrentIndex(); event.accepted = true }
                else if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                    if (list.currentIndex >= 0) popup.choose(list.model[list.currentIndex].id)
                    event.accepted = true
                }
            }
        }
        ListView {
            id: list
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 2
            ScrollBar.vertical: ScrollBar {}
            highlightMoveDuration: LingmoUI.Units.veryShortDuration
            highlight: Rectangle { radius: LingmoUI.Theme.mediumRadius; color: LingmoUI.Theme.highlightColor }
            delegate: Item {
                id: row
                required property var modelData
                required property int index
                width: ListView.view.width
                height: LingmoUI.Units.fontMetrics.height + LingmoUI.Units.largeSpacing
                readonly property color fg: ListView.isCurrentItem ? LingmoUI.Theme.highlightedTextColor : LingmoUI.Theme.textColor
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: LingmoUI.Units.largeSpacing + (filter.text === "" ? row.modelData.depth * 14 : 0)
                    spacing: LingmoUI.Units.smallSpacing
                    Label {
                        visible: row.modelData.icon !== ""
                        text: row.modelData.icon
                        font.pointSize: LingmoUI.Units.fontBody
                        Layout.preferredWidth: LingmoUI.Units.iconSizes.small
                        horizontalAlignment: Text.AlignHCenter
                    }
                    LingmoUI.IconItem {
                        visible: row.modelData.icon === ""
                        source: row.modelData.themeIcon || "text-x-generic"
                        Layout.preferredWidth: LingmoUI.Units.iconSizes.small
                        Layout.preferredHeight: LingmoUI.Units.iconSizes.small
                    }
                    Label {
                        text: row.modelData.title
                        color: row.fg
                        font.pointSize: LingmoUI.Units.fontBody
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: list.currentIndex = row.index
                    onClicked: popup.choose(row.modelData.id)
                }
            }
        }
    }
}

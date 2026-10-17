import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LingmoUI.CompatibleModule 3.0 as LingmoUI

// Ctrl+K / Ctrl+P: search titles and content, keyboard driven.
Popup {
    id: popup
    modal: true
    focus: true
    x: (parent.width - width) / 2
    y: Math.max(60, parent.height * 0.12)
    width: Math.min(600, parent.width - 80)
    height: Math.min(460, parent.height - y - 40)
    padding: LingmoUI.Units.smallSpacing

    property var results: []

    function openSearch() {
        field.text = ""
        refresh()
        open()
        field.forceActiveFocus()
    }
    function refresh() {
        results = notes.search(field.text)
        list.currentIndex = results.length > 0 ? 0 : -1
    }
    function activate(index) {
        if (index < 0 || index >= results.length)
            return
        const id = results[index].id
        close()
        root.openPage(id)
    }

    onOpened: field.forceActiveFocus()

    contentItem: ColumnLayout {
        spacing: LingmoUI.Units.smallSpacing
        TextField {
            id: field
            Layout.fillWidth: true
            placeholderText: qsTr("Buscar páginas e conteúdo…")
            selectByMouse: true
            onTextChanged: popup.refresh()
            Keys.onPressed: function(event) {
                if (event.key === Qt.Key_Down) {
                    list.currentIndex = Math.min(list.currentIndex + 1, list.count - 1)
                    event.accepted = true
                } else if (event.key === Qt.Key_Up) {
                    list.currentIndex = Math.max(list.currentIndex - 1, 0)
                    event.accepted = true
                } else if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                    popup.activate(list.currentIndex)
                    event.accepted = true
                } else if (event.key === Qt.Key_Escape) {
                    popup.close()
                    event.accepted = true
                }
            }
        }
        Label {
            text: field.text === "" ? qsTr("Editadas recentemente") : qsTr("Resultados")
            color: LingmoUI.Theme.secondaryTextColor
            font.pointSize: LingmoUI.Units.fontCaption
            font.weight: Font.DemiBold
            leftPadding: LingmoUI.Units.smallSpacing
            visible: list.count > 0
        }
        ListView {
            id: list
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: popup.results
            spacing: 2
            ScrollBar.vertical: ScrollBar {}
            highlightMoveDuration: LingmoUI.Units.veryShortDuration
            highlight: Rectangle {
                radius: LingmoUI.Theme.mediumRadius
                color: LingmoUI.Theme.highlightColor
            }
            delegate: Item {
                id: row
                required property var modelData
                required property int index
                readonly property bool isCurrent: ListView.isCurrentItem
                readonly property color fg: isCurrent ? LingmoUI.Theme.highlightedTextColor : LingmoUI.Theme.textColor
                readonly property color fg2: isCurrent ? LingmoUI.Theme.highlightedTextColor : LingmoUI.Theme.secondaryTextColor
                width: ListView.view.width
                height: col.implicitHeight + LingmoUI.Units.smallSpacing * 2
                Rectangle {
                    anchors.fill: parent
                    radius: LingmoUI.Theme.mediumRadius
                    color: !row.isCurrent && rowMouse.containsMouse ? LingmoUI.Theme.hoverColor : "transparent"
                    Behavior on color { ColorAnimation { duration: LingmoUI.Units.veryShortDuration } }
                }
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: LingmoUI.Units.largeSpacing
                    anchors.rightMargin: LingmoUI.Units.largeSpacing
                    spacing: LingmoUI.Units.smallSpacing
                    Label {
                        visible: row.modelData.icon !== ""
                        text: row.modelData.icon
                        font.pointSize: LingmoUI.Units.fontBody
                        Layout.preferredWidth: LingmoUI.Units.iconSizes.small
                        horizontalAlignment: Text.AlignHCenter
                        Layout.alignment: Qt.AlignTop
                        topPadding: 1
                    }
                    LingmoUI.IconItem {
                        visible: row.modelData.icon === ""
                        source: "text-x-generic"
                        Layout.preferredWidth: LingmoUI.Units.iconSizes.small
                        Layout.preferredHeight: LingmoUI.Units.iconSizes.small
                        Layout.alignment: Qt.AlignTop
                        Layout.topMargin: 2
                    }
                    ColumnLayout {
                        id: col
                        Layout.fillWidth: true
                        spacing: 0
                        RowLayout {
                            Layout.fillWidth: true
                            Label {
                                text: row.modelData.title
                                color: row.fg
                                font.pointSize: LingmoUI.Units.fontBody
                                elide: Text.ElideRight
                                Layout.maximumWidth: list.width * 0.6
                            }
                            Label {
                                visible: row.modelData.path !== ""
                                text: "— " + row.modelData.path
                                color: row.fg2
                                font.pointSize: LingmoUI.Units.fontCaption
                                elide: Text.ElideLeft
                                Layout.fillWidth: true
                            }
                            Item { Layout.fillWidth: row.modelData.path === "" }
                        }
                        Label {
                            visible: row.modelData.snippet !== ""
                            Layout.fillWidth: true
                            text: row.modelData.snippet
                            color: row.fg2
                            elide: Text.ElideRight
                            font.pointSize: LingmoUI.Units.fontCaption
                        }
                    }
                }
                MouseArea {
                    id: rowMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: popup.activate(row.index)
                }
            }
            LingmoUI.EmptyState {
                anchors.centerIn: parent
                width: Math.min(implicitWidth, parent.width)
                visible: list.count === 0
                iconName: field.text === "" ? "document-edit" : "edit-find"
                iconSize: LingmoUI.Units.iconSizes.large
                title: field.text === "" ? qsTr("Nenhuma página ainda") : qsTr("Nada encontrado para “%1”").arg(field.text)
            }
        }
    }
}

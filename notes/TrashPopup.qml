import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LingmoUI.CompatibleModule 3.0 as LingmoUI

Popup {
    id: popup
    modal: true
    focus: true
    x: (parent.width - width) / 2
    y: Math.max(60, parent.height * 0.12)
    width: Math.min(520, parent.width - 80)
    height: Math.min(480, parent.height - y - 40)
    padding: LingmoUI.Units.largeSpacing

    function filtered() {
        const q = filter.text.toLowerCase()
        const all = notes.trashPages
        return q === "" ? all : all.filter(t => t.title.toLowerCase().indexOf(q) >= 0)
    }

    onOpened: filter.text = ""

    contentItem: ColumnLayout {
        spacing: LingmoUI.Units.smallSpacing
        Label {
            color: LingmoUI.Theme.textColor
            text: qsTr("Lixeira")
            font.pointSize: LingmoUI.Units.fontHeading
            font.weight: Font.DemiBold
        }
        Label {
            text: qsTr("Páginas aqui ficam na pasta Trash das notas até serem excluídas permanentemente.")
            color: LingmoUI.Theme.secondaryTextColor
            font.pointSize: LingmoUI.Units.fontBody
            wrapMode: Text.Wrap
            Layout.fillWidth: true
        }
        TextField {
            id: filter
            Layout.fillWidth: true
            placeholderText: qsTr("Filtrar por título…")
        }
        ListView {
            id: list
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 2
            model: { notes.trashPages; filter.text; return popup.filtered() }
            ScrollBar.vertical: ScrollBar {}
            delegate: Item {
                id: row
                required property var modelData
                width: ListView.view.width
                height: LingmoUI.Units.fontMetrics.height * 2 + LingmoUI.Units.largeSpacing
                HoverHandler { id: rowHover }
                Rectangle {
                    anchors.fill: parent
                    radius: LingmoUI.Theme.mediumRadius
                    color: rowHover.hovered ? LingmoUI.Theme.hoverColor : "transparent"
                    Behavior on color { ColorAnimation { duration: LingmoUI.Units.veryShortDuration } }
                }
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: LingmoUI.Units.smallSpacing
                    anchors.rightMargin: LingmoUI.Units.smallSpacing
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
                        source: "text-x-generic"
                        Layout.preferredWidth: LingmoUI.Units.iconSizes.small
                        Layout.preferredHeight: LingmoUI.Units.iconSizes.small
                    }
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 0
                        Label {
                            color: LingmoUI.Theme.textColor
                            text: row.modelData.title
                            font.pointSize: LingmoUI.Units.fontBody
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                        }
                        Label {
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                            color: LingmoUI.Theme.secondaryTextColor
                            font.pointSize: LingmoUI.Units.fontCaption
                            text: {
                                let parts = []
                                if (row.modelData.parentTitle !== "")
                                    parts.push(qsTr("em %1").arg(row.modelData.parentTitle))
                                if (row.modelData.children > 0)
                                    parts.push(row.modelData.children === 1 ? qsTr("1 subpágina") : qsTr("%1 subpáginas").arg(row.modelData.children))
                                return parts.join(" · ")
                            }
                            visible: text !== ""
                        }
                    }
                    LingmoUI.IconButton {
                        iconName: "edit-undo"
                        tipText: qsTr("Restaurar")
                        onClicked: notes.restore(row.modelData.id)
                    }
                    LingmoUI.IconButton {
                        iconName: "edit-delete"
                        tipText: qsTr("Excluir permanentemente")
                        onClicked: {
                            const id = row.modelData.id
                            root.confirm(qsTr("Excluir “%1” permanentemente?").arg(row.modelData.title),
                                         qsTr("A página e suas subpáginas na lixeira serão apagadas do disco. Isso não pode ser desfeito."),
                                         qsTr("Excluir"), function() { notes.deleteForever(id) })
                        }
                    }
                }
            }
            LingmoUI.EmptyState {
                anchors.centerIn: parent
                width: Math.min(implicitWidth, parent.width)
                visible: list.count === 0
                iconName: "user-trash"
                iconSize: LingmoUI.Units.iconSizes.large
                title: qsTr("A lixeira está vazia")
            }
        }
    }
}

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LingmoUI.CompatibleModule 3.0 as LingmoUI

// Notes navigation: app title, actions, favourites, the page tree and the
// trash, all built from LingmoUI.SideBarItem rows (same rows as Mail).
Item {
    id: sidebar
    implicitWidth: 260

    Rectangle {
        anchors.fill: parent
        color: LingmoUI.Theme.sideBarColor
        Behavior on color { ColorAnimation { duration: LingmoUI.Units.longDuration; easing.type: Easing.Linear } }
    }
    Rectangle {
        anchors.right: parent.right
        width: 1
        height: parent.height
        color: LingmoUI.Theme.dividerColor
    }

    component SectionLabel: Item {
        property alias text: label.text
        property bool showAdd: false
        Layout.fillWidth: true
        Layout.topMargin: LingmoUI.Units.smallSpacing
        implicitHeight: Math.max(label.implicitHeight + LingmoUI.Units.smallSpacing, addButton.height)
        HoverHandler { id: sectionHover }
        Label {
            id: label
            anchors.left: parent.left
            anchors.leftMargin: LingmoUI.Units.largeSpacing
            anchors.verticalCenter: parent.verticalCenter
            color: LingmoUI.Theme.secondaryTextColor
            font.pointSize: LingmoUI.Units.fontCaption
            font.weight: Font.DemiBold
        }
        LingmoUI.IconButton {
            id: addButton
            visible: parent.showAdd
            opacity: sectionHover.hovered || hovered ? 1 : 0
            Behavior on opacity { NumberAnimation { duration: LingmoUI.Units.shortDuration } }
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            size: 22
            iconName: "list-add"
            tipText: qsTr("Nova página (Ctrl+N)")
            onClicked: root.newPage("")
        }
    }

    // One page: a SideBarItem with the tree arrow and the page's emoji drawn
    // in its indent, plus "⋯" / "+" buttons while hovered.
    component PageRow: Item {
        id: row
        property string pageId
        property string title
        property string icon
        property int depth: 0
        property bool hasChildren: false
        property bool expanded: false
        property bool current: false
        property bool treeMode: true

        readonly property int arrowWidth: 20
        readonly property int levelIndent: 14
        readonly property int iconSlot: LingmoUI.Units.iconSizes.small
        readonly property int iconGap: LingmoUI.Units.smallSpacing * 1.5
        readonly property real treeIndent: treeMode ? depth * levelIndent + arrowWidth - 4 : 0
        readonly property color fg: current ? LingmoUI.Theme.highlightedTextColor : LingmoUI.Theme.textColor
        readonly property bool hovered: rowHover.hovered

        Layout.fillWidth: true
        implicitHeight: item.implicitHeight

        HoverHandler { id: rowHover }

        LingmoUI.SideBarItem {
            id: item
            anchors.fill: parent
            title: row.title
            // Pages without an emoji get a document icon from the theme
            iconName: row.icon === "" ? "text-x-generic" : ""
            indent: row.treeIndent + (row.icon !== "" ? row.iconSlot + row.iconGap : 0)
            isCurrent: row.current
            onClicked: root.openPage(row.pageId)
            onRightClicked: root.showPageMenu(row.pageId, row, rowHover.point.position.x, rowHover.point.position.y)

            LingmoUI.IconButton {
                id: moreButton
                visible: row.hovered
                size: 22
                iconName: "view-more-horizontal-symbolic"
                tipText: qsTr("Excluir, duplicar e mais…")
                onClicked: root.showPageMenu(row.pageId, moreButton, 0, moreButton.height)
            }
            LingmoUI.IconButton {
                visible: row.hovered && row.treeMode
                size: 22
                iconName: "list-add"
                tipText: qsTr("Adicionar uma página dentro")
                onClicked: root.newPage(row.pageId)
            }
        }

        // Expand / collapse arrow
        Rectangle {
            visible: row.treeMode
            x: LingmoUI.Units.smallSpacing + row.depth * row.levelIndent
            anchors.verticalCenter: parent.verticalCenter
            width: row.arrowWidth
            height: row.arrowWidth
            radius: LingmoUI.Theme.tinyRadius
            color: arrowHover.hovered ? Qt.rgba(row.fg.r, row.fg.g, row.fg.b, 0.15) : "transparent"
            Behavior on color { ColorAnimation { duration: LingmoUI.Units.veryShortDuration } }
            LingmoUI.IconItem {
                anchors.centerIn: parent
                width: LingmoUI.Units.iconSizes.small
                height: width
                source: "pan-end-symbolic"
                opacity: row.hasChildren ? 0.85 : 0.35
                rotation: row.expanded ? 90 : 0
                Behavior on rotation { NumberAnimation { duration: LingmoUI.Units.shortDuration; easing.type: LingmoUI.Units.easing } }
            }
            HoverHandler { id: arrowHover }
            MouseArea {
                anchors.fill: parent
                onClicked: notes.toggleExpanded(row.pageId)
            }
        }

        // The page's own emoji (user content), where the SideBarItem icon would be
        Label {
            visible: row.icon !== ""
            x: LingmoUI.Units.largeSpacing + row.treeIndent
            anchors.verticalCenter: parent.verticalCenter
            width: row.iconSlot
            horizontalAlignment: Text.AlignHCenter
            text: row.icon
            font.pointSize: LingmoUI.Units.fontBody
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // The window controls sit in this strip
        Item { Layout.preferredHeight: root.header.height }

        Label {
            text: root.title
            color: root.active ? LingmoUI.Theme.textColor : LingmoUI.Theme.secondaryTextColor
            leftPadding: LingmoUI.Units.largeSpacing + LingmoUI.Units.smallSpacing
            rightPadding: LingmoUI.Units.largeSpacing
            topPadding: 2
            bottomPadding: LingmoUI.Units.smallSpacing * 1.5
            font.pointSize: LingmoUI.Units.fontTitle
            font.bold: true
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.leftMargin: LingmoUI.Units.largeSpacing
            Layout.rightMargin: LingmoUI.Units.largeSpacing
            spacing: 2
            LingmoUI.SideBarItem {
                Layout.fillWidth: true
                iconName: "edit-find"
                title: qsTr("Buscar")
                trailing: "Ctrl+K"
                onClicked: root.openSearch()
            }
            LingmoUI.SideBarItem {
                Layout.fillWidth: true
                iconName: "list-add"
                title: qsTr("Nova página")
                trailing: "Ctrl+N"
                onClicked: root.newPage("")
            }
        }

        ScrollView {
            id: scroll
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.topMargin: LingmoUI.Units.smallSpacing
            clip: true
            contentWidth: availableWidth

            ColumnLayout {
                width: scroll.availableWidth - LingmoUI.Units.largeSpacing * 2
                x: LingmoUI.Units.largeSpacing
                spacing: 2

                SectionLabel { text: qsTr("Favoritos"); visible: notes.favorites.length > 0 }
                Repeater {
                    model: notes.favorites
                    delegate: PageRow {
                        required property var modelData
                        treeMode: false
                        pageId: modelData.id
                        title: modelData.title
                        icon: modelData.icon
                        current: notes.currentId === modelData.id
                    }
                }

                SectionLabel { text: qsTr("Páginas"); showAdd: true }
                Repeater {
                    model: notes.tree
                    delegate: PageRow {
                        required pageId
                        required title
                        required icon
                        required depth
                        required hasChildren
                        required expanded
                        required current
                    }
                }
                Label {
                    visible: notes.tree.count === 0
                    Layout.fillWidth: true
                    Layout.margins: LingmoUI.Units.smallSpacing
                    text: qsTr("Nenhuma página ainda. Crie a primeira com Ctrl+N.")
                    color: LingmoUI.Theme.secondaryTextColor
                    font.pointSize: LingmoUI.Units.fontCaption
                    wrapMode: Text.Wrap
                }
                Item { Layout.preferredHeight: LingmoUI.Units.largeSpacing }
            }
        }

        LingmoUI.SideBarItem {
            Layout.fillWidth: true
            Layout.leftMargin: LingmoUI.Units.largeSpacing
            Layout.rightMargin: LingmoUI.Units.largeSpacing
            Layout.bottomMargin: LingmoUI.Units.largeSpacing
            iconName: "user-trash"
            title: qsTr("Lixeira")
            trailing: notes.trashPages.length > 0 ? String(notes.trashPages.length) : ""
            onClicked: root.openTrash()
        }
    }
}

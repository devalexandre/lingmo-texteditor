import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LingmoUI.CompatibleModule 3.0 as LingmoUI

LingmoUI.Window {
    id: root
    width: 1120
    height: 740
    minimumWidth: 760
    minimumHeight: 480
    visible: true
    title: qsTr("Notas")

    // Same window treatment as lingmo-settings / lingmo-store
    background.opacity: LingmoUI.Theme.darkMode ? 0.88 : 0.97
    header.height: LingmoUI.Units.windowHeaderHeight
    contentTopMargin: 0

    property bool sidebarVisible: true

    onClosing: function(close) { close.accepted = notes.save() }

    LingmoUI.WindowBlur {
        view: root
        geometry: Qt.rect(root.x, root.y, root.width, root.height)
        windowRadius: root.windowRadius
        enabled: true
    }

    function openSearch() { quickSearch.openSearch() }
    function openPage(id) { notes.open(id); pageView.focusTitleIfEmpty() }
    function newPage(parentId) {
        if (notes.create(parentId || "") !== "")
            pageView.focusTitle()
    }
    function showPageMenu(pageId, item, x, y) {
        pageMenu.pageId = pageId
        pageMenu.popup(item, x, y)
    }
    function pickEmoji(item, callback) {
        emojiPicker.callback = callback
        const p = item.mapToItem(root.contentItem, 0, item.height + 4)
        emojiPicker.x = Math.min(p.x, root.width - emojiPicker.width - 12)
        emojiPicker.y = Math.min(p.y, root.height - emojiPicker.height - 12)
        emojiPicker.open()
    }
    function startRename(pageId) {
        if (pageId === notes.currentId) {
            pageView.focusTitle()
            return
        }
        renamePopup.pageId = pageId
        renamePopup.open()
        renameField.text = notes.pageTitle(pageId)
        renameField.selectAll()
        renameField.forceActiveFocus()
    }
    function startMove(pageId) {
        movePopup.pageId = pageId
        movePopup.open()
    }
    function confirm(title, text, action, callback) {
        confirmPopup.titleText = title
        confirmPopup.bodyText = text
        confirmPopup.actionText = action
        confirmPopup.callback = callback
        confirmPopup.open()
    }
    function openTrash() { trashPopup.open() }

    Shortcut { sequences: ["Ctrl+P", "Ctrl+K"]; onActivated: root.openSearch() }
    Shortcut { sequence: "Ctrl+N"; onActivated: root.newPage("") }
    Shortcut { sequence: "Ctrl+S"; onActivated: notes.save() }
    Shortcut { sequence: "Ctrl+\\"; onActivated: root.sidebarVisible = !root.sidebarVisible }

    // Breadcrumbs and page actions live in the title bar, right of the sidebar
    headerItem: Item {
        id: headerBar
        property real originX: 0
        function updateOrigin() { originX = mapToItem(null, 0, 0).x }
        Component.onCompleted: Qt.callLater(updateOrigin)
        onWidthChanged: Qt.callLater(updateOrigin)

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: Math.max(0, (root.sidebarVisible ? sidebar.width : 0) - headerBar.originX) + LingmoUI.Units.largeSpacing
            anchors.rightMargin: LingmoUI.Units.largeSpacing
            spacing: 2

            LingmoUI.IconButton {
                iconName: "sidebar-show-symbolic"
                tipText: qsTr("Mostrar/ocultar barra lateral (Ctrl+\\)")
                onClicked: root.sidebarVisible = !root.sidebarVisible
            }

            Repeater {
                model: notes.breadcrumbs
                delegate: RowLayout {
                    required property var modelData
                    required property int index
                    spacing: 2
                    Label {
                        visible: index > 0
                        text: "/"
                        color: LingmoUI.Theme.secondaryTextColor
                        font.pointSize: LingmoUI.Units.fontBody
                    }
                    Rectangle {
                        Layout.preferredHeight: LingmoUI.Units.smallControlHeight
                        Layout.preferredWidth: Math.min(crumbRow.implicitWidth + LingmoUI.Units.largeSpacing, 200)
                        radius: LingmoUI.Theme.smallRadius
                        color: crumbMouse.pressed ? LingmoUI.Theme.pressedColor
                             : crumbMouse.containsMouse ? LingmoUI.Theme.hoverColor : "transparent"
                        Behavior on color { ColorAnimation { duration: LingmoUI.Units.veryShortDuration } }
                        RowLayout {
                            id: crumbRow
                            anchors.fill: parent
                            anchors.leftMargin: LingmoUI.Units.smallSpacing
                            anchors.rightMargin: LingmoUI.Units.smallSpacing
                            spacing: 4
                            Label {
                                visible: modelData.icon !== ""
                                text: modelData.icon
                                font.pointSize: LingmoUI.Units.fontBody
                            }
                            LingmoUI.IconItem {
                                visible: modelData.icon === ""
                                source: "text-x-generic"
                                Layout.preferredWidth: LingmoUI.Units.iconSizes.small
                                Layout.preferredHeight: LingmoUI.Units.iconSizes.small
                            }
                            Label {
                                Layout.fillWidth: true
                                text: modelData.title
                                elide: Text.ElideRight
                                color: LingmoUI.Theme.textColor
                                font.pointSize: LingmoUI.Units.fontBody
                            }
                        }
                        MouseArea {
                            id: crumbMouse
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: root.openPage(modelData.id)
                        }
                    }
                }
            }

            Item { Layout.fillWidth: true }

            Label {
                text: notes.status
                color: LingmoUI.Theme.secondaryTextColor
                font.pointSize: LingmoUI.Units.fontCaption
                elide: Text.ElideRight
                Layout.maximumWidth: 260
                visible: notes.currentId !== ""
            }
            LingmoUI.IconButton {
                visible: notes.currentId !== ""
                iconName: notes.favorite ? "starred-symbolic" : "non-starred-symbolic"
                checked: notes.favorite
                tipText: notes.favorite ? qsTr("Remover dos favoritos") : qsTr("Adicionar aos favoritos")
                onClicked: notes.setFavorite(notes.currentId, !notes.favorite)
            }
            LingmoUI.IconButton {
                id: moreButton
                visible: notes.currentId !== ""
                iconName: "view-more-horizontal-symbolic"
                tipText: qsTr("Mais ações")
                onClicked: root.showPageMenu(notes.currentId, moreButton, 0, moreButton.height)
            }
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Sidebar {
            id: sidebar
            Layout.fillHeight: true
            Layout.preferredWidth: 260
            visible: root.sidebarVisible
        }

        PageView {
            id: pageView
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }

    PageMenu { id: pageMenu }
    EmojiPicker { id: emojiPicker }
    QuickSearch { id: quickSearch }
    TrashPopup { id: trashPopup }
    MovePopup { id: movePopup }

    Popup {
        id: renamePopup
        property string pageId: ""
        modal: true
        focus: true
        anchors.centerIn: Overlay.overlay
        width: 360
        contentItem: ColumnLayout {
            spacing: LingmoUI.Units.largeSpacing
            Label {
                color: LingmoUI.Theme.textColor
                text: qsTr("Renomear página")
                font.pointSize: LingmoUI.Units.fontHeading
                font.weight: Font.DemiBold
            }
            TextField {
                id: renameField
                Layout.fillWidth: true
                selectByMouse: true
                onAccepted: renameButton.clicked()
            }
            RowLayout {
                Layout.alignment: Qt.AlignRight
                spacing: LingmoUI.Units.smallSpacing
                LingmoUI.ActionButton { text: qsTr("Cancelar"); onClicked: renamePopup.close() }
                LingmoUI.ActionButton {
                    id: renameButton
                    text: qsTr("Renomear")
                    primary: true
                    onClicked: {
                        notes.rename(renamePopup.pageId, renameField.text)
                        renamePopup.close()
                    }
                }
            }
        }
    }

    Popup {
        id: confirmPopup
        property string titleText
        property string bodyText
        property string actionText
        property var callback
        modal: true
        anchors.centerIn: Overlay.overlay
        width: 400
        contentItem: ColumnLayout {
            spacing: LingmoUI.Units.largeSpacing
            Label {
                color: LingmoUI.Theme.textColor
                text: confirmPopup.titleText
                font.pointSize: LingmoUI.Units.fontHeading
                font.weight: Font.DemiBold
                Layout.fillWidth: true
                wrapMode: Text.Wrap
            }
            Label {
                text: confirmPopup.bodyText
                Layout.fillWidth: true
                wrapMode: Text.Wrap
                color: LingmoUI.Theme.secondaryTextColor
                font.pointSize: LingmoUI.Units.fontBody
            }
            RowLayout {
                Layout.alignment: Qt.AlignRight
                spacing: LingmoUI.Units.smallSpacing
                LingmoUI.ActionButton { text: qsTr("Cancelar"); onClicked: confirmPopup.close() }
                LingmoUI.ActionButton {
                    text: confirmPopup.actionText
                    // The only confirmation is "delete forever"
                    destructive: true
                    onClicked: {
                        confirmPopup.close()
                        if (confirmPopup.callback)
                            confirmPopup.callback()
                    }
                }
            }
        }
    }
}

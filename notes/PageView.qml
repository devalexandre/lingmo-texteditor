import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LingmoUI.CompatibleModule 3.0 as LingmoUI

// The open page: icon + big title header and the block editor column.
Item {
    id: page
    readonly property var blocks: notes.blocks
    readonly property alias columnItem: column
    readonly property bool slashVisible: slash.visible
    property int slashRow: -1
    property int slashStart: -1
    property var slashBlock: null

    // ---- focus helpers used by the blocks ---------------------------------
    function focusBlock(row, pos) {
        Qt.callLater(function() {
            const item = repeater.itemAt(row)
            if (item)
                item.focusAt(pos)
        })
    }
    function applyFocus(r) {
        if (r && r.row !== undefined && r.row >= 0)
            focusBlock(r.row, r.pos)
    }
    function focusAtX(row, x, fromBottom) {
        const item = repeater.itemAt(row)
        if (item)
            item.focusAtX(x, fromBottom)
    }
    function focusTitle() {
        Qt.callLater(function() {
            titleEdit.forceActiveFocus()
            titleEdit.cursorPosition = titleEdit.length
        })
    }
    function focusTitleIfEmpty() {
        if (notes.currentId !== "" && notes.title === "")
            focusTitle()
    }
    function focusEnd() {
        if (notes.currentId === "")
            return
        const last = blocks.count - 1
        if (last >= 0 && blocks.typeAt(last) === "paragraph" && blocks.textAt(last) === "" && blocks.indentAt(last) === 0)
            focusBlock(last, 0)
        else
            focusBlock(blocks.insertBlock(last + 1, "paragraph", 0), 0)
    }
    function enterFromTitle() {
        if (blocks.count > 0 && blocks.typeAt(0) === "paragraph" && blocks.textAt(0) === "")
            focusBlock(0, 0)
        else
            focusBlock(blocks.insertBlock(0, "paragraph", 0), 0)
    }
    function ensureVisible(edit) {
        const flick = scroll.contentItem
        if (!flick || flick.contentY === undefined)
            return
        const r = edit.cursorRectangle
        const p = edit.mapToItem(flick.contentItem, r.x, r.y)
        if (p.y < flick.contentY + 8)
            flick.contentY = Math.max(0, p.y - 48)
        else if (p.y + r.height > flick.contentY + flick.height - 8)
            flick.contentY = Math.max(0, Math.min(flick.contentHeight - flick.height, p.y + r.height - flick.height + 48))
    }
    function insertBelowWithSlash(row) {
        let target = row
        if (!(blocks.typeAt(row) === "paragraph" && blocks.textAt(row) === ""))
            target = blocks.insertBlock(row + 1, "paragraph", blocks.indentAt(row))
        Qt.callLater(function() {
            const item = repeater.itemAt(target)
            if (item) {
                item.focusAt(-1)
                item.typeText("/")
            }
        })
    }
    function showBlockMenu(row, item) {
        blockMenu.row = row
        blockMenu.popup(item, 0, item.height)
    }

    // ---- slash menu -------------------------------------------------------
    function openSlash(item, start) {
        slashRow = item.index
        slashStart = start
        slashBlock = item
        slash.filter = ""
        const r = item.cursorRectIn(page)
        slash.x = Math.min(r.x, page.width - slash.width - 12)
        slash.y = r.y + r.height + 6
        if (slash.y + 360 > page.height)
            slash.y = Math.max(8, r.y - 366)
        slash.open()
    }
    function closeSlash() {
        if (slash.visible)
            slash.close()
        slashRow = -1
        slashBlock = null
    }
    function updateSlash(item) {
        const t = item.editText()
        const pos = item.cursorPos()
        if (!slash.visible) {
            if (item.blockType !== "code" && pos > 0 && t.charAt(pos - 1) === "/"
                    && (pos === 1 || /\s/.test(t.charAt(pos - 2))))
                openSlash(item, pos - 1)
        } else if (item.index === slashRow) {
            if (pos <= slashStart || t.charAt(slashStart) !== "/") {
                closeSlash()
                return
            }
            const q = t.substring(slashStart + 1, pos)
            if (q.length > 24 || q.indexOf("\n") >= 0 || q.indexOf("  ") >= 0)
                closeSlash()
            else
                slash.filter = q
        }
    }
    function cursorMoved(item) {
        if (slash.visible && item.index === slashRow && item.cursorPos() <= slashStart)
            closeSlash()
    }
    function blockFocusLost(item) {
        Qt.callLater(function() {
            if (slash.visible && slashBlock === item && !item.activeFocus)
                closeSlash()
        })
    }
    function slashMove(delta) { slash.move(delta) }
    function slashAccept() {
        const type = slash.currentType()
        if (type === "")
            closeSlash()
        else
            chooseSlash(type)
    }
    function chooseSlash(type) {
        const item = slashBlock
        const row = slashRow
        const start = slashStart
        if (!item)
            return
        const t = item.editText()
        const end = Math.max(item.cursorPos(), start + 1)
        closeSlash()
        blocks.setText(row, t.substring(0, start) + t.substring(end), -1)
        if (type === "page") {
            const id = notes.addSubpageBlock(row)
            if (id !== "") {
                notes.open(id)
                focusTitle()
            }
            return
        }
        applyFocus(blocks.applyType(row, type))
    }

    // ---- view -------------------------------------------------------------
    Rectangle {
        anchors.fill: parent
        color: LingmoUI.Theme.secondBackgroundColor
        Behavior on color { ColorAnimation { duration: LingmoUI.Units.longDuration } }
    }

    Connections {
        target: notes
        function onCurrentChanged() {
            page.closeSlash()
            titleEdit.text = notes.title
            if (scroll.contentItem && scroll.contentItem.contentY !== undefined)
                scroll.contentItem.contentY = 0
        }
        function onTitleChanged() {
            if (!titleEdit.activeFocus && titleEdit.text !== notes.title)
                titleEdit.text = notes.title
        }
    }

    ScrollView {
        id: scroll
        anchors.fill: parent
        anchors.topMargin: root.header.height
        visible: notes.currentId !== ""
        contentWidth: availableWidth
        clip: true

        Item {
            width: scroll.availableWidth
            height: Math.max(column.y + column.height + 160, scroll.availableHeight)

            // Clicking below the last block continues writing at the end
            MouseArea {
                x: column.x
                width: column.width
                y: column.y + column.height
                height: parent.height - y
                cursorShape: Qt.IBeamCursor
                onClicked: page.focusEnd()
            }

            Column {
                id: column
                width: Math.max(200, Math.min(720, parent.width - 2 * 72))
                x: (parent.width - width) / 2
                y: 36
                spacing: 0

                // Page icon + "Adicionar ícone"
                Item {
                    id: headerArea
                    width: parent.width
                    height: (notes.icon !== "" ? 76 : 0) + 30
                    HoverHandler { id: headerHover }

                    Text {
                        id: pageIcon
                        visible: notes.icon !== ""
                        text: notes.icon
                        font.pixelSize: 58
                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: root.pickEmoji(pageIcon, function(e) { notes.setIcon(notes.currentId, e) })
                        }
                    }
                    Rectangle {
                        id: addIcon
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 2
                        visible: notes.icon === ""
                        opacity: headerHover.hovered || titleEdit.activeFocus ? 1 : 0
                        Behavior on opacity { NumberAnimation { duration: LingmoUI.Units.shortDuration } }
                        width: addIconRow.implicitWidth + LingmoUI.Units.largeSpacing
                        height: LingmoUI.Units.smallControlHeight
                        radius: LingmoUI.Theme.smallRadius
                        color: addIconMouse.pressed ? LingmoUI.Theme.pressedColor
                             : addIconMouse.containsMouse ? LingmoUI.Theme.hoverColor : "transparent"
                        Behavior on color { ColorAnimation { duration: LingmoUI.Units.veryShortDuration } }
                        Row {
                            id: addIconRow
                            anchors.centerIn: parent
                            spacing: LingmoUI.Units.smallSpacing
                            LingmoUI.IconItem {
                                anchors.verticalCenter: parent.verticalCenter
                                width: LingmoUI.Units.iconSizes.small
                                height: width
                                source: "emoji-body-symbolic"
                            }
                            Label {
                                anchors.verticalCenter: parent.verticalCenter
                                text: qsTr("Adicionar ícone")
                                color: LingmoUI.Theme.secondaryTextColor
                                font.pointSize: LingmoUI.Units.fontBody
                            }
                        }
                        MouseArea {
                            id: addIconMouse
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: root.pickEmoji(addIcon, function(e) { notes.setIcon(notes.currentId, e) })
                        }
                    }
                }

                TextEdit {
                    id: titleEdit
                    width: parent.width
                    wrapMode: TextEdit.Wrap
                    textFormat: TextEdit.PlainText
                    selectByMouse: true
                    font.pixelSize: 38
                    font.bold: true
                    color: LingmoUI.Theme.textColor
                    selectionColor: LingmoUI.Theme.highlightColor
                    selectedTextColor: LingmoUI.Theme.highlightedTextColor
                    Component.onCompleted: text = notes.title
                    onTextChanged: if (activeFocus) notes.setTitle(text)
                    Keys.onPressed: function(event) {
                        if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                            page.enterFromTitle()
                            event.accepted = true
                        } else if (event.key === Qt.Key_Down && cursorRectangle.y + cursorRectangle.height >= contentHeight - 4) {
                            const n = blocks.nextEditable(-1)
                            if (n >= 0)
                                page.focusBlock(n, 0)
                            event.accepted = true
                        } else if ((event.modifiers & Qt.ControlModifier) && (event.key === Qt.Key_K || event.key === Qt.Key_P)) {
                            root.openSearch()
                            event.accepted = true
                        }
                    }
                    Text {
                        visible: titleEdit.length === 0 && titleEdit.preeditText === ""
                        text: qsTr("Sem título")
                        font: titleEdit.font
                        color: LingmoUI.Theme.secondaryTextColor
                        opacity: 0.6
                    }
                }

                Label {
                    width: parent.width
                    topPadding: 6
                    bottomPadding: 18
                    color: LingmoUI.Theme.secondaryTextColor
                    font.pointSize: LingmoUI.Units.fontCaption
                    text: notes.updatedText + "  ·  " + (notes.wordCount === 1 ? qsTr("1 palavra") : qsTr("%1 palavras").arg(notes.wordCount))
                }

                Repeater {
                    id: repeater
                    model: notes.blocks
                    delegate: BlockDelegate {
                        view: page
                        width: column.width
                    }
                }
            }
        }
    }

    // Nothing open
    LingmoUI.EmptyState {
        anchors.centerIn: parent
        width: Math.min(implicitWidth, parent.width - LingmoUI.Units.largeSpacing * 4)
        visible: notes.currentId === ""
        iconName: "document-edit"
        title: notes.pageCount > 0 ? qsTr("Escolha uma página na barra lateral") : qsTr("Seu espaço de notas está vazio")
        text: qsTr("Tudo fica salvo localmente, em arquivos Markdown.")
        actionText: qsTr("Nova página")
        onActionTriggered: root.newPage("")
    }

    SlashMenu {
        id: slash
        onChosen: function(type) { page.chooseSlash(type) }
        onClosed: page.slashRow = -1
    }

    Menu {
        id: blockMenu
        property int row: -1

        component TurnInto: MenuItem {
            property string blockType
            onTriggered: page.blocks.setType(blockMenu.row, blockType)
        }

        MenuItem { text: qsTr("Excluir"); onTriggered: page.applyFocus(page.blocks.deleteBlock(blockMenu.row)) }
        MenuItem { text: qsTr("Duplicar"); onTriggered: page.blocks.duplicateBlock(blockMenu.row) }
        MenuItem { text: qsTr("Mover para cima"); onTriggered: page.blocks.moveBlock(blockMenu.row, -1) }
        MenuItem { text: qsTr("Mover para baixo"); onTriggered: page.blocks.moveBlock(blockMenu.row, 1) }
        MenuSeparator {}
        Menu {
            title: qsTr("Transformar em")
            TurnInto { text: qsTr("Texto"); blockType: "paragraph" }
            TurnInto { text: qsTr("Título 1"); blockType: "h1" }
            TurnInto { text: qsTr("Título 2"); blockType: "h2" }
            TurnInto { text: qsTr("Título 3"); blockType: "h3" }
            TurnInto { text: qsTr("Lista"); blockType: "bullet" }
            TurnInto { text: qsTr("Lista numerada"); blockType: "numbered" }
            TurnInto { text: qsTr("Tarefa"); blockType: "todo" }
            TurnInto { text: qsTr("Lista suspensa"); blockType: "toggle" }
            TurnInto { text: qsTr("Citação"); blockType: "quote" }
            TurnInto { text: qsTr("Destaque"); blockType: "callout" }
            TurnInto { text: qsTr("Código"); blockType: "code" }
        }
    }
}

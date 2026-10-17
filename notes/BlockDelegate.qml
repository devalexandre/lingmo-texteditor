import QtQuick
import QtQuick.Controls
import LingmoUI.CompatibleModule 3.0 as LingmoUI

// One block of the page. Editing operations go to the C++ BlockModel
// (notes.blocks); this file only maps keys and draws the block.
Item {
    id: block
    required property int index
    required property string blockType
    required property string text
    required property int indent
    required property bool checked
    required property bool collapsed
    required property string emoji
    required property string language
    required property string ref
    required property int number
    required property bool hidden
    required property bool hasChildren
    required property string html
    required property string pageIcon
    property var view

    readonly property var blocks: notes.blocks
    readonly property bool textual: blockType !== "divider" && blockType !== "page"
    readonly property bool isHeading: blockType === "h1" || blockType === "h2" || blockType === "h3"
    readonly property bool isCode: blockType === "code"
    readonly property bool isCallout: blockType === "callout"
    readonly property bool isQuote: blockType === "quote"
    readonly property bool hasMarker: blockType === "bullet" || blockType === "numbered" || blockType === "todo" || blockType === "toggle"
    readonly property real indentStep: 26
    readonly property int fontSize: blockType === "h1" ? 30 : blockType === "h2" ? 24 : blockType === "h3" ? 19 : isCode ? 14 : 16
    readonly property real topGap: blockType === "h1" ? 24 : blockType === "h2" ? 16 : blockType === "h3" ? 10 : 2
    readonly property real padH: isCode || isCallout ? 16 : isQuote ? 16 : 2
    readonly property real padV: isCode || isCallout ? 12 : 3
    readonly property real markerWidth: hasMarker ? 26 : 0
    readonly property bool showRich: textual && !isCode && !edit.activeFocus && /[*_`~\[]|https?:\/\//.test(edit.text)
    property bool programmatic: false
    // Re-evaluated when pages change (e.g. the linked page went to the trash)
    readonly property bool linkAlive: { notes.pageCount; return blockType === "page" && notes.exists(ref) }

    function tint(a) { return Qt.rgba(LingmoUI.Theme.textColor.r, LingmoUI.Theme.textColor.g, LingmoUI.Theme.textColor.b, a) }

    width: parent ? parent.width : 0
    visible: !hidden
    height: hidden ? 0 : topGap + frame.height + 2

    // ---- editor plumbing -------------------------------------------------
    onTextChanged: if (edit.text !== text) Qt.callLater(syncText)
    Component.onCompleted: edit.text = text

    function syncText() {
        if (edit.text !== block.text) {
            programmatic = true
            edit.text = block.text
            programmatic = false
        }
    }
    function focusAt(pos) {
        syncText()
        if (!textual)
            return
        edit.forceActiveFocus()
        edit.cursorPosition = pos < 0 ? edit.length : Math.min(pos, edit.length)
    }
    function focusAtX(x, fromBottom) {
        syncText()
        if (!textual)
            return
        edit.forceActiveFocus()
        const local = edit.mapFromItem(view.columnItem, x, 0).x
        const y = fromBottom ? edit.contentHeight - fm.height / 2 : fm.height / 2
        edit.cursorPosition = edit.positionAt(local, y)
    }
    function cursorX() { return edit.mapToItem(view.columnItem, edit.cursorRectangle.x, 0).x }
    function cursorRectIn(item) {
        const r = edit.cursorRectangle
        return edit.mapToItem(item, r.x, r.y, r.width, r.height)
    }
    function editText() { return edit.text }
    function cursorPos() { return edit.cursorPosition }
    function typeText(t) { edit.insert(edit.cursorPosition, t) }

    function textEdited() {
        const r = blocks.setText(index, edit.text, programmatic ? -1 : edit.cursorPosition)
        if (r && r.applied) {
            view.closeSlash()
            view.applyFocus(r)
            return
        }
        if (!programmatic)
            view.updateSlash(block)
    }

    function wrapSelection(marker) {
        const m = marker.length
        const s = edit.selectionStart, e = edit.selectionEnd
        programmatic = true
        if (s === e) {
            edit.insert(s, marker + marker)
            edit.cursorPosition = s + m
        } else {
            const sel = edit.text.substring(s, e)
            if (sel.length >= 2 * m && sel.startsWith(marker) && sel.endsWith(marker)) {
                edit.remove(s, e)
                edit.insert(s, sel.substring(m, sel.length - m))
                edit.select(s, e - 2 * m)
            } else {
                edit.insert(e, marker)
                edit.insert(s, marker)
                edit.select(s + m, e + m)
            }
        }
        programmatic = false
        blocks.setText(index, edit.text, -1)
    }

    function onFirstLine() { return edit.cursorRectangle.y < fm.height * 0.5 }
    function onLastLine() { return edit.cursorRectangle.y + edit.cursorRectangle.height > edit.contentHeight - fm.height * 0.5 }

    function handleKey(e) {
        const ctrl = (e.modifiers & Qt.ControlModifier) !== 0
        const shift = (e.modifiers & Qt.ShiftModifier) !== 0
        const alt = (e.modifiers & Qt.AltModifier) !== 0
        const k = e.key
        const noSelection = edit.selectionStart === edit.selectionEnd

        if (view.slashVisible && view.slashRow === index) {
            if (k === Qt.Key_Up) { view.slashMove(-1); e.accepted = true; return }
            if (k === Qt.Key_Down) { view.slashMove(1); e.accepted = true; return }
            if (k === Qt.Key_Return || k === Qt.Key_Enter || k === Qt.Key_Tab) { view.slashAccept(); e.accepted = true; return }
            if (k === Qt.Key_Escape) { view.closeSlash(); e.accepted = true; return }
        }
        if (ctrl && (k === Qt.Key_K || k === Qt.Key_P)) { root.openSearch(); e.accepted = true; return }
        if (ctrl && !shift && k === Qt.Key_B) { wrapSelection("**"); e.accepted = true; return }
        if (ctrl && !shift && k === Qt.Key_I) { wrapSelection("*"); e.accepted = true; return }
        if (ctrl && !shift && k === Qt.Key_E) { wrapSelection("`"); e.accepted = true; return }
        if (ctrl && shift && k === Qt.Key_X) { wrapSelection("~~"); e.accepted = true; return }
        if (ctrl && !shift && k === Qt.Key_D) {
            view.focusBlock(blocks.duplicateBlock(index), edit.cursorPosition)
            e.accepted = true
            return
        }
        if (alt && shift && (k === Qt.Key_Up || k === Qt.Key_Down)) {
            const pos = edit.cursorPosition
            view.focusBlock(blocks.moveBlock(index, k === Qt.Key_Up ? -1 : 1), pos)
            e.accepted = true
            return
        }
        if (k === Qt.Key_Return || k === Qt.Key_Enter) {
            if (shift || (isCode && !ctrl))
                return // newline inside the block
            e.accepted = true
            view.applyFocus(blocks.split(index, edit.cursorPosition))
            return
        }
        if (k === Qt.Key_Backspace && !ctrl && edit.cursorPosition === 0 && noSelection) {
            e.accepted = true
            view.applyFocus(blocks.backspaceAtStart(index))
            return
        }
        if (k === Qt.Key_Tab && !ctrl) {
            e.accepted = true
            if (isCode) {
                edit.insert(edit.cursorPosition, "    ")
            } else {
                const pos = edit.cursorPosition
                blocks.changeIndent(index, 1)
                view.focusBlock(index, pos)
            }
            return
        }
        if (k === Qt.Key_Backtab) {
            e.accepted = true
            const pos = edit.cursorPosition
            blocks.changeIndent(index, -1)
            view.focusBlock(index, pos)
            return
        }
        if (shift || ctrl || alt)
            return
        if (k === Qt.Key_Up && onFirstLine()) {
            const p = blocks.previousEditable(index)
            if (p >= 0)
                view.focusAtX(p, cursorX(), true)
            else
                view.focusTitle()
            e.accepted = true
        } else if (k === Qt.Key_Down && onLastLine()) {
            const n = blocks.nextEditable(index)
            if (n >= 0) {
                view.focusAtX(n, cursorX(), false)
                e.accepted = true
            }
        } else if (k === Qt.Key_Left && edit.cursorPosition === 0 && noSelection) {
            const p = blocks.previousEditable(index)
            if (p >= 0) {
                view.focusBlock(p, -1)
                e.accepted = true
            }
        } else if (k === Qt.Key_Right && edit.cursorPosition === edit.length && noSelection) {
            const n = blocks.nextEditable(index)
            if (n >= 0) {
                view.focusBlock(n, 0)
                e.accepted = true
            }
        } else if (k === Qt.Key_Escape) {
            edit.deselect()
            view.forceActiveFocus()
            e.accepted = true
        }
    }

    // ---- drawing ---------------------------------------------------------
    HoverHandler { id: hover }

    FontMetrics { id: fm; font: edit.font }

    // Hover gutter: "+" adds a block below, "⋮⋮" opens the block menu
    Row {
        x: frame.x - width - 6
        y: block.topGap + block.padV + Math.max(0, (fm.height - 24) / 2)
        opacity: hover.hovered ? 1 : 0
        visible: opacity > 0
        Behavior on opacity { NumberAnimation { duration: LingmoUI.Units.veryShortDuration } }
        LingmoUI.IconButton {
            size: 24
            iconName: "list-add"
            tipText: qsTr("Adicionar um bloco abaixo")
            onClicked: block.view.insertBelowWithSlash(block.index)
        }
        LingmoUI.IconButton {
            id: handle
            size: 24
            iconName: "view-more-symbolic"
            tipText: qsTr("Opções do bloco · Alt+Shift+↑/↓ para mover")
            onClicked: block.view.showBlockMenu(block.index, handle)
        }
    }

    Rectangle {
        id: frame
        x: block.indent * block.indentStep
        y: block.topGap
        width: block.width - x
        height: blockType === "divider" ? 22 : blockType === "page" ? 34 : body.height + block.padV * 2
        radius: LingmoUI.Theme.smallRadius
        color: block.isCode ? block.tint(0.06)
             : block.isCallout ? block.tint(0.05)
             : blockType === "page" && pageMouse.containsMouse ? block.tint(0.06)
             : "transparent"
        Behavior on color { ColorAnimation { duration: LingmoUI.Units.shortDuration } }

        // Quote bar
        Rectangle {
            visible: block.isQuote
            width: 3
            height: parent.height
            radius: 1.5
            color: LingmoUI.Theme.textColor
        }

        // Divider
        Rectangle {
            visible: blockType === "divider"
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width
            height: 1
            color: block.tint(0.16)
        }

        // Link to a sub-page
        Row {
            visible: blockType === "page"
            opacity: block.linkAlive ? 1 : 0.45
            anchors.verticalCenter: parent.verticalCenter
            x: 4
            spacing: 6
            Text {
                visible: block.pageIcon !== ""
                anchors.verticalCenter: parent.verticalCenter
                text: block.pageIcon
                font.pixelSize: 16
            }
            LingmoUI.IconItem {
                visible: block.pageIcon === ""
                anchors.verticalCenter: parent.verticalCenter
                width: LingmoUI.Units.iconSizes.small
                height: width
                source: "text-x-generic"
            }
            Text {
                text: block.text !== "" ? block.text : qsTr("Sem título")
                font.pixelSize: 16
                font.underline: block.linkAlive
                font.strikeout: !block.linkAlive
                color: LingmoUI.Theme.textColor
            }
        }
        MouseArea {
            id: pageMouse
            visible: blockType === "page"
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: block.linkAlive ? Qt.PointingHandCursor : Qt.ArrowCursor
            onClicked: if (block.linkAlive) root.openPage(block.ref)
        }

        // List marker / checkbox / toggle arrow
        Item {
            id: marker
            visible: block.hasMarker
            x: block.padH
            y: block.padV
            width: block.markerWidth
            height: fm.height

            Text {
                visible: blockType === "bullet"
                anchors.centerIn: parent
                text: ["•", "◦", "▪"][block.indent % 3]
                font.pixelSize: Math.round(block.fontSize * 1.25)
                color: LingmoUI.Theme.textColor
            }
            Text {
                visible: blockType === "numbered"
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 6
                text: block.number + "."
                font.pixelSize: block.fontSize
                color: LingmoUI.Theme.textColor
            }
            Rectangle {
                visible: blockType === "todo"
                anchors.centerIn: parent
                width: 17
                height: 17
                radius: 4
                color: block.checked ? LingmoUI.Theme.highlightColor : "transparent"
                border.width: block.checked ? 0 : 1.5
                border.color: block.tint(0.45)
                Behavior on color { ColorAnimation { duration: LingmoUI.Units.shortDuration } }
                Text {
                    anchors.centerIn: parent
                    visible: block.checked
                    text: "✓"
                    font.pixelSize: 12
                    font.bold: true
                    color: LingmoUI.Theme.highlightedTextColor
                }
                MouseArea {
                    anchors.fill: parent
                    anchors.margins: -4
                    cursorShape: Qt.PointingHandCursor
                    onClicked: block.blocks.setChecked(block.index, !block.checked)
                }
            }
            Rectangle {
                visible: blockType === "toggle"
                anchors.centerIn: parent
                width: 22
                height: 22
                radius: 5
                color: toggleMouse.containsMouse ? block.tint(0.08) : "transparent"
                Text {
                    anchors.centerIn: parent
                    text: "▶"
                    font.pixelSize: 10
                    color: LingmoUI.Theme.textColor
                    rotation: block.collapsed ? 0 : 90
                    Behavior on rotation { NumberAnimation { duration: LingmoUI.Units.shortDuration; easing.type: LingmoUI.Units.easing } }
                }
                MouseArea {
                    id: toggleMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: block.blocks.setCollapsed(block.index, !block.collapsed)
                }
            }
        }

        // Callout icon
        Text {
            id: calloutIcon
            visible: block.isCallout
            x: block.padH
            y: block.padV
            text: block.emoji
            font.pixelSize: 19
            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    const row = block.index
                    root.pickEmoji(calloutIcon, function(e) { notes.blocks.setEmoji(row, e) })
                }
            }
        }

        // Code language
        TextInput {
            visible: block.isCode
            anchors.right: parent.right
            anchors.rightMargin: 10
            y: 6
            width: 90
            horizontalAlignment: TextInput.AlignRight
            text: block.language
            font.pointSize: LingmoUI.Units.fontCaption
            color: LingmoUI.Theme.secondaryTextColor
            selectByMouse: true
            onEditingFinished: block.blocks.setLanguage(block.index, text)
            Text {
                anchors.right: parent.right
                visible: parent.text === "" && !parent.activeFocus
                text: qsTr("linguagem")
                font: parent.font
                color: LingmoUI.Theme.secondaryTextColor
                opacity: 0.6
            }
        }

        Item {
            id: body
            visible: block.textual
            x: block.padH + block.markerWidth + (block.isCallout ? 32 : 0)
            y: block.padV
            width: frame.width - x - block.padH - (block.isCode ? 90 : 0)
            height: block.textual ? Math.max(fm.height, block.showRich ? rich.implicitHeight : edit.implicitHeight) : 0

            TextEdit {
                id: edit
                width: parent.width
                opacity: block.showRich ? 0 : 1
                textFormat: TextEdit.PlainText
                wrapMode: block.isCode ? TextEdit.WrapAnywhere : TextEdit.Wrap
                selectByMouse: true
                color: blockType === "todo" && block.checked ? LingmoUI.Theme.secondaryTextColor : LingmoUI.Theme.textColor
                selectionColor: LingmoUI.Theme.highlightColor
                selectedTextColor: LingmoUI.Theme.highlightedTextColor
                font.pixelSize: block.fontSize
                font.bold: block.isHeading
                font.strikeout: blockType === "todo" && block.checked
                font.family: block.isCode ? "monospace" : LingmoUI.Theme.fontFamily
                Keys.onPressed: function(event) { block.handleKey(event) }
                onTextChanged: if (activeFocus || block.programmatic) block.textEdited()
                onCursorPositionChanged: if (activeFocus) block.view.cursorMoved(block)
                onCursorRectangleChanged: if (activeFocus) block.view.ensureVisible(edit)
                onActiveFocusChanged: if (!activeFocus) block.view.blockFocusLost(block)
            }

            Text {
                id: rich
                width: parent.width
                visible: block.showRich
                textFormat: Text.RichText
                text: block.html
                wrapMode: Text.Wrap
                font: edit.font
                color: edit.color
                linkColor: LingmoUI.Theme.linkColor
                HoverHandler { cursorShape: rich.hoveredLink !== "" ? Qt.PointingHandCursor : Qt.IBeamCursor }
                TapHandler {
                    onTapped: function(point) {
                        const link = rich.linkAt(point.position.x, point.position.y)
                        if (link !== "" && /^(https?:|mailto:)/.test(link)) {
                            Qt.openUrlExternally(link)
                            return
                        }
                        block.syncText()
                        edit.forceActiveFocus()
                        edit.cursorPosition = edit.positionAt(point.position.x, point.position.y)
                    }
                }
            }

            Text {
                visible: edit.length === 0 && edit.preeditText === ""
                         && (edit.activeFocus || block.isHeading)
                text: block.isHeading ? qsTr("Título %1").arg(blockType.substring(1))
                    : blockType === "todo" ? qsTr("Tarefa")
                    : blockType === "bullet" || blockType === "numbered" ? qsTr("Lista")
                    : blockType === "toggle" ? qsTr("Lista suspensa")
                    : block.isQuote ? qsTr("Citação")
                    : block.isCallout ? qsTr("Escreva um destaque…")
                    : block.isCode ? ""
                    : qsTr("Escreva algo, ou digite “/” para comandos…")
                font: edit.font
                color: LingmoUI.Theme.secondaryTextColor
                opacity: 0.8
            }
        }
    }
}

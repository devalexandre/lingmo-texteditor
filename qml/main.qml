/*
 * Copyright (C) 2023 LingmoOS Team.
 */

import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.platform as Platform
import LingmoUI.CompatibleModule 3.0 as LingmoUI
import Lingmo.TextEditor 1.0

LingmoUI.Window {
    id: root
    width: 640
    height: 480
    minimumWidth: 300
    minimumHeight: 300
    visible: true
    title: qsTr("Lingmo OS Text Editor")

    FileHelper {
        id: fileHelper

        onNewPath: function(path) {
            _tabView.addTab(textEditorComponent, { fileUrl: "file://" + path, newFile: false })
        }

        onUnavailable: function(path) {
            root.notify(qsTr("%1 doesn't exists").arg(path))
        }
    }

    // Passive notification (LingmoUI.Toast is not usable on Qt 6 yet)
    Popup {
        id: _toast
        parent: Overlay.overlay
        x: Math.round((root.width - width) / 2)
        y: root.height - height - _bottomItem.height - LingmoUI.Units.largeSpacing
        modal: false
        focus: false
        closePolicy: Popup.NoAutoClose
        padding: LingmoUI.Units.largeSpacing

        property alias text: _toastLabel.text

        background: Rectangle {
            radius: LingmoUI.Theme.mediumRadius
            color: LingmoUI.Theme.secondBackgroundColor
            border.width: 1
            border.color: LingmoUI.Theme.darkMode ? Qt.rgba(1, 1, 1, 0.15) : Qt.rgba(0, 0, 0, 0.15)
        }

        contentItem: Label {
            id: _toastLabel
            color: LingmoUI.Theme.textColor
        }

        Timer {
            id: _toastTimer
            onTriggered: _toast.close()
        }
    }

    function notify(message) {
        _toast.text = message
        _toastTimer.interval = 3000
        _toastTimer.restart()
        _toast.open()
    }

    ExitPromptDialog {
        id: exitPrompt

        property var index: -1

        onOkBtnClicked: {
            if (index != -1)
                closeTab(index)
            else
                Qt.quit()
        }
    }

    headerItem: Item {
        Rectangle {
            anchors.fill: parent
            color: LingmoUI.Theme.backgroundColor
        }

        LingmoUI.TabBar {
            id: _tabbar
            anchors.fill: parent
            anchors.margins: LingmoUI.Units.smallSpacing / 2
            anchors.rightMargin: LingmoUI.Units.largeSpacing * 4

            model: _tabView.count
            currentIndex : _tabView.currentIndex

            onNewTabClicked: {
                addTab()
            }

            delegate: LingmoUI.TabButton {
                id: _tabBtn
                text: _tabView.contentModel.get(index).tabName
                implicitHeight: _tabbar.height
                implicitWidth: Math.min(_tabbar.width / _tabbar.count,
                                        _tabBtn.contentWidth)

                ToolTip.delay: 1000
                ToolTip.timeout: 5000

                checked: _tabView.currentIndex === index

                ToolTip.visible: hovered
                ToolTip.text: _tabView.contentModel.get(index).fileUrl

                onClicked: {
                    _tabView.currentIndex = index
                    _tabView.currentItem.forceActiveFocus()
                }

                onCloseClicked: {
                    closeProtection(index)
                }
            }
        }
    }

    DropArea {
        id: _dropArea
        anchors.fill: parent

        onDropped: function(drop) {
            if (drop.hasUrls) {
                for (var i = 0; i < drop.urls.length; ++i) {
                    root.addPath(fileHelper.toLocalFile(drop.urls[i]))
                }
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        LingmoUI.TabView {
            id: _tabView
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Item {
            id: _bottomItem
            z: 999
            Layout.fillWidth: true
            Layout.preferredHeight: 20 + LingmoUI.Units.smallSpacing

            Rectangle {
                anchors.fill: parent
                color: LingmoUI.Theme.backgroundColor
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.leftMargin: LingmoUI.Units.smallSpacing
                anchors.rightMargin: LingmoUI.Units.smallSpacing
                anchors.bottomMargin: LingmoUI.Units.smallSpacing

                Label {
                    text: _tabView.currentItem ? qsTr("Characters %1").arg(_tabView.currentItem.characterCount)
                                               : ""
                }
            }
        }
    }

    function addPath(path) {
        fileHelper.addPath(path)
        // _tabView.addTab(textEditorComponent, { fileUrl: path, newFile: false })
    }

    function addTab() {
        _tabView.addTab(textEditorComponent, { fileUrl: "", newFile: true, fileName: qsTr("Untitled") })
        _tabView.currentItem.forceActiveFocus()
    }

    Platform.FileDialog {
        id: fileOpenDialog
        title: qsTr("Open...")
        folder: Platform.StandardPaths.writableLocation(Platform.StandardPaths.HomeLocation)
        nameFilters: [ qsTr("All files (*)") ]
        fileMode: Platform.FileDialog.OpenFiles

        onAccepted: {
            for (var i = 0; i < fileOpenDialog.files.length; i++)
                addPath(fileHelper.toLocalFile(fileOpenDialog.files[i]))
        }
    }

    function open() {
        fileOpenDialog.open()
    }

    function closeAll() {
        for (var i = 0; i < _tabView.contentModel.count; i++) {
            var obj = _tabView.contentModel.get(i)
            if (obj.documentModified) {
                exitPrompt.index = -1
                showExitPrompt()
                return false
            }
        }
        return true
    }

    onClosing: function(close) {
        close.accepted = closeAll()
    }

    function showExitPrompt() {
        exitPrompt.x = root.x + Math.round((root.width - exitPrompt.width) / 2)
        exitPrompt.y = root.y + Math.round((root.height - exitPrompt.height) / 2)
        exitPrompt.visible = true
        exitPrompt.raise()
        exitPrompt.requestActivate()
    }

    function closeProtection(index) {
        var obj = _tabView.contentModel.get(index)
        if (obj.documentModified) {
            exitPrompt.index = index
            showExitPrompt()
            return
        }

        closeTab(index)
    }

    function closeTab(index) {
        _tabView.closeTab(index)

        if (_tabView.contentModel.count === 0)
            Qt.quit()

        _tabView.currentItem.forceActiveFocus()
    }

    function closeCurrentTab() {
        closeProtection(_tabView.currentIndex)
    }

    function toggleTab(arg) { //arg = -1 (forward) or 1 (backward)
        var nextIndex = _tabView.currentIndex + arg
        if (nextIndex > _tabView.contentModel.count - 1)
            nextIndex = 0
        if (nextIndex < 0)
            nextIndex = _tabView.contentModel.count - 1

        _tabView.currentIndex = nextIndex
        _tabView.currentItem.forceActiveFocus()
    }

    Component {
        id: textEditorComponent

        TextEditor {
            fileUrl: ""
            newFile: true
        }
    }

    Component.onCompleted: {
    }
}

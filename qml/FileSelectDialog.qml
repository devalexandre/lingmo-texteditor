/*
 * Copyright (C) 2023 LingmoOS Team.
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import LingmoUI.CompatibleModule 3.0 as LingmoUI

LingmoUI.Window {
    id: control

    property var contentWidth: _mainLayout.implicitWidth + LingmoUI.Units.largeSpacing * 2
    property var contentHeight: _mainLayout.implicitHeight + header.height + LingmoUI.Units.largeSpacing * 2
    property var fileUrl: ""
    property var displayText: ""

    flags: Qt.WindowStaysOnTopHint | Qt.Dialog | Qt.FramelessWindowHint
    modality: Qt.WindowModal
    visible: false

    width: contentWidth
    height: contentHeight
    minimumWidth: contentWidth
    minimumHeight: contentHeight
    maximumWidth: contentWidth
    maximumHeight: contentHeight
    minimizeButtonVisible: false

    background.color: LingmoUI.Theme.secondBackgroundColor

    signal okBtnClicked

    ColumnLayout {
        id: _mainLayout
        anchors.fill: parent
        spacing: LingmoUI.Units.smallSpacing
        anchors.margins: LingmoUI.Units.smallSpacing

        Label {
            text: displayText
            color: LingmoUI.Theme.textColor
            font.pointSize: LingmoUI.Units.fontBody
        }
        ColumnLayout {
            spacing: LingmoUI.Units.largeSpacing

            TextInput{
                id:textInput
                height: LingmoUI.Units.largeSpacing
                width: parent.width
                activeFocusOnPress: true
                autoScroll: true
                selectByMouse: true
                horizontalAlignment: TextInput.AlignLeft
                color: LingmoUI.Theme.textColor
                selectionColor: LingmoUI.Theme.highlightColor
                selectedTextColor: LingmoUI.Theme.highlightedTextColor
                font.pointSize: LingmoUI.Units.fontBody
                text: fileUrl

                onAccepted: {
                    fileUrl = text
                    control.visible = false
                    control.okBtnClicked()
                }
            }

            RowLayout {
                spacing: LingmoUI.Units.largeSpacing

                LingmoUI.ActionButton {
                    text: qsTr("Yes")
                    Layout.fillWidth: true
                    primary: true

                    onClicked: {
                        fileUrl = textInput.text
                        control.visible = false
                        control.okBtnClicked()
                    }
                }

                LingmoUI.ActionButton {
                    text: qsTr("Cancel")
                    Layout.fillWidth: true
                    onClicked: control.visible = false
                }
            }
        }
    }
}

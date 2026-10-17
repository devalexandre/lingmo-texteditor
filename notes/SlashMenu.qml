import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import LingmoUI.CompatibleModule 3.0 as LingmoUI

// "/" command menu. It never takes keyboard focus: the block editor keeps typing
// the filter and forwards Up/Down/Enter/Esc here.
Popup {
    id: menu
    property string filter: ""
    signal chosen(string type)

    width: 300
    height: Math.min(list.contentHeight + topPadding + bottomPadding + header.implicitHeight + 6, 360)
    padding: 6
    focus: false
    closePolicy: Popup.CloseOnPressOutside

    readonly property var allItems: [
        { type: "paragraph", label: qsTr("Texto"), hint: qsTr("Texto simples"), glyph: "Aa", keys: "texto text paragrafo paragraph" },
        { type: "h1", label: qsTr("Título 1"), hint: qsTr("Título grande de seção"), glyph: "H1", keys: "titulo heading h1 cabecalho" },
        { type: "h2", label: qsTr("Título 2"), hint: qsTr("Título médio"), glyph: "H2", keys: "titulo heading h2 cabecalho" },
        { type: "h3", label: qsTr("Título 3"), hint: qsTr("Título pequeno"), glyph: "H3", keys: "titulo heading h3 cabecalho" },
        { type: "bullet", label: qsTr("Lista"), hint: qsTr("Lista com marcadores"), glyph: "•", keys: "lista marcadores bullet ul" },
        { type: "numbered", label: qsTr("Lista numerada"), hint: qsTr("Lista com números"), glyph: "1.", keys: "lista numerada numbered ordenada ol" },
        { type: "todo", label: qsTr("Tarefa"), hint: qsTr("Lista de tarefas com caixas"), glyph: "☑", keys: "tarefa todo checkbox caixa lista" },
        { type: "toggle", label: qsTr("Lista suspensa"), hint: qsTr("Conteúdo que abre e fecha"), glyph: "▸", keys: "toggle alternar suspensa recolher" },
        { type: "quote", label: qsTr("Citação"), hint: qsTr("Destaque uma citação"), glyph: "❝", keys: "citacao quote" },
        { type: "callout", label: qsTr("Destaque"), hint: qsTr("Caixa com ícone para chamar atenção"), glyph: "💡", keys: "destaque callout aviso nota" },
        { type: "code", label: qsTr("Código"), hint: qsTr("Bloco de código"), glyph: "</>", keys: "codigo code snippet" },
        { type: "divider", label: qsTr("Divisor"), hint: qsTr("Linha separadora"), glyph: "—", keys: "divisor divider linha separador hr" },
        { type: "page", label: qsTr("Subpágina"), hint: qsTr("Uma página dentro desta"), glyph: "📄", keys: "pagina subpagina page" }
    ]

    function normalize(s) {
        return s.toLowerCase().normalize("NFD").replace(/[̀-ͯ]/g, "")
    }
    readonly property var items: {
        const q = normalize(filter.trim())
        if (q === "")
            return allItems
        return allItems.filter(i => normalize(i.label + " " + i.keys).indexOf(q) >= 0)
    }
    onItemsChanged: list.currentIndex = items.length > 0 ? 0 : -1
    onOpened: list.currentIndex = 0

    function move(delta) {
        if (items.length === 0)
            return
        list.currentIndex = (list.currentIndex + delta + items.length) % items.length
    }
    function currentType() {
        return list.currentIndex >= 0 && list.currentIndex < items.length ? items[list.currentIndex].type : ""
    }

    contentItem: ColumnLayout {
        spacing: 2
        Label {
            id: header
            text: menu.items.length > 0 ? qsTr("Blocos básicos") : qsTr("Nenhum resultado")
            color: LingmoUI.Theme.secondaryTextColor
            leftPadding: LingmoUI.Units.smallSpacing
            topPadding: 2
            font.pointSize: LingmoUI.Units.fontCaption
            font.weight: Font.DemiBold
        }
        ListView {
            id: list
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: menu.items
            spacing: 1
            highlightMoveDuration: LingmoUI.Units.veryShortDuration
            ScrollBar.vertical: ScrollBar {}
            highlight: Rectangle {
                radius: LingmoUI.Theme.mediumRadius
                color: LingmoUI.Theme.highlightColor
            }
            delegate: Item {
                id: row
                required property var modelData
                required property int index
                readonly property color fg: ListView.isCurrentItem ? LingmoUI.Theme.highlightedTextColor : LingmoUI.Theme.textColor
                readonly property color fg2: ListView.isCurrentItem ? LingmoUI.Theme.highlightedTextColor : LingmoUI.Theme.secondaryTextColor
                width: ListView.view.width
                height: 44
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: LingmoUI.Units.smallSpacing
                    anchors.rightMargin: LingmoUI.Units.smallSpacing
                    spacing: LingmoUI.Units.largeSpacing
                    Rectangle {
                        Layout.preferredWidth: 34
                        Layout.preferredHeight: 34
                        radius: LingmoUI.Theme.smallRadius
                        color: LingmoUI.Theme.secondBackgroundColor
                        border.width: 1
                        border.color: LingmoUI.Theme.borderColor
                        Label {
                            anchors.centerIn: parent
                            text: row.modelData.glyph
                            font.bold: true
                            color: LingmoUI.Theme.textColor
                        }
                    }
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 0
                        Label {
                            text: row.modelData.label
                            color: row.fg
                            font.pointSize: LingmoUI.Units.fontBody
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                        }
                        Label {
                            text: row.modelData.hint
                            color: row.fg2
                            font.pointSize: LingmoUI.Units.fontCaption
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                        }
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: list.currentIndex = row.index
                    onClicked: menu.chosen(row.modelData.type)
                }
            }
        }
    }
}

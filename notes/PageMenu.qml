import QtQuick
import QtQuick.Controls

// Per-page actions (sidebar "⋯", right click, title bar). Menu gets the Lingmo
// popup surface from the platform style.
Menu {
    id: menu
    property string pageId: ""
    readonly property bool isFavorite: pageId !== "" && notes.favorites.some(p => p.id === pageId)

    MenuItem { text: qsTr("Renomear"); onTriggered: root.startRename(menu.pageId) }
    MenuItem {
        text: qsTr("Adicionar subpágina")
        onTriggered: root.newPage(menu.pageId)
    }
    MenuItem {
        text: qsTr("Duplicar")
        onTriggered: {
            const id = notes.duplicate(menu.pageId)
            if (id !== "")
                root.openPage(id)
        }
    }
    MenuItem {
        text: menu.isFavorite ? qsTr("Remover dos favoritos") : qsTr("Adicionar aos favoritos")
        onTriggered: notes.setFavorite(menu.pageId, !menu.isFavorite)
    }
    MenuSeparator {}
    MenuItem { text: qsTr("Mover para…"); onTriggered: root.startMove(menu.pageId) }
    MenuItem { text: qsTr("Mover para cima"); onTriggered: notes.moveSibling(menu.pageId, -1) }
    MenuItem { text: qsTr("Mover para baixo"); onTriggered: notes.moveSibling(menu.pageId, 1) }
    MenuSeparator {}
    MenuItem {
        text: qsTr("Exportar como Markdown")
        visible: menu.pageId === notes.currentId
        height: visible ? implicitHeight : 0
        onTriggered: notes.exportCurrent()
    }
    MenuItem { text: qsTr("Mover para a lixeira"); onTriggered: notes.trash(menu.pageId) }
}

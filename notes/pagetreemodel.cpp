#include "pagetreemodel.h"

PageTreeModel::PageTreeModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int PageTreeModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : int(m_rows.size());
}

QHash<int, QByteArray> PageTreeModel::roleNames() const
{
    return {
        {IdRole, "pageId"}, {TitleRole, "title"}, {IconRole, "icon"}, {DepthRole, "depth"},
        {HasChildrenRole, "hasChildren"}, {ExpandedRole, "expanded"}, {FavoriteRole, "favorite"},
        {CurrentRole, "current"},
    };
}

QVariant PageTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_rows.size())
        return {};
    const Row &r = m_rows.at(index.row());
    switch (role) {
    case IdRole: return r.id;
    case TitleRole: return r.title;
    case IconRole: return r.icon;
    case DepthRole: return r.depth;
    case HasChildrenRole: return r.hasChildren;
    case ExpandedRole: return r.expanded;
    case FavoriteRole: return r.favorite;
    case CurrentRole: return r.id == m_current;
    }
    return {};
}

void PageTreeModel::setRows(const QList<Row> &rows)
{
    // Same shape (only titles/icons/flags changed): update in place so the
    // sidebar keeps its scroll position and delegates.
    bool sameShape = rows.size() == m_rows.size();
    for (int i = 0; sameShape && i < rows.size(); ++i)
        sameShape = rows.at(i).id == m_rows.at(i).id && rows.at(i).depth == m_rows.at(i).depth;
    if (sameShape) {
        const auto old = m_rows;
        m_rows = rows;
        for (int i = 0; i < rows.size(); ++i) {
            const Row &a = old.at(i), &b = rows.at(i);
            if (a.title != b.title || a.icon != b.icon || a.hasChildren != b.hasChildren
                || a.expanded != b.expanded || a.favorite != b.favorite)
                emit dataChanged(index(i), index(i));
        }
        return;
    }
    beginResetModel();
    m_rows = rows;
    endResetModel();
    emit countChanged();
}

void PageTreeModel::updatePage(const QString &id, const QString &title, const QString &icon, bool favorite)
{
    for (int i = 0; i < m_rows.size(); ++i) {
        Row &r = m_rows[i];
        if (r.id != id)
            continue;
        if (r.title == title && r.icon == icon && r.favorite == favorite)
            return;
        r.title = title;
        r.icon = icon;
        r.favorite = favorite;
        emit dataChanged(index(i), index(i), {TitleRole, IconRole, FavoriteRole});
        return;
    }
}

void PageTreeModel::setCurrent(const QString &id)
{
    if (m_current == id)
        return;
    m_current = id;
    if (!m_rows.isEmpty())
        emit dataChanged(index(0), index(int(m_rows.size()) - 1), {CurrentRole});
}

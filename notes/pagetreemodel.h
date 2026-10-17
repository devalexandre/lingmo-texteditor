#pragma once

#include <QAbstractListModel>

// The sidebar page tree, flattened to the rows currently visible (children of
// collapsed pages are left out). Built by NoteStore.
class PageTreeModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    struct Row {
        QString id;
        QString title;
        QString icon;
        int depth = 0;
        bool hasChildren = false;
        bool expanded = false;
        bool favorite = false;
    };
    enum Roles {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        IconRole,
        DepthRole,
        HasChildrenRole,
        ExpandedRole,
        FavoriteRole,
        CurrentRole,
    };

    explicit PageTreeModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const { return int(m_rows.size()); }
    void setRows(const QList<Row> &rows);
    void updatePage(const QString &id, const QString &title, const QString &icon, bool favorite);
    void setCurrent(const QString &id);
    QList<Row> rows() const { return m_rows; }

signals:
    void countChanged();

private:
    QList<Row> m_rows;
    QString m_current;
};

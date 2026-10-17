#pragma once

#include "markdown.h"

#include <QAbstractListModel>
#include <QVariantMap>
#include <functional>

// The blocks of the open page. Every editing operation the QML editor needs lives
// here so the delegates stay thin. Operations that move focus return
// {row, pos}: the block and cursor position the editor should focus next.
class BlockModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    enum Roles {
        TypeRole = Qt::UserRole + 1,
        TextRole,
        IndentRole,
        CheckedRole,
        CollapsedRole,
        EmojiRole,
        LanguageRole,
        RefRole,
        NumberRole,
        HiddenRole,
        HasChildrenRole,
        HtmlRole,
        IconRole,
    };

    explicit BlockModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const { return int(m_blocks.size()); }
    QList<Notes::Block> blocks() const { return m_blocks; }
    void setBlocks(const QList<Notes::Block> &blocks);
    QString markdown() const;

    // Title/icon lookup for page-link blocks (set by the store).
    std::function<QPair<QString, QString>(const QString &)> pageInfo;
    void refreshPageLinks();
    QString codeColor;

    Q_INVOKABLE QString typeAt(int row) const;
    Q_INVOKABLE QString textAt(int row) const;
    Q_INVOKABLE int indentAt(int row) const;

    // Text typed into a block. With cursor >= 0, Markdown shortcuts typed at the
    // start of a text block convert it ("# " → heading, "- " → list, ...).
    Q_INVOKABLE QVariantMap setText(int row, const QString &text, int cursor = -1);
    Q_INVOKABLE void setType(int row, const QString &type);
    // Slash menu: converts an empty block or inserts a new one below.
    Q_INVOKABLE QVariantMap applyType(int row, const QString &type);
    Q_INVOKABLE QVariantMap split(int row, int pos);
    Q_INVOKABLE QVariantMap backspaceAtStart(int row);
    Q_INVOKABLE QVariantMap deleteBlock(int row);
    Q_INVOKABLE int insertBlock(int row, const QString &type, int indent = 0, const QString &text = QString());
    Q_INVOKABLE bool changeIndent(int row, int delta);
    Q_INVOKABLE int moveBlock(int row, int delta);
    Q_INVOKABLE int duplicateBlock(int row);
    Q_INVOKABLE void setChecked(int row, bool checked);
    Q_INVOKABLE void setCollapsed(int row, bool collapsed);
    Q_INVOKABLE void setEmoji(int row, const QString &emoji);
    Q_INVOKABLE void setLanguage(int row, const QString &language);
    void setRef(int row, const QString &id, const QString &label);
    Q_INVOKABLE int previousEditable(int row) const;
    Q_INVOKABLE int nextEditable(int row) const;

signals:
    void countChanged();
    void edited();

private:
    bool valid(int row) const { return row >= 0 && row < m_blocks.size(); }
    int subtreeEnd(int row) const;
    void insertAt(int row, const Notes::Block &block);
    void removeAt(int row);
    void changed(int row, const QList<int> &roles);
    void recompute();
    static QVariantMap focus(int row, int pos);

    QList<Notes::Block> m_blocks;
    QList<int> m_numbers;
    QList<bool> m_hidden;
    QList<bool> m_hasChildren;
};

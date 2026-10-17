#include "blockmodel.h"

#include <QRegularExpression>

using Notes::Block;

namespace {
const int kMaxIndent = 8;

bool isHeading(const QString &type)
{
    return type == QLatin1String("h1") || type == QLatin1String("h2") || type == QLatin1String("h3");
}

const QStringList &knownTypes()
{
    static const QStringList types = {
        QStringLiteral("paragraph"), QStringLiteral("h1"), QStringLiteral("h2"), QStringLiteral("h3"),
        QStringLiteral("bullet"), QStringLiteral("numbered"), QStringLiteral("todo"), QStringLiteral("toggle"),
        QStringLiteral("quote"), QStringLiteral("callout"), QStringLiteral("code"), QStringLiteral("divider"),
    };
    return types;
}
} // namespace

BlockModel::BlockModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int BlockModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : int(m_blocks.size());
}

QHash<int, QByteArray> BlockModel::roleNames() const
{
    return {
        {TypeRole, "blockType"}, {TextRole, "text"}, {IndentRole, "indent"},
        {CheckedRole, "checked"}, {CollapsedRole, "collapsed"}, {EmojiRole, "emoji"},
        {LanguageRole, "language"}, {RefRole, "ref"}, {NumberRole, "number"},
        {HiddenRole, "hidden"}, {HasChildrenRole, "hasChildren"}, {HtmlRole, "html"},
        {IconRole, "pageIcon"},
    };
}

QVariant BlockModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !valid(index.row()))
        return {};
    const int row = index.row();
    const Block &b = m_blocks.at(row);
    switch (role) {
    case TypeRole: return b.type;
    case TextRole: return b.text;
    case IndentRole: return b.indent;
    case CheckedRole: return b.checked;
    case CollapsedRole: return b.collapsed;
    case EmojiRole: return b.emoji.isEmpty() ? QStringLiteral("💡") : b.emoji;
    case LanguageRole: return b.language;
    case RefRole: return b.ref;
    case NumberRole: return row < m_numbers.size() ? m_numbers.at(row) : 0;
    case HiddenRole: return row < m_hidden.size() ? m_hidden.at(row) : false;
    case HasChildrenRole: return row < m_hasChildren.size() ? m_hasChildren.at(row) : false;
    case HtmlRole:
        if (b.type == QLatin1String("code"))
            return b.text.toHtmlEscaped();
        return Notes::inlineToHtml(b.text, codeColor);
    case IconRole:
        return pageInfo && !b.ref.isEmpty() ? pageInfo(b.ref).second : QString();
    }
    return {};
}

void BlockModel::setBlocks(const QList<Block> &blocks)
{
    beginResetModel();
    m_blocks = blocks;
    for (Block &b : m_blocks) {
        if (!knownTypes().contains(b.type) && b.type != QLatin1String("page"))
            b.type = QStringLiteral("paragraph");
        b.indent = qBound(0, b.indent, kMaxIndent);
    }
    if (m_blocks.isEmpty())
        m_blocks.append(Block());
    m_numbers.clear();
    m_hidden.clear();
    m_hasChildren.clear();
    endResetModel();
    refreshPageLinks();
    recompute();
    emit countChanged();
}

QString BlockModel::markdown() const
{
    return Notes::serializeBlocks(m_blocks);
}

void BlockModel::refreshPageLinks()
{
    if (!pageInfo)
        return;
    for (int i = 0; i < m_blocks.size(); ++i) {
        Block &b = m_blocks[i];
        if (b.type != QLatin1String("page"))
            continue;
        const QString title = pageInfo(b.ref).first;
        if (!title.isEmpty() && title != b.text)
            b.text = title;
        changed(i, {TextRole, HtmlRole, IconRole});
    }
}

QString BlockModel::typeAt(int row) const { return valid(row) ? m_blocks.at(row).type : QString(); }
QString BlockModel::textAt(int row) const { return valid(row) ? m_blocks.at(row).text : QString(); }
int BlockModel::indentAt(int row) const { return valid(row) ? m_blocks.at(row).indent : 0; }

QVariantMap BlockModel::focus(int row, int pos)
{
    return {{QStringLiteral("row"), row}, {QStringLiteral("pos"), pos}};
}

void BlockModel::changed(int row, const QList<int> &roles)
{
    const QModelIndex i = index(row);
    emit dataChanged(i, i, roles);
}

QVariantMap BlockModel::setText(int row, const QString &text, int cursor)
{
    if (!valid(row) || !Notes::isTextual(m_blocks.at(row).type) || m_blocks.at(row).text == text)
        return {};
    Block &b = m_blocks[row];
    if (cursor > 0 && cursor <= text.size()
        && (b.type == QLatin1String("paragraph") || b.type == QLatin1String("bullet"))) {
        const QString prefix = text.left(cursor);
        QString type;
        bool checked = false;
        if (b.type == QLatin1String("paragraph")) {
            static const QRegularExpression numbered(QStringLiteral("^\\d{1,9}[.)] $"));
            if (prefix == QLatin1String("# ")) type = QStringLiteral("h1");
            else if (prefix == QLatin1String("## ")) type = QStringLiteral("h2");
            else if (prefix == QLatin1String("### ")) type = QStringLiteral("h3");
            else if (prefix == QLatin1String("- ") || prefix == QLatin1String("* ") || prefix == QLatin1String("+ ")) type = QStringLiteral("bullet");
            else if (numbered.match(prefix).hasMatch()) type = QStringLiteral("numbered");
            else if (prefix == QLatin1String("> ")) type = QStringLiteral("quote");
            else if (prefix == QLatin1String("```")) type = QStringLiteral("code");
            else if (prefix == QLatin1String("---") && text == prefix) type = QStringLiteral("divider");
        }
        if (prefix == QLatin1String("[] ") || prefix == QLatin1String("[ ] ")) {
            type = QStringLiteral("todo");
        } else if (prefix == QLatin1String("[x] ") || prefix == QLatin1String("[X] ")) {
            type = QStringLiteral("todo");
            checked = true;
        }
        if (!type.isEmpty()) {
            b.type = type;
            b.checked = checked;
            b.text = text.mid(cursor);
            changed(row, {TypeRole, TextRole, CheckedRole, HtmlRole});
            recompute();
            emit edited();
            QVariantMap result = focus(row, 0);
            if (type == QLatin1String("divider")) {
                b.text.clear();
                insertAt(row + 1, Block{QStringLiteral("paragraph"), {}, b.indent});
                result = focus(row + 1, 0);
            }
            result.insert(QStringLiteral("applied"), true);
            return result;
        }
    }
    b.text = text;
    // The editor that typed it compares before assigning, so this is a no-op there.
    changed(row, {TextRole, HtmlRole});
    emit edited();
    return {};
}

void BlockModel::setType(int row, const QString &type)
{
    if (!valid(row) || !knownTypes().contains(type))
        return;
    Block &b = m_blocks[row];
    if (b.type == type)
        return;
    b.type = type;
    b.checked = false;
    if (type == QLatin1String("divider"))
        b.text.clear();
    if (isHeading(type))
        b.text.replace(QLatin1Char('\n'), QLatin1Char(' '));
    changed(row, {TypeRole, TextRole, CheckedRole, HtmlRole});
    recompute();
    emit edited();
}

QVariantMap BlockModel::applyType(int row, const QString &type)
{
    if (!valid(row) || !knownTypes().contains(type))
        return {};
    const Block &b = m_blocks.at(row);
    int target = row;
    if (b.text.isEmpty() && Notes::isTextual(b.type)) {
        setType(row, type);
    } else {
        target = insertBlock(subtreeEnd(row), type, b.indent);
    }
    if (type == QLatin1String("divider")) {
        const int next = target + 1;
        if (!valid(next) || m_blocks.at(next).type != QLatin1String("paragraph") || !m_blocks.at(next).text.isEmpty())
            insertBlock(next, QStringLiteral("paragraph"), m_blocks.at(target).indent);
        return focus(next, 0);
    }
    return focus(target, 0);
}

QVariantMap BlockModel::split(int row, int pos)
{
    if (!valid(row))
        return {};
    Block &b = m_blocks[row];
    if (!Notes::isTextual(b.type))
        return focus(insertBlock(row + 1, QStringLiteral("paragraph"), b.indent), 0);

    const bool list = Notes::isListLike(b.type);
    if (b.text.isEmpty() && b.type == QLatin1String("paragraph") && b.indent > 0) {
        // Enter on an empty nested line steps out (e.g. out of a toggle).
        changeIndent(row, -1);
        return focus(row, 0);
    }
    if (b.text.isEmpty() && b.type != QLatin1String("paragraph")) {
        // Enter on an empty list item leaves the list (outdenting first).
        if (list && b.indent > 0)
            changeIndent(row, -1);
        else
            setType(row, QStringLiteral("paragraph"));
        return focus(row, 0);
    }

    pos = qBound(0, pos, int(b.text.size()));
    Block next;
    next.indent = b.indent;
    int insertRow = row + 1;
    if (b.type == QLatin1String("toggle")) {
        if (b.collapsed) {
            next.type = QStringLiteral("toggle");
            next.collapsed = true;
            insertRow = subtreeEnd(row);
        } else {
            next.indent = qMin(b.indent + 1, kMaxIndent);
        }
    } else if (list) {
        next.type = b.type;
    }

    if (pos == 0) {
        // Enter at the very start pushes the block down, like Notion.
        Block empty;
        empty.indent = b.indent;
        empty.type = list ? b.type : QStringLiteral("paragraph");
        if (empty.type == QLatin1String("toggle"))
            empty.collapsed = true;
        insertAt(row, empty);
        return focus(row + 1, 0);
    }

    next.text = b.text.mid(pos);
    b.text = b.text.left(pos);
    changed(row, {TextRole, HtmlRole});
    insertAt(insertRow, next);
    return focus(insertRow, 0);
}

QVariantMap BlockModel::backspaceAtStart(int row)
{
    if (!valid(row))
        return {};
    Block &b = m_blocks[row];
    if (!b.text.isEmpty() && b.type != QLatin1String("paragraph") && Notes::isTextual(b.type)) {
        setType(row, QStringLiteral("paragraph"));
        return focus(row, 0);
    }
    if (!b.text.isEmpty() && b.indent > 0) {
        changeIndent(row, -1);
        return focus(row, 0);
    }
    int prev = row - 1;
    while (prev >= 0 && m_hidden.value(prev))
        --prev;
    if (prev < 0) {
        if (b.type != QLatin1String("paragraph"))
            setType(row, QStringLiteral("paragraph"));
        return focus(row, 0);
    }
    if (!Notes::isTextual(m_blocks.at(prev).type)) {
        removeAt(prev);
        return focus(row - 1, 0);
    }
    Block &p = m_blocks[prev];
    const int pos = int(p.text.size());
    if (!b.text.isEmpty()) {
        p.text += isHeading(p.type) ? QString(b.text).replace(QLatin1Char('\n'), QLatin1Char(' ')) : b.text;
        changed(prev, {TextRole, HtmlRole});
    }
    removeAt(row);
    return focus(prev, pos);
}

QVariantMap BlockModel::deleteBlock(int row)
{
    if (!valid(row))
        return {};
    const int prev = previousEditable(row);
    removeAt(row);
    if (prev >= 0)
        return focus(prev, int(m_blocks.at(prev).text.size()));
    return focus(qMin(row, int(m_blocks.size()) - 1), 0);
}

int BlockModel::insertBlock(int row, const QString &type, int indent, const QString &text)
{
    row = qBound(0, row, int(m_blocks.size()));
    Block b;
    b.type = knownTypes().contains(type) ? type : QStringLiteral("paragraph");
    b.indent = qBound(0, indent, kMaxIndent);
    b.text = b.type == QLatin1String("divider") ? QString() : text;
    insertAt(row, b);
    return row;
}

void BlockModel::insertAt(int row, const Block &block)
{
    beginInsertRows(QModelIndex(), row, row);
    m_blocks.insert(row, block);
    m_numbers.insert(row, 0);
    m_hidden.insert(row, false);
    m_hasChildren.insert(row, false);
    endInsertRows();
    recompute();
    emit countChanged();
    emit edited();
}

void BlockModel::removeAt(int row)
{
    if (!valid(row))
        return;
    beginRemoveRows(QModelIndex(), row, row);
    m_blocks.removeAt(row);
    m_numbers.removeAt(row);
    m_hidden.removeAt(row);
    m_hasChildren.removeAt(row);
    endRemoveRows();
    if (m_blocks.isEmpty())
        insertAt(0, Block());
    recompute();
    emit countChanged();
    emit edited();
}

int BlockModel::subtreeEnd(int row) const
{
    int end = row + 1;
    while (end < m_blocks.size() && m_blocks.at(end).indent > m_blocks.at(row).indent)
        ++end;
    return end;
}

bool BlockModel::changeIndent(int row, int delta)
{
    if (!valid(row) || delta == 0)
        return false;
    const int indent = m_blocks.at(row).indent;
    if (delta > 0 && (row == 0 || indent > m_blocks.at(row - 1).indent || indent >= kMaxIndent))
        return false;
    if (delta < 0 && indent == 0)
        return false;
    const int end = subtreeEnd(row);
    for (int i = row; i < end; ++i) {
        m_blocks[i].indent = qBound(0, m_blocks[i].indent + (delta > 0 ? 1 : -1), kMaxIndent);
        changed(i, {IndentRole});
    }
    recompute();
    emit edited();
    return true;
}

int BlockModel::moveBlock(int row, int delta)
{
    if (!valid(row) || delta == 0)
        return row;
    const int end = subtreeEnd(row);
    const int len = end - row;
    const int indent = m_blocks.at(row).indent;
    if (delta < 0) {
        if (row == 0)
            return row;
        int dest = row - 1;
        while (dest > 0 && m_blocks.at(dest).indent > indent)
            --dest;
        beginMoveRows(QModelIndex(), row, end - 1, QModelIndex(), dest);
        const auto moved = m_blocks.mid(row, len);
        m_blocks.remove(row, len);
        for (int i = 0; i < len; ++i)
            m_blocks.insert(dest + i, moved.at(i));
        endMoveRows();
        recompute();
        emit edited();
        return dest;
    }
    if (end >= m_blocks.size())
        return row;
    const int nextEnd = m_blocks.at(end).indent >= indent ? subtreeEnd(end) : end + 1;
    beginMoveRows(QModelIndex(), row, end - 1, QModelIndex(), nextEnd);
    const auto moved = m_blocks.mid(row, len);
    m_blocks.remove(row, len);
    const int dest = nextEnd - len;
    for (int i = 0; i < len; ++i)
        m_blocks.insert(dest + i, moved.at(i));
    endMoveRows();
    recompute();
    emit edited();
    return dest;
}

int BlockModel::duplicateBlock(int row)
{
    if (!valid(row))
        return row;
    const int end = subtreeEnd(row);
    const auto copies = m_blocks.mid(row, end - row);
    for (int i = 0; i < copies.size(); ++i)
        insertAt(end + i, copies.at(i));
    return end;
}

void BlockModel::setChecked(int row, bool checked)
{
    if (!valid(row) || m_blocks.at(row).checked == checked)
        return;
    m_blocks[row].checked = checked;
    changed(row, {CheckedRole});
    emit edited();
}

void BlockModel::setCollapsed(int row, bool collapsed)
{
    if (!valid(row) || m_blocks.at(row).collapsed == collapsed)
        return;
    m_blocks[row].collapsed = collapsed;
    changed(row, {CollapsedRole});
    recompute();
    emit edited();
}

void BlockModel::setEmoji(int row, const QString &emoji)
{
    if (!valid(row))
        return;
    m_blocks[row].emoji = emoji.trimmed().left(16);
    changed(row, {EmojiRole});
    emit edited();
}

void BlockModel::setLanguage(int row, const QString &language)
{
    if (!valid(row))
        return;
    m_blocks[row].language = language.trimmed().left(32);
    changed(row, {LanguageRole});
    emit edited();
}

void BlockModel::setRef(int row, const QString &id, const QString &label)
{
    if (!valid(row))
        return;
    Block &b = m_blocks[row];
    b.type = QStringLiteral("page");
    b.ref = id;
    b.text = label;
    changed(row, {TypeRole, RefRole, TextRole, HtmlRole, IconRole});
    recompute();
    emit edited();
}

int BlockModel::previousEditable(int row) const
{
    for (int i = row - 1; i >= 0; --i)
        if (!m_hidden.value(i) && Notes::isTextual(m_blocks.at(i).type))
            return i;
    return -1;
}

int BlockModel::nextEditable(int row) const
{
    for (int i = row + 1; i < m_blocks.size(); ++i)
        if (!m_hidden.value(i) && Notes::isTextual(m_blocks.at(i).type))
            return i;
    return -1;
}

void BlockModel::recompute()
{
    const int n = int(m_blocks.size());
    QList<int> numbers(n, 0);
    QList<bool> hidden(n, false), hasChildren(n, false);
    QList<int> lastNumber(kMaxIndent + 1, 0);
    int collapsedAt = -1;
    for (int i = 0; i < n; ++i) {
        const Block &b = m_blocks.at(i);
        const int ind = b.indent;
        for (int k = ind + 1; k <= kMaxIndent; ++k)
            lastNumber[k] = 0;
        numbers[i] = b.type == QLatin1String("numbered") ? lastNumber[ind] + 1 : 0;
        lastNumber[ind] = numbers[i];
        if (collapsedAt >= 0 && ind > collapsedAt) {
            hidden[i] = true;
        } else {
            collapsedAt = -1;
            if (b.type == QLatin1String("toggle") && b.collapsed)
                collapsedAt = ind;
        }
        hasChildren[i] = i + 1 < n && m_blocks.at(i + 1).indent > ind;
    }
    const bool sameSize = m_numbers.size() == n && m_hidden.size() == n && m_hasChildren.size() == n;
    const auto oldNumbers = m_numbers;
    const auto oldHidden = m_hidden;
    const auto oldChildren = m_hasChildren;
    m_numbers = numbers;
    m_hidden = hidden;
    m_hasChildren = hasChildren;
    for (int i = 0; i < n; ++i) {
        QList<int> roles;
        if (!sameSize || oldNumbers.at(i) != numbers.at(i))
            roles << NumberRole;
        if (!sameSize || oldHidden.at(i) != hidden.at(i))
            roles << HiddenRole;
        if (!sameSize || oldChildren.at(i) != hasChildren.at(i))
            roles << HasChildrenRole;
        if (!roles.isEmpty())
            changed(i, roles);
    }
}

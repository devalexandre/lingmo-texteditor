#include "notestore.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLocale>
#include <QSaveFile>
#include <QStandardPaths>
#include <QUuid>
#include <algorithm>
#include <functional>

using Notes::PageMeta;

namespace {
QString readFile(const QString &path, bool *ok = nullptr)
{
    QFile input(path);
    const bool opened = input.open(QIODevice::ReadOnly);
    if (ok)
        *ok = opened;
    return opened ? QString::fromUtf8(input.readAll()) : QString();
}

QString displayTitle(const PageMeta &meta)
{
    return meta.title.trimmed().isEmpty() ? QObject::tr("Sem título") : meta.title;
}
} // namespace

NoteStore::NoteStore(const QString &directory, QObject *parent)
    : QObject(parent)
    , m_directory(directory)
{
    QDir().mkpath(directory);
    m_saveTimer.setSingleShot(true);
    m_saveTimer.setInterval(600);
    connect(&m_saveTimer, &QTimer::timeout, this, &NoteStore::save);
    m_blocks.pageInfo = [this](const QString &id) {
        const auto it = m_pages.constFind(id);
        if (it == m_pages.constEnd())
            return qMakePair(QString(), QString());
        return qMakePair(displayTitle(*it), it->icon);
    };
    connect(&m_blocks, &BlockModel::edited, this, [this] {
        if (!m_loading && !m_currentId.isEmpty()) {
            markDirty();
            updateWordCount();
        }
    });
    load();
    const auto recent = recentPages();
    if (!recent.isEmpty())
        open(recent.first().toMap().value(QStringLiteral("id")).toString());
}

NoteStore::~NoteStore()
{
    if (m_dirty)
        save();
}

QString NoteStore::pagePath(const QString &id) const { return m_directory + QLatin1Char('/') + id + QStringLiteral(".md"); }
QString NoteStore::trashPath(const QString &id) const { return m_directory + QStringLiteral("/Trash/") + id + QStringLiteral(".md"); }

void NoteStore::load()
{
    auto scan = [](const QString &dir, QHash<QString, PageMeta> &into, QHash<QString, QString> *content) {
        const auto files = QDir(dir).entryInfoList({QStringLiteral("*.md")}, QDir::Files);
        for (const QFileInfo &file : files) {
            const QString id = file.completeBaseName();
            if (!Notes::isValidId(id))
                continue;
            bool ok = false;
            const QString text = readFile(file.absoluteFilePath(), &ok);
            if (!ok)
                continue;
            Notes::PageFile page = Notes::parsePage(text);
            PageMeta meta = page.meta;
            meta.id = id; // the file name is the identity
            if (!meta.parent.isEmpty() && !Notes::isValidId(meta.parent))
                meta.parent.clear();
            if (!page.hasFrontmatter || !meta.updated.isValid()) {
                // Legacy "# Title\n\nbody" note: a root page dated by the file.
                meta.updated = file.lastModified();
            }
            if (!meta.created.isValid())
                meta.created = file.birthTime().isValid() ? file.birthTime() : file.lastModified();
            into.insert(id, meta);
            if (content)
                content->insert(id, page.body);
        }
    };
    m_pages.clear();
    m_trash.clear();
    m_content.clear();
    scan(m_directory, m_pages, &m_content);
    scan(m_directory + QStringLiteral("/Trash"), m_trash, nullptr);
    for (auto it = m_trash.begin(); it != m_trash.end();) // never both live and trashed
        it = m_pages.contains(it.key()) ? m_trash.erase(it) : std::next(it);
    rebuildTree();
    emit pagesChanged();
    emit trashChanged();
}

QString NoteStore::effectiveParent(const QString &id) const
{
    const QString parent = m_pages.value(id).parent;
    if (parent.isEmpty() || parent == id || !m_pages.contains(parent))
        return QString();
    // A cycle written by hand: treat the page as a root.
    QSet<QString> seen{id};
    QString p = parent;
    while (!p.isEmpty()) {
        if (seen.contains(p))
            return QString();
        seen.insert(p);
        const QString next = m_pages.value(p).parent;
        p = m_pages.contains(next) ? next : QString();
    }
    return parent;
}

QStringList NoteStore::childrenOf(const QString &parentId) const
{
    QStringList ids;
    for (auto it = m_pages.constBegin(); it != m_pages.constEnd(); ++it)
        if (effectiveParent(it.key()) == parentId)
            ids << it.key();
    std::sort(ids.begin(), ids.end(), [this](const QString &a, const QString &b) {
        const PageMeta &x = m_pages[a], &y = m_pages[b];
        if (x.order != y.order)
            return x.order < y.order;
        if (x.created != y.created)
            return x.created < y.created;
        return a < b;
    });
    return ids;
}

QStringList NoteStore::subtree(const QString &id) const
{
    QStringList result{id};
    for (int i = 0; i < result.size(); ++i)
        result << childrenOf(result.at(i));
    return result;
}

QStringList NoteStore::trashSubtree(const QString &id) const
{
    QStringList result{id};
    for (int i = 0; i < result.size(); ++i)
        for (auto it = m_trash.constBegin(); it != m_trash.constEnd(); ++it)
            if (it->parent == result.at(i) && !result.contains(it.key()))
                result << it.key();
    return result;
}

QStringList NoteStore::ancestors(const QString &id) const
{
    QStringList chain;
    QString p = effectiveParent(id);
    while (!p.isEmpty() && !chain.contains(p)) {
        chain.prepend(p);
        p = effectiveParent(p);
    }
    return chain;
}

QStringList NoteStore::orderedIds() const
{
    QStringList ids;
    std::function<void(const QString &)> walk = [&](const QString &parent) {
        for (const QString &id : childrenOf(parent)) {
            ids << id;
            walk(id);
        }
    };
    walk(QString());
    return ids;
}

int NoteStore::nextOrder(const QString &parentId) const
{
    int order = 0;
    for (const QString &id : childrenOf(parentId))
        order = qMax(order, m_pages.value(id).order + 1);
    return order;
}

void NoteStore::rebuildTree()
{
    QList<PageTreeModel::Row> rows;
    std::function<void(const QString &, int)> walk = [&](const QString &parent, int depth) {
        for (const QString &id : childrenOf(parent)) {
            const PageMeta &meta = m_pages[id];
            PageTreeModel::Row row;
            row.id = id;
            row.title = displayTitle(meta);
            row.icon = meta.icon;
            row.depth = depth;
            row.hasChildren = !childrenOf(id).isEmpty();
            row.expanded = m_expanded.contains(id);
            row.favorite = meta.favorite;
            rows << row;
            if (row.expanded)
                walk(id, depth + 1);
        }
    };
    walk(QString(), 0);
    m_tree.setRows(rows);
    m_tree.setCurrent(m_currentId);
}

void NoteStore::setStatus(const QString &status)
{
    if (m_status == status)
        return;
    m_status = status;
    emit statusChanged();
}

void NoteStore::markDirty()
{
    m_dirty = true;
    setStatus(tr("Salvando…"));
    m_saveTimer.start();
}

bool NoteStore::writeText(const QString &path, const QString &text)
{
    QSaveFile output(path);
    const QByteArray data = text.toUtf8();
    if (!output.open(QIODevice::WriteOnly) || output.write(data) != data.size() || !output.commit()) {
        setStatus(tr("Falha ao salvar: %1").arg(output.errorString()));
        return false;
    }
    return true;
}

bool NoteStore::writeMeta(const QString &id)
{
    if (id == m_currentId) {
        m_dirty = true;
        return save();
    }
    bool ok = false;
    const QString text = readFile(pagePath(id), &ok);
    if (!ok) {
        setStatus(tr("Falha ao abrir a página."));
        return false;
    }
    const Notes::PageFile page = Notes::parsePage(text);
    if (!writeText(pagePath(id), Notes::serializePage(m_pages.value(id), page.body)))
        return false;
    m_content.insert(id, page.body);
    return true;
}

bool NoteStore::save()
{
    m_saveTimer.stop();
    if (!m_dirty || m_currentId.isEmpty())
        return true;
    PageMeta meta = m_pages.value(m_currentId);
    meta.updated = QDateTime::currentDateTime();
    const QString body = m_blocks.markdown();
    if (!writeText(pagePath(m_currentId), Notes::serializePage(meta, body)))
        return false;
    m_pages.insert(m_currentId, meta);
    m_content.insert(m_currentId, body);
    m_dirty = false;
    setStatus(tr("Salvo"));
    m_tree.updatePage(m_currentId, displayTitle(meta), meta.icon, meta.favorite);
    emit metaChanged();
    emit pagesChanged();
    return true;
}

QString NoteStore::createPage(const QString &parentId)
{
    if (!parentId.isEmpty() && !m_pages.contains(parentId))
        return QString();
    PageMeta meta;
    meta.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    meta.parent = parentId;
    meta.order = nextOrder(parentId);
    meta.created = meta.updated = QDateTime::currentDateTime();
    if (!writeText(pagePath(meta.id), Notes::serializePage(meta, QString())))
        return QString();
    m_pages.insert(meta.id, meta);
    m_content.insert(meta.id, QString());
    if (!parentId.isEmpty())
        m_expanded.insert(parentId);
    return meta.id;
}

static constexpr qsizetype MaxPageBytes = 2 * 1024 * 1024;

QString NoteStore::createPageWithContent(const QString &title, const QString &markdown, const QString &parentId)
{
    if (!save())
        return QString();
    if ((!parentId.isEmpty() && !m_pages.contains(parentId)) || markdown.toUtf8().size() > MaxPageBytes)
        return QString();
    PageMeta meta;
    meta.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    meta.parent = parentId;
    meta.title = QString(title).replace(QLatin1Char('\n'), QLatin1Char(' ')).replace(QLatin1Char('\r'), QLatin1Char(' ')).trimmed();
    meta.order = nextOrder(parentId);
    meta.created = meta.updated = QDateTime::currentDateTime();
    if (!writeText(pagePath(meta.id), Notes::serializePage(meta, markdown)))
        return QString();
    m_pages.insert(meta.id, meta);
    m_content.insert(meta.id, markdown);
    rebuildTree();
    emit pagesChanged();
    return meta.id;
}

bool NoteStore::appendToPage(const QString &id, const QString &markdown)
{
    if (!Notes::isValidId(id) || !m_pages.contains(id) || !save())
        return false;
    bool ok = false;
    const QString text = readFile(pagePath(id), &ok);
    if (!ok)
        return false;
    Notes::PageFile page = Notes::parsePage(text);
    QString body = page.body;
    if (!body.trimmed().isEmpty() && !body.endsWith(QLatin1Char('\n')))
        body += QLatin1Char('\n');
    if (!body.trimmed().isEmpty())
        body += QLatin1Char('\n');
    body += markdown.trimmed() + QLatin1Char('\n');
    if (body.toUtf8().size() > MaxPageBytes)
        return false;
    PageMeta meta = m_pages.value(id);
    meta.title = page.meta.title.isEmpty() ? meta.title : page.meta.title;
    meta.updated = QDateTime::currentDateTime();
    if (!writeText(pagePath(id), Notes::serializePage(meta, body)))
        return false;
    m_pages.insert(id, meta);
    m_content.insert(id, body);
    if (id == m_currentId) {
        m_loading = true;
        m_blocks.setBlocks(Notes::parseBlocks(body));
        m_loading = false;
        m_dirty = false;
        updateWordCount();
    }
    emit pagesChanged();
    return true;
}

QString NoteStore::findPage(const QString &ref) const
{
    const QString r = ref.trimmed();
    if (Notes::isValidId(r) && m_pages.contains(r))
        return r;
    for (const QString &id : orderedIds())
        if (m_pages.value(id).title.trimmed().compare(r, Qt::CaseInsensitive) == 0)
            return id;
    return QString();
}

QString NoteStore::create(const QString &parentId)
{
    if (!save())
        return QString();
    const QString id = createPage(parentId);
    if (id.isEmpty())
        return QString();
    rebuildTree();
    emit pagesChanged();
    open(id);
    return id;
}

bool NoteStore::open(const QString &id)
{
    // IDs are generated locally; never accept a path from the UI.
    if (!Notes::isValidId(id) || !m_pages.contains(id))
        return false;
    if (!save())
        return false;
    bool ok = false;
    const QString text = readFile(pagePath(id), &ok);
    if (!ok) {
        setStatus(tr("Falha ao abrir a página."));
        return false;
    }
    const Notes::PageFile page = Notes::parsePage(text);
    PageMeta &meta = m_pages[id];
    meta.title = page.meta.title; // the file wins over the cached copy
    m_content.insert(id, page.body);
    m_loading = true;
    m_currentId = id;
    m_blocks.setBlocks(Notes::parseBlocks(page.body));
    m_loading = false;
    m_dirty = false;
    for (const QString &a : ancestors(id))
        m_expanded.insert(a);
    rebuildTree();
    updateWordCount();
    emit currentChanged();
    emit titleChanged();
    emit metaChanged();
    emit breadcrumbsChanged();
    setStatus(tr("Salvo"));
    return true;
}

void NoteStore::clearCurrent()
{
    m_saveTimer.stop();
    m_dirty = false;
    m_currentId.clear();
    m_loading = true;
    m_blocks.setBlocks({});
    m_loading = false;
    m_tree.setCurrent(QString());
    updateWordCount();
    emit currentChanged();
    emit titleChanged();
    emit metaChanged();
    emit breadcrumbsChanged();
}

QString NoteStore::title() const { return m_pages.value(m_currentId).title; }
QString NoteStore::icon() const { return m_pages.value(m_currentId).icon; }
bool NoteStore::favorite() const { return m_pages.value(m_currentId).favorite; }

QString NoteStore::updatedText() const
{
    if (m_currentId.isEmpty())
        return QString();
    return tr("Editado em %1").arg(QLocale().toString(m_pages.value(m_currentId).updated, QLocale::ShortFormat));
}

void NoteStore::setTitle(const QString &title)
{
    if (m_currentId.isEmpty())
        return;
    QString clean = title;
    clean.replace(QLatin1Char('\n'), QLatin1Char(' ')).replace(QLatin1Char('\r'), QLatin1Char(' '));
    PageMeta &meta = m_pages[m_currentId];
    if (meta.title == clean)
        return;
    meta.title = clean;
    m_tree.updatePage(m_currentId, displayTitle(meta), meta.icon, meta.favorite);
    emit titleChanged();
    emit breadcrumbsChanged();
    markDirty();
}

bool NoteStore::rename(const QString &id, const QString &title)
{
    if (!m_pages.contains(id))
        return false;
    if (id == m_currentId) {
        setTitle(title);
        return save();
    }
    m_pages[id].title = QString(title).replace(QLatin1Char('\n'), QLatin1Char(' '));
    if (!writeMeta(id))
        return false;
    rebuildTree();
    emit breadcrumbsChanged();
    emit pagesChanged();
    return true;
}

bool NoteStore::setIcon(const QString &id, const QString &icon)
{
    if (!m_pages.contains(id))
        return false;
    m_pages[id].icon = QString(icon).remove(QLatin1Char('\n')).remove(QLatin1Char('\r')).trimmed().left(16);
    if (!writeMeta(id))
        return false;
    rebuildTree();
    m_blocks.refreshPageLinks();
    emit metaChanged();
    emit breadcrumbsChanged();
    emit pagesChanged();
    return true;
}

bool NoteStore::setFavorite(const QString &id, bool favorite)
{
    if (!m_pages.contains(id))
        return false;
    m_pages[id].favorite = favorite;
    if (!writeMeta(id))
        return false;
    rebuildTree();
    emit metaChanged();
    emit pagesChanged();
    return true;
}

QString NoteStore::duplicate(const QString &id)
{
    if (!m_pages.contains(id) || !save())
        return QString();
    bool ok = false;
    const QString text = readFile(pagePath(id), &ok);
    if (!ok)
        return QString();
    const Notes::PageFile page = Notes::parsePage(text);
    const PageMeta original = m_pages.value(id);
    const QString parent = effectiveParent(id);
    for (const QString &sibling : childrenOf(parent)) {
        if (m_pages.value(sibling).order > original.order) {
            m_pages[sibling].order += 1;
            writeMeta(sibling);
        }
    }
    PageMeta copy = original;
    copy.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    copy.parent = parent;
    copy.title = tr("%1 (cópia)").arg(displayTitle(original));
    copy.favorite = false;
    copy.order = original.order + 1;
    copy.created = copy.updated = QDateTime::currentDateTime();
    if (!writeText(pagePath(copy.id), Notes::serializePage(copy, page.body)))
        return QString();
    m_pages.insert(copy.id, copy);
    m_content.insert(copy.id, page.body);
    rebuildTree();
    emit pagesChanged();
    setStatus(tr("Página duplicada"));
    return copy.id;
}

bool NoteStore::move(const QString &id, const QString &parentId)
{
    if (!m_pages.contains(id) || (!parentId.isEmpty() && !m_pages.contains(parentId)))
        return false;
    if (subtree(id).contains(parentId))
        return false; // into itself or a descendant
    if (effectiveParent(id) == parentId)
        return true;
    m_pages[id].parent = parentId;
    m_pages[id].order = nextOrder(parentId);
    if (!writeMeta(id))
        return false;
    if (!parentId.isEmpty())
        m_expanded.insert(parentId);
    rebuildTree();
    emit breadcrumbsChanged();
    emit pagesChanged();
    setStatus(tr("Página movida"));
    return true;
}

bool NoteStore::moveSibling(const QString &id, int delta)
{
    if (!m_pages.contains(id) || delta == 0)
        return false;
    QStringList siblings = childrenOf(effectiveParent(id));
    const int from = int(siblings.indexOf(id));
    const int to = from + (delta > 0 ? 1 : -1);
    if (to < 0 || to >= siblings.size())
        return false;
    siblings.move(from, to);
    for (int i = 0; i < siblings.size(); ++i) {
        if (m_pages.value(siblings.at(i)).order != i) {
            m_pages[siblings.at(i)].order = i;
            if (!writeMeta(siblings.at(i)))
                return false;
        }
    }
    rebuildTree();
    return true;
}

QVariantList NoteStore::moveTargets(const QString &id) const
{
    const QStringList excluded = m_pages.contains(id) ? subtree(id) : QStringList();
    QVariantList result;
    for (const QString &page : orderedIds()) {
        if (excluded.contains(page))
            continue;
        result << QVariantMap{{QStringLiteral("id"), page},
                              {QStringLiteral("title"), displayTitle(m_pages.value(page))},
                              {QStringLiteral("icon"), m_pages.value(page).icon},
                              {QStringLiteral("depth"), int(ancestors(page).size())}};
    }
    return result;
}

void NoteStore::toggleExpanded(const QString &id)
{
    if (m_expanded.contains(id))
        m_expanded.remove(id);
    else if (m_pages.contains(id))
        m_expanded.insert(id);
    rebuildTree();
}

QString NoteStore::pageTitle(const QString &id) const
{
    return m_pages.contains(id) ? displayTitle(m_pages.value(id)) : QString();
}

QString NoteStore::pageIcon(const QString &id) const { return m_pages.value(id).icon; }
QString NoteStore::parentOf(const QString &id) const { return effectiveParent(id); }

QString NoteStore::addSubpageBlock(int row)
{
    if (m_currentId.isEmpty() || row < 0 || row >= m_blocks.count())
        return QString();
    const QString id = createPage(m_currentId);
    if (id.isEmpty())
        return QString();
    int target = row;
    if (!(Notes::isTextual(m_blocks.typeAt(row)) && m_blocks.textAt(row).isEmpty()))
        target = m_blocks.insertBlock(row + 1, QStringLiteral("paragraph"), m_blocks.indentAt(row));
    m_blocks.setRef(target, id, tr("Sem título"));
    rebuildTree();
    emit pagesChanged();
    if (!save())
        return QString();
    return id;
}

QVariantList NoteStore::breadcrumbs() const
{
    QVariantList result;
    if (m_currentId.isEmpty())
        return result;
    QStringList chain = ancestors(m_currentId);
    chain << m_currentId;
    for (const QString &id : chain)
        result << QVariantMap{{QStringLiteral("id"), id},
                              {QStringLiteral("title"), displayTitle(m_pages.value(id))},
                              {QStringLiteral("icon"), m_pages.value(id).icon}};
    return result;
}

QVariantList NoteStore::favorites() const
{
    QVariantList result;
    for (const QString &id : orderedIds()) {
        const PageMeta &meta = m_pages[id];
        if (meta.favorite)
            result << QVariantMap{{QStringLiteral("id"), id}, {QStringLiteral("title"), displayTitle(meta)},
                                  {QStringLiteral("icon"), meta.icon}};
    }
    return result;
}

QVariantList NoteStore::recentPages() const
{
    QList<PageMeta> pages = m_pages.values();
    std::sort(pages.begin(), pages.end(), [](const PageMeta &a, const PageMeta &b) { return a.updated > b.updated; });
    QVariantList result;
    for (const PageMeta &meta : pages) {
        if (result.size() >= 8)
            break;
        QStringList path;
        for (const QString &a : ancestors(meta.id))
            path << displayTitle(m_pages.value(a));
        result << QVariantMap{{QStringLiteral("id"), meta.id}, {QStringLiteral("title"), displayTitle(meta)},
                              {QStringLiteral("icon"), meta.icon}, {QStringLiteral("path"), path.join(QStringLiteral(" / "))},
                              {QStringLiteral("snippet"), QString()}};
    }
    return result;
}

QVariantList NoteStore::trashPages() const
{
    QList<PageMeta> pages;
    for (const PageMeta &meta : m_trash)
        if (!m_trash.contains(meta.parent))
            pages << meta;
    std::sort(pages.begin(), pages.end(), [](const PageMeta &a, const PageMeta &b) { return a.updated > b.updated; });
    QVariantList result;
    for (const PageMeta &meta : pages)
        result << QVariantMap{{QStringLiteral("id"), meta.id}, {QStringLiteral("title"), displayTitle(meta)},
                              {QStringLiteral("icon"), meta.icon},
                              {QStringLiteral("children"), int(trashSubtree(meta.id).size()) - 1},
                              {QStringLiteral("parentTitle"), pageTitle(meta.parent)}};
    return result;
}

QVariantList NoteStore::search(const QString &query) const
{
    const QString q = query.trimmed();
    if (q.isEmpty())
        return recentPages();
    struct Hit { int score; QVariantMap item; QDateTime updated; };
    QList<Hit> hits;
    for (auto it = m_pages.constBegin(); it != m_pages.constEnd(); ++it) {
        const PageMeta &meta = it.value();
        const QString title = displayTitle(meta);
        const QString body = m_content.value(it.key());
        int score = 0;
        QString snippet;
        if (title.startsWith(q, Qt::CaseInsensitive))
            score = 3;
        else if (title.contains(q, Qt::CaseInsensitive))
            score = 2;
        const int at = int(body.indexOf(q, 0, Qt::CaseInsensitive));
        if (at >= 0) {
            score = qMax(score, 1);
            const int start = qMax(0, at - 30);
            snippet = (start > 0 ? QStringLiteral("…") : QString())
                + body.mid(start, 100).simplified() + (start + 100 < body.size() ? QStringLiteral("…") : QString());
        }
        if (score == 0)
            continue;
        QStringList path;
        for (const QString &a : ancestors(meta.id))
            path << displayTitle(m_pages.value(a));
        hits << Hit{score,
                    {{QStringLiteral("id"), meta.id}, {QStringLiteral("title"), title}, {QStringLiteral("icon"), meta.icon},
                     {QStringLiteral("path"), path.join(QStringLiteral(" / "))}, {QStringLiteral("snippet"), snippet}},
                    meta.updated};
    }
    std::sort(hits.begin(), hits.end(), [](const Hit &a, const Hit &b) {
        return a.score != b.score ? a.score > b.score : a.updated > b.updated;
    });
    QVariantList result;
    for (const Hit &h : hits) {
        if (result.size() >= 30)
            break;
        result << h.item;
    }
    return result;
}

QString NoteStore::exportCurrent()
{
    if (m_currentId.isEmpty() || !save())
        return QString();
    QString dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if (dir.isEmpty())
        dir = QDir::homePath();
    QDir().mkpath(dir);
    QString name = displayTitle(m_pages.value(m_currentId));
    for (QChar &c : name)
        if (c.category() == QChar::Other_Control || QStringLiteral("/\\:*?\"<>|").contains(c))
            c = QLatin1Char('-');
    name = name.trimmed();
    while (name.startsWith(QLatin1Char('.')))
        name.remove(0, 1);
    if (name.isEmpty())
        name = tr("Sem título");
    name = name.left(80);
    QString path = dir + QLatin1Char('/') + name + QStringLiteral(".md");
    for (int n = 2; QFile::exists(path); ++n)
        path = dir + QLatin1Char('/') + name + QStringLiteral(" (%1).md").arg(n);
    const QString body = m_blocks.markdown();
    QString text = QStringLiteral("# ") + displayTitle(m_pages.value(m_currentId)) + QLatin1Char('\n');
    if (!body.isEmpty())
        text += QLatin1Char('\n') + body;
    if (!writeText(path, text))
        return QString();
    setStatus(tr("Exportado para %1").arg(path));
    return path;
}

bool NoteStore::trash(const QString &id)
{
    if (!m_pages.contains(id))
        return false;
    const QStringList ids = subtree(id);
    const bool closesCurrent = ids.contains(m_currentId);
    if (closesCurrent && !save())
        return false;
    const QString fallback = effectiveParent(id);
    QDir().mkpath(m_directory + QStringLiteral("/Trash"));
    for (const QString &page : ids) {
        if (QFile::exists(trashPath(page)))
            QFile::remove(trashPath(page));
        if (!QFile::rename(pagePath(page), trashPath(page))) {
            setStatus(tr("Não foi possível mover a página para a lixeira."));
            load(); // resync with whatever did move
            return false;
        }
        PageMeta meta = m_pages.take(page);
        m_trash.insert(page, meta);
        m_content.remove(page);
        m_expanded.remove(page);
    }
    if (closesCurrent) {
        clearCurrent();
        if (!fallback.isEmpty())
            open(fallback);
        else if (!m_pages.isEmpty())
            open(recentPages().first().toMap().value(QStringLiteral("id")).toString());
    }
    rebuildTree();
    emit breadcrumbsChanged();
    emit pagesChanged();
    emit trashChanged();
    setStatus(tr("Página movida para a lixeira"));
    return true;
}

bool NoteStore::restore(const QString &id)
{
    if (!Notes::isValidId(id) || !m_trash.contains(id))
        return false;
    const QStringList ids = trashSubtree(id);
    for (const QString &page : ids) {
        if (QFile::exists(pagePath(page)) || !QFile::rename(trashPath(page), pagePath(page))) {
            setStatus(tr("Não foi possível restaurar a página."));
            load();
            return false;
        }
        m_pages.insert(page, m_trash.take(page));
    }
    for (const QString &page : ids)
        m_content.insert(page, Notes::parsePage(readFile(pagePath(page))).body);
    if (!m_pages[id].parent.isEmpty() && effectiveParent(id).isEmpty()) {
        // The parent is gone (or still in the trash): restore at the top level.
        m_pages[id].parent.clear();
        m_pages[id].order = nextOrder(QString());
        writeMeta(id);
    }
    rebuildTree();
    emit pagesChanged();
    emit trashChanged();
    setStatus(tr("Página restaurada"));
    return true;
}

bool NoteStore::deleteForever(const QString &id)
{
    if (!Notes::isValidId(id) || !m_trash.contains(id))
        return false;
    for (const QString &page : trashSubtree(id)) {
        if (QFile::exists(trashPath(page)) && !QFile::remove(trashPath(page))) {
            setStatus(tr("Não foi possível excluir a página."));
            return false;
        }
        m_trash.remove(page);
    }
    emit trashChanged();
    setStatus(tr("Página excluída permanentemente"));
    return true;
}

void NoteStore::updateWordCount()
{
    const int count = Notes::wordCount(Notes::plainText(m_blocks.blocks()));
    if (count != m_wordCount) {
        m_wordCount = count;
        emit wordCountChanged();
    }
}

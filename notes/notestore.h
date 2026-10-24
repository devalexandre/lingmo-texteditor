#pragma once

#include "blockmodel.h"
#include "markdown.h"
#include "pagetreemodel.h"

#include <QHash>
#include <QObject>
#include <QSet>
#include <QTimer>
#include <QVariantList>

// Pages live as one Markdown file each (<uuid>.md) in the notes directory, with
// a small frontmatter for metadata; trashed pages move to Trash/. Page ids are
// always UUIDs generated here: nothing coming from QML is ever used as a path.
class NoteStore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(PageTreeModel *tree READ tree CONSTANT)
    Q_PROPERTY(BlockModel *blocks READ blocks CONSTANT)
    Q_PROPERTY(QString currentId READ currentId NOTIFY currentChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString icon READ icon NOTIFY metaChanged)
    Q_PROPERTY(bool favorite READ favorite NOTIFY metaChanged)
    Q_PROPERTY(QString updatedText READ updatedText NOTIFY metaChanged)
    Q_PROPERTY(QVariantList breadcrumbs READ breadcrumbs NOTIFY breadcrumbsChanged)
    Q_PROPERTY(QVariantList favorites READ favorites NOTIFY pagesChanged)
    Q_PROPERTY(QVariantList recentPages READ recentPages NOTIFY pagesChanged)
    Q_PROPERTY(QVariantList trashPages READ trashPages NOTIFY trashChanged)
    Q_PROPERTY(int pageCount READ pageCount NOTIFY pagesChanged)
    Q_PROPERTY(int wordCount READ wordCount NOTIFY wordCountChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
public:
    explicit NoteStore(const QString &directory, QObject *parent = nullptr);
    ~NoteStore() override;

    PageTreeModel *tree() { return &m_tree; }
    BlockModel *blocks() { return &m_blocks; }
    QString currentId() const { return m_currentId; }
    QString title() const;
    QString icon() const;
    bool favorite() const;
    QString updatedText() const;
    QVariantList breadcrumbs() const;
    QVariantList favorites() const;
    QVariantList recentPages() const;
    QVariantList trashPages() const;
    int pageCount() const { return int(m_pages.size()); }
    int wordCount() const { return m_wordCount; }
    QString status() const { return m_status; }

    // Pages
    Q_INVOKABLE QString create(const QString &parentId = QString());
    Q_INVOKABLE bool open(const QString &id);
    Q_INVOKABLE bool save();
    Q_INVOKABLE void setTitle(const QString &title);
    Q_INVOKABLE bool rename(const QString &id, const QString &title);
    Q_INVOKABLE bool setIcon(const QString &id, const QString &icon);
    Q_INVOKABLE bool setFavorite(const QString &id, bool favorite);
    Q_INVOKABLE QString duplicate(const QString &id);
    Q_INVOKABLE bool move(const QString &id, const QString &parentId);
    Q_INVOKABLE bool moveSibling(const QString &id, int delta);
    Q_INVOKABLE QVariantList moveTargets(const QString &id) const;
    Q_INVOKABLE void toggleExpanded(const QString &id);
    Q_INVOKABLE QString pageTitle(const QString &id) const;
    Q_INVOKABLE QString pageIcon(const QString &id) const;
    Q_INVOKABLE QString parentOf(const QString &id) const;
    Q_INVOKABLE bool exists(const QString &id) const { return m_pages.contains(id); }
    // Creates a child of the current page and links it from block `row`.
    Q_INVOKABLE QString addSubpageBlock(int row);
    Q_INVOKABLE QVariantList search(const QString &query) const;
    Q_INVOKABLE QString exportCurrent();

    // Trash
    Q_INVOKABLE bool trash(const QString &id);
    Q_INVOKABLE bool restore(const QString &id);
    Q_INVOKABLE bool deleteForever(const QString &id);

    // Tests and helpers
    QString currentMarkdown() const { return m_blocks.markdown(); }

    // Lingmo Intents (notes.create / notes.append): work on the files without
    // opening the page in the editor. Empty id / false on failure (status set).
    QString createPageWithContent(const QString &title, const QString &markdown, const QString &parentId = QString());
    bool appendToPage(const QString &id, const QString &markdown);
    // A page id, or the first page whose title is `ref` (case-insensitive)
    QString findPage(const QString &ref) const;
    QString directory() const { return m_directory; }

signals:
    void currentChanged();
    void titleChanged();
    void metaChanged();
    void breadcrumbsChanged();
    void pagesChanged();
    void trashChanged();
    void wordCountChanged();
    void statusChanged();

private:
    void load();
    void rebuildTree();
    void setStatus(const QString &status);
    void markDirty();
    void clearCurrent();
    bool writeText(const QString &path, const QString &text);
    bool writeMeta(const QString &id);
    QString pagePath(const QString &id) const;
    QString trashPath(const QString &id) const;
    QString effectiveParent(const QString &id) const;
    QStringList childrenOf(const QString &parentId) const;
    QStringList subtree(const QString &id) const;
    QStringList trashSubtree(const QString &id) const;
    QStringList ancestors(const QString &id) const;
    QStringList orderedIds() const;
    int nextOrder(const QString &parentId) const;
    QString createPage(const QString &parentId);
    void updateWordCount();

    QString m_directory;
    QHash<QString, Notes::PageMeta> m_pages;
    QHash<QString, Notes::PageMeta> m_trash;
    QHash<QString, QString> m_content; // body Markdown, for search
    QSet<QString> m_expanded;
    QString m_currentId;
    QString m_status;
    bool m_dirty = false;
    bool m_loading = false;
    int m_wordCount = 0;
    QTimer m_saveTimer;
    PageTreeModel m_tree;
    BlockModel m_blocks;
};

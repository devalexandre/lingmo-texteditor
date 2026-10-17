#pragma once

#include <QDateTime>
#include <QList>
#include <QString>

// Plain data + the Markdown dialect the notes are stored in. Everything here is
// pure (no I/O) so it can be unit tested directly.
namespace Notes {

struct Block {
    QString type = QStringLiteral("paragraph");
    QString text;
    int indent = 0;
    bool checked = false;   // todo
    bool collapsed = false; // toggle
    QString emoji;          // callout
    QString language;       // code
    QString ref;            // page link: id of the linked page
    bool operator==(const Block &o) const
    {
        return type == o.type && text == o.text && indent == o.indent && checked == o.checked
            && collapsed == o.collapsed && emoji == o.emoji && language == o.language && ref == o.ref;
    }
};

struct PageMeta {
    QString id;
    QString parent;
    QString icon;
    QString title;
    bool favorite = false;
    int order = 0;
    QDateTime created;
    QDateTime updated;
};

struct PageFile {
    PageMeta meta;
    QString body;          // Markdown after the title line
    bool hasFrontmatter = false;
};

// Block types that carry editable text.
bool isTextual(const QString &type);
// Bulleted, numbered, todo and toggle: written as tight list items.
bool isListLike(const QString &type);
bool isValidId(const QString &id);

QList<Block> parseBlocks(const QString &markdown);
QString serializeBlocks(const QList<Block> &blocks);

PageFile parsePage(const QString &fileText);
QString serializePage(const PageMeta &meta, const QString &body);

// Inline Markdown (**bold**, *italic*, `code`, ~~strike~~, [links](url)) to Qt rich text.
QString inlineToHtml(const QString &text, const QString &codeColor = QString());
// Text without Markdown syntax, for search and word count.
QString plainText(const QList<Block> &blocks);
int wordCount(const QString &text);

} // namespace Notes

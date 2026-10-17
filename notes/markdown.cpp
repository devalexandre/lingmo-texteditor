#include "markdown.h"

#include <QHash>
#include <QRegularExpression>
#include <functional>
#include <QStringList>
#include <QUuid>

namespace Notes {

namespace {

const QRegularExpression &re(const char *pattern)
{
    // Patterns are string literals, so caching by pointer is safe.
    static thread_local QHash<const char *, QRegularExpression> cache;
    auto it = cache.find(pattern);
    if (it == cache.end())
        it = cache.insert(pattern, QRegularExpression(QString::fromUtf8(pattern),
                                                      QRegularExpression::UseUnicodePropertiesOption));
    return *it;
}

const char *kFence = "^(`{3,}|~{3,})(.*)$";
const char *kHeading = "^(#{1,6})(?:[ \\t]+(.*?))?[ \\t]*$";
const char *kDivider = "^(?:-{3,}|\\*{3,}|_{3,})\\s*$";
const char *kTodo = "^[-*+] \\[([ xX])\\](?: (.*))?$";
const char *kToggle = "^[-*+] ([▶▼])(?: (.*))?$";
const char *kBullet = "^[-*+](?: (.*))?$";
const char *kNumbered = "^(\\d{1,9})[.)](?: (.*))?$";
const char *kCallout = "^> ?\\[!([^\\]]+)\\](?: (.*))?$";
const char *kQuote = "^> ?(.*)$";
const char *kPage = "^\\[((?:[^\\]\\\\]|\\\\.)*)\\]\\(([0-9a-fA-F-]{36})\\.md\\)\\s*$";

bool isMarker(const QString &s)
{
    if (s.isEmpty())
        return false;
    for (const char *p : {kFence, kHeading, kDivider, kBullet, kNumbered, kPage})
        if (re(p).match(s).hasMatch())
            return true;
    return s.startsWith(QLatin1Char('>'));
}

// Lines of paragraphs and list continuations: protect anything the parser would
// read as structure (markers, leading whitespace, empty lines, backslashes).
bool needsEscape(const QString &line)
{
    return line.isEmpty() || line.at(0).isSpace() || line.startsWith(QLatin1Char('\\')) || isMarker(line);
}
QString escapeLine(const QString &line) { return needsEscape(line) ? QLatin1Char('\\') + line : line; }
QString unescapeLine(const QString &s)
{
    return s.startsWith(QLatin1Char('\\')) && needsEscape(s.mid(1)) ? s.mid(1) : s;
}

// First line of a list item: "[ ]" / "▶" would change the item type.
bool itemNeedsEscape(const QString &t)
{
    return t.startsWith(QLatin1Char('\\')) || re("^\\[[ xX]\\](?: |$)").match(t).hasMatch()
        || re("^[▶▼](?: |$)").match(t).hasMatch();
}
QString escapeItem(const QString &t) { return itemNeedsEscape(t) ? QLatin1Char('\\') + t : t; }
QString unescapeItem(const QString &t)
{
    return t.startsWith(QLatin1Char('\\')) && itemNeedsEscape(t.mid(1)) ? t.mid(1) : t;
}

bool quoteNeedsEscape(const QString &t)
{
    return t.startsWith(QLatin1Char('\\')) || t.startsWith(QLatin1String("[!"));
}
QString escapeQuote(const QString &t) { return quoteNeedsEscape(t) ? QLatin1Char('\\') + t : t; }
QString unescapeQuote(const QString &t)
{
    return t.startsWith(QLatin1Char('\\')) && quoteNeedsEscape(t.mid(1)) ? t.mid(1) : t;
}

int leadingSpaces(const QString &line, int *chars)
{
    int width = 0, i = 0;
    for (; i < line.size(); ++i) {
        if (line.at(i) == QLatin1Char(' '))
            width += 1;
        else if (line.at(i) == QLatin1Char('\t'))
            width += 4;
        else
            break;
    }
    if (chars)
        *chars = i;
    return width;
}

QString stripIndent(const QString &line, int width)
{
    int i = 0, w = 0;
    while (i < line.size() && w < width && (line.at(i) == QLatin1Char(' ') || line.at(i) == QLatin1Char('\t'))) {
        w += line.at(i) == QLatin1Char('\t') ? 4 : 1;
        ++i;
    }
    return line.mid(i);
}

bool isAsciiWord(const QString &s)
{
    for (QChar c : s)
        if (!(c.isLetter() && c.unicode() < 128))
            return false;
    return !s.isEmpty();
}

} // namespace

bool isTextual(const QString &type)
{
    return type != QLatin1String("divider") && type != QLatin1String("page");
}

bool isListLike(const QString &type)
{
    return type == QLatin1String("bullet") || type == QLatin1String("numbered")
        || type == QLatin1String("todo") || type == QLatin1String("toggle");
}

bool isValidId(const QString &id)
{
    // Only canonical lowercase/uppercase UUIDs without braces: never a path.
    if (id.size() != 36)
        return false;
    return !QUuid::fromString(id).isNull() && re("^[0-9a-fA-F-]{36}$").match(id).hasMatch();
}

QList<Block> parseBlocks(const QString &markdown)
{
    QString text = markdown;
    text.replace(QLatin1String("\r\n"), QLatin1String("\n"));
    const QStringList lines = text.split(QLatin1Char('\n'));
    QList<Block> blocks;
    enum { None, Para, Item, Quote } cont = None;

    for (int i = 0; i < lines.size(); ++i) {
        const QString &raw = lines.at(i);
        if (raw.trimmed().isEmpty()) {
            cont = None;
            continue;
        }
        int chars = 0;
        const int spaces = leadingSpaces(raw, &chars);
        const QString s = raw.mid(chars);

        if (cont == Quote && s.startsWith(QLatin1Char('>'))) {
            QString rest = s.mid(1);
            if (rest.startsWith(QLatin1Char(' ')))
                rest.remove(0, 1);
            blocks.last().text += QLatin1Char('\n') + unescapeQuote(rest);
            continue;
        }
        if ((cont == Para || cont == Item) && !isMarker(s)) {
            blocks.last().text += QLatin1Char('\n') + unescapeLine(s);
            continue;
        }

        Block b;
        b.indent = qMin(spaces / 2, 8);
        cont = None;
        QRegularExpressionMatch m;
        if ((m = re(kFence).match(s)).hasMatch()) {
            const QString fence = m.captured(1);
            b.type = QStringLiteral("code");
            b.language = m.captured(2).trimmed();
            QStringList content;
            int j = i + 1;
            for (; j < lines.size(); ++j) {
                const QString t = lines.at(j).trimmed();
                if (t.size() >= fence.size() && t.startsWith(fence)
                    && t.count(fence.at(0)) == t.size())
                    break;
                content << stripIndent(lines.at(j), spaces);
            }
            b.text = content.join(QLatin1Char('\n'));
            i = j; // skip the closing fence (or run to the end when unclosed)
        } else if ((m = re(kHeading).match(s)).hasMatch()) {
            b.type = QStringLiteral("h%1").arg(qMin(m.captured(1).size(), 3));
            b.text = m.captured(2);
        } else if (re(kDivider).match(s).hasMatch()) {
            b.type = QStringLiteral("divider");
        } else if ((m = re(kPage).match(s)).hasMatch() && isValidId(m.captured(2))) {
            b.type = QStringLiteral("page");
            b.text = m.captured(1).replace(QLatin1String("\\]"), QLatin1String("]"))
                         .replace(QLatin1String("\\\\"), QLatin1String("\\"));
            b.ref = m.captured(2).toLower();
        } else if ((m = re(kTodo).match(s)).hasMatch()) {
            b.type = QStringLiteral("todo");
            b.checked = m.captured(1) != QLatin1String(" ");
            b.text = unescapeItem(m.captured(2));
            cont = Item;
        } else if ((m = re(kToggle).match(s)).hasMatch()) {
            b.type = QStringLiteral("toggle");
            b.collapsed = m.captured(1) == QStringLiteral("▶");
            b.text = unescapeItem(m.captured(2));
            cont = Item;
        } else if ((m = re(kBullet).match(s)).hasMatch()) {
            b.type = QStringLiteral("bullet");
            b.text = unescapeItem(m.captured(1));
            cont = Item;
        } else if ((m = re(kNumbered).match(s)).hasMatch()) {
            b.type = QStringLiteral("numbered");
            b.text = unescapeItem(m.captured(2));
            cont = Item;
        } else if ((m = re(kCallout).match(s)).hasMatch() && !isAsciiWord(m.captured(1))) {
            b.type = QStringLiteral("callout");
            b.emoji = m.captured(1);
            b.text = unescapeQuote(m.captured(2));
            cont = Quote;
        } else if ((m = re(kQuote).match(s)).hasMatch()) {
            b.type = QStringLiteral("quote");
            b.text = unescapeQuote(m.captured(1));
            cont = Quote;
        } else {
            b.text = unescapeLine(s);
            cont = Para;
        }
        blocks.append(b);
    }
    return blocks;
}

QString serializeBlocks(const QList<Block> &blocks)
{
    QString out;
    const Block *prev = nullptr;
    QList<int> numbers; // running number per indent level
    for (const Block &b : blocks) {
        const int indent = qBound(0, b.indent, 8);
        if (numbers.size() > indent + 1)
            numbers.resize(indent + 1);
        while (numbers.size() < indent + 1)
            numbers.append(0);
        if (b.type == QLatin1String("numbered"))
            numbers[indent] += 1;
        else
            numbers[indent] = 0;

        if (b.type == QLatin1String("paragraph") && b.text.isEmpty())
            continue; // empty paragraphs are just spacing in the editor

        const QString ind(indent * 2, QLatin1Char(' '));
        const QStringList lines = b.text.split(QLatin1Char('\n'));
        QStringList chunk;
        const QString &t = b.type;
        auto item = [&](const QString &marker) {
            const QString first = escapeItem(lines.first());
            chunk << ind + marker + (first.isEmpty() ? QString() : QLatin1Char(' ') + first);
            for (int i = 1; i < lines.size(); ++i)
                chunk << ind + QLatin1String("  ") + escapeLine(lines.at(i));
        };
        if (t == QLatin1String("h1") || t == QLatin1String("h2") || t == QLatin1String("h3")) {
            const QString title = QString(b.text).replace(QLatin1Char('\n'), QLatin1Char(' '));
            chunk << ind + QString(t.mid(1).toInt(), QLatin1Char('#')) + QLatin1Char(' ') + title;
        } else if (t == QLatin1String("bullet")) {
            item(QStringLiteral("-"));
        } else if (t == QLatin1String("numbered")) {
            item(QString::number(numbers[indent]) + QLatin1Char('.'));
        } else if (t == QLatin1String("todo")) {
            item(b.checked ? QStringLiteral("- [x]") : QStringLiteral("- [ ]"));
        } else if (t == QLatin1String("toggle")) {
            item(b.collapsed ? QStringLiteral("- ▶") : QStringLiteral("- ▼"));
        } else if (t == QLatin1String("quote") || t == QLatin1String("callout")) {
            for (int i = 0; i < lines.size(); ++i) {
                const QString line = escapeQuote(lines.at(i));
                QString prefix = QStringLiteral(">");
                if (i == 0 && t == QLatin1String("callout")) {
                    QString emoji = b.emoji.isEmpty() ? QStringLiteral("💡") : b.emoji;
                    emoji.remove(QLatin1Char(']')).remove(QLatin1Char('\n'));
                    if (isAsciiWord(emoji))
                        emoji = QStringLiteral("💡");
                    prefix += QStringLiteral(" [!") + emoji + QLatin1Char(']');
                }
                chunk << ind + prefix + (line.isEmpty() ? QString() : QLatin1Char(' ') + line);
            }
        } else if (t == QLatin1String("code")) {
            QString fence = QStringLiteral("```");
            while (b.text.contains(fence))
                fence += QLatin1Char('`');
            chunk << ind + fence + QString(b.language).remove(QLatin1Char('\n')).remove(QLatin1Char('`'));
            if (!b.text.isEmpty())
                for (const QString &line : lines)
                    chunk << (line.isEmpty() ? QString() : ind + line);
            chunk << ind + fence;
        } else if (t == QLatin1String("divider")) {
            chunk << ind + QStringLiteral("---");
        } else if (t == QLatin1String("page")) {
            if (!isValidId(b.ref))
                continue;
            QString label = QString(b.text).replace(QLatin1Char('\n'), QLatin1Char(' '));
            label.replace(QLatin1String("\\"), QLatin1String("\\\\")).replace(QLatin1String("]"), QLatin1String("\\]"));
            chunk << ind + QLatin1Char('[') + label + QStringLiteral("](") + b.ref + QStringLiteral(".md)");
        } else { // paragraph
            for (const QString &line : lines)
                chunk << ind + escapeLine(line);
        }

        if (prev)
            out += (isListLike(prev->type) && isListLike(b.type)) ? QStringLiteral("\n") : QStringLiteral("\n\n");
        out += chunk.join(QLatin1Char('\n'));
        prev = &b;
    }
    if (!out.isEmpty())
        out += QLatin1Char('\n');
    return out;
}

PageFile parsePage(const QString &fileText)
{
    PageFile page;
    QString rest = fileText;
    rest.replace(QLatin1String("\r\n"), QLatin1String("\n"));
    if (rest.startsWith(QLatin1String("---\n"))) {
        int end = rest.indexOf(QLatin1String("\n---"), 3);
        while (end >= 0 && end + 4 < rest.size() && rest.at(end + 4) != QLatin1Char('\n'))
            end = rest.indexOf(QLatin1String("\n---"), end + 4);
        if (end >= 0) {
            PageMeta meta;
            bool hasId = false;
            const QStringList header = rest.mid(4, end - 4 + 1).split(QLatin1Char('\n'));
            for (const QString &line : header) {
                const int colon = line.indexOf(QLatin1Char(':'));
                if (colon <= 0)
                    continue;
                const QString key = line.left(colon).trimmed();
                const QString value = line.mid(colon + 1).trimmed();
                if (key == QLatin1String("id")) {
                    meta.id = value;
                    hasId = true;
                } else if (key == QLatin1String("parent")) {
                    meta.parent = value;
                } else if (key == QLatin1String("icon")) {
                    meta.icon = value;
                } else if (key == QLatin1String("favorite")) {
                    meta.favorite = value == QLatin1String("true");
                } else if (key == QLatin1String("order")) {
                    meta.order = value.toInt();
                } else if (key == QLatin1String("created")) {
                    meta.created = QDateTime::fromString(value, Qt::ISODate);
                } else if (key == QLatin1String("updated")) {
                    meta.updated = QDateTime::fromString(value, Qt::ISODate);
                }
            }
            if (hasId) {
                page.meta = meta;
                page.hasFrontmatter = true;
                rest = rest.mid(end + 4);
                if (rest.startsWith(QLatin1Char('\n')))
                    rest.remove(0, 1);
            }
        }
    }
    while (rest.startsWith(QLatin1Char('\n')))
        rest.remove(0, 1);
    if (rest.startsWith(QLatin1String("# ")) || rest == QLatin1String("#")
        || rest.startsWith(QLatin1String("#\n"))) {
        const int newline = rest.indexOf(QLatin1Char('\n'));
        page.meta.title = rest.left(newline < 0 ? rest.size() : newline).mid(2);
        rest = newline < 0 ? QString() : rest.mid(newline + 1);
        while (rest.startsWith(QLatin1Char('\n')))
            rest.remove(0, 1);
    }
    page.body = rest;
    return page;
}

QString serializePage(const PageMeta &meta, const QString &body)
{
    auto clean = [](QString s) { return s.replace(QLatin1Char('\n'), QLatin1Char(' ')).replace(QLatin1Char('\r'), QLatin1Char(' ')); };
    QString out = QStringLiteral("---\n");
    out += QStringLiteral("id: ") + clean(meta.id) + QLatin1Char('\n');
    out += QStringLiteral("parent: ") + clean(meta.parent) + QLatin1Char('\n');
    out += QStringLiteral("icon: ") + clean(meta.icon) + QLatin1Char('\n');
    out += QStringLiteral("favorite: ") + (meta.favorite ? QStringLiteral("true") : QStringLiteral("false")) + QLatin1Char('\n');
    out += QStringLiteral("order: ") + QString::number(meta.order) + QLatin1Char('\n');
    out += QStringLiteral("created: ") + meta.created.toString(Qt::ISODate) + QLatin1Char('\n');
    out += QStringLiteral("updated: ") + meta.updated.toString(Qt::ISODate) + QLatin1Char('\n');
    out += QStringLiteral("---\n# ") + clean(meta.title) + QLatin1Char('\n');
    if (!body.trimmed().isEmpty()) {
        out += QLatin1Char('\n') + body;
        if (!body.endsWith(QLatin1Char('\n')))
            out += QLatin1Char('\n');
    }
    return out;
}

QString inlineToHtml(const QString &text, const QString &codeColor)
{
    QString s = text.toHtmlEscaped();
    QStringList stash;
    auto keep = [&stash](const QString &html) {
        stash << html;
        return QString(QChar(0xE000)) + QString::number(stash.size() - 1) + QChar(0xE001);
    };
    auto replaceAll = [&s](const QRegularExpression &rx, const std::function<QString(const QRegularExpressionMatch &)> &fn) {
        QString result;
        int last = 0;
        auto it = rx.globalMatch(s);
        while (it.hasNext()) {
            const auto m = it.next();
            result += s.mid(last, m.capturedStart() - last) + fn(m);
            last = m.capturedEnd();
        }
        result += s.mid(last);
        s = result;
    };
    const QString codeStyle = codeColor.isEmpty()
        ? QStringLiteral("font-family:monospace;")
        : QStringLiteral("font-family:monospace; color:%1;").arg(codeColor);
    replaceAll(re("`([^`]+)`"), [&](const QRegularExpressionMatch &m) {
        return keep(QStringLiteral("<span style=\"%1\">%2</span>").arg(codeStyle, m.captured(1)));
    });
    replaceAll(re("\\[([^\\]]+)\\]\\(((?:https?|mailto):[^)\\s\"]+)\\)"), [&](const QRegularExpressionMatch &m) {
        return keep(QStringLiteral("<a href=\"%1\">%2</a>").arg(m.captured(2), m.captured(1)));
    });
    replaceAll(re("(?<![\\w/\"'=])(https?://[^\\s<\"]+[^\\s<\".,;:!?)])"), [&](const QRegularExpressionMatch &m) {
        return keep(QStringLiteral("<a href=\"%1\">%1</a>").arg(m.captured(1)));
    });
    s.replace(re("\\*\\*(?=\\S)(.+?)(?<=\\S)\\*\\*"), QStringLiteral("<b>\\1</b>"));
    s.replace(re("(?<!\\w)__(?=\\S)(.+?)(?<=\\S)__(?!\\w)"), QStringLiteral("<b>\\1</b>"));
    s.replace(re("~~(?=\\S)(.+?)(?<=\\S)~~"), QStringLiteral("<s>\\1</s>"));
    s.replace(re("(?<![*\\w])\\*(?=[^\\s*])(.+?)(?<=[^\\s*])\\*(?![*\\w])"), QStringLiteral("<i>\\1</i>"));
    s.replace(re("(?<![_\\w])_(?=[^\\s_])(.+?)(?<=[^\\s_])_(?![_\\w])"), QStringLiteral("<i>\\1</i>"));
    s.replace(QLatin1Char('\n'), QLatin1String("<br/>"));
    s.replace(QLatin1String("  "), QLatin1String("&nbsp; "));
    for (int i = stash.size() - 1; i >= 0; --i)
        s.replace(QString(QChar(0xE000)) + QString::number(i) + QChar(0xE001), stash.at(i));
    return s;
}

QString plainText(const QList<Block> &blocks)
{
    QStringList parts;
    for (const Block &b : blocks)
        if (isTextual(b.type) && !b.text.isEmpty())
            parts << b.text;
    return parts.join(QLatin1Char('\n'));
}

int wordCount(const QString &text)
{
    int count = 0;
    bool inWord = false;
    for (QChar c : text) {
        const bool wordChar = c.isLetterOrNumber();
        if (wordChar && !inWord)
            ++count;
        inWord = wordChar || (inWord && (c == QLatin1Char('\'') || c == QLatin1Char('-')));
    }
    return count;
}

} // namespace Notes

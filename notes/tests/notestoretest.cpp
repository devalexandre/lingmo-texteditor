#include "../notestore.h"

#include <QDir>
#include <QTemporaryDir>
#include <QUuid>
#include <QtTest>

using Notes::Block;

namespace {
Block mk(const QString &type, const QString &text, int indent = 0)
{
    Block b;
    b.type = type;
    b.text = text;
    b.indent = indent;
    return b;
}

QString readAll(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
        return {};
    return QString::fromUtf8(f.readAll());
}

void writeAll(const QString &path, const QString &text)
{
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write(text.toUtf8());
}

QString newId() { return QUuid::createUuid().toString(QUuid::WithoutBraces); }

QStringList treeTitles(NoteStore &store)
{
    QStringList titles;
    const auto rows = store.tree()->rows();
    for (const auto &r : rows)
        titles << QString(r.depth * 2, QLatin1Char(' ')) + r.title;
    return titles;
}
} // namespace

class NoteStoreTest : public QObject
{
    Q_OBJECT
private slots:
    void roundTripEveryBlockType()
    {
        QList<Block> blocks;
        blocks << mk("h1", "Título grande") << mk("h2", "Seção") << mk("h3", "Sub **seção**");
        blocks << mk("paragraph", "Texto com **negrito**, *itálico* e `código`\nsegunda linha");
        blocks << mk("paragraph", "- não é lista\n# nem título\n\n  espaços\n\\barra\n1. nem número\n---\n> nem citação");
        blocks << mk("bullet", "Item") << mk("bullet", "Filho", 1) << mk("bullet", "Neto\ncom quebra", 2);
        blocks << mk("bullet", "[ ] parece tarefa") << mk("bullet", "▶ parece toggle");
        blocks << mk("numbered", "Um") << mk("numbered", "Dois") << mk("numbered", "Dois.a", 1) << mk("numbered", "Três");
        Block todo = mk("todo", "Feito");
        todo.checked = true;
        blocks << todo << mk("todo", "Pendente") << mk("todo", "");
        Block toggle = mk("toggle", "Detalhes");
        toggle.collapsed = true;
        blocks << toggle << mk("paragraph", "Dentro do toggle", 1) << mk("bullet", "Lista no toggle", 1);
        Block open = mk("toggle", "Aberto");
        blocks << open;
        blocks << mk("quote", "Citação\n\nem várias linhas\n[!nao] é callout");
        blocks << mk("quote", "[!x] começa estranho");
        Block callout = mk("callout", "Atenção a **isto**\nsegunda linha");
        callout.emoji = "⚠️";
        blocks << callout;
        Block code = mk("code", "int main() {\n\n    return 0; // ```\n}");
        code.language = "cpp";
        blocks << code;
        Block nestedCode = mk("code", "echo oi\n  indentado", 1);
        blocks << mk("bullet", "Antes do código") << nestedCode;
        blocks << mk("divider", "");
        Block page;
        page.type = "page";
        page.ref = newId();
        page.text = "Sub [página]";
        blocks << page;
        blocks << mk("paragraph", "Fim");

        const QString md = Notes::serializeBlocks(blocks);
        const QList<Block> parsed = Notes::parseBlocks(md);
        QCOMPARE(parsed.size(), blocks.size());
        for (int i = 0; i < blocks.size(); ++i) {
            if (!(parsed.at(i) == blocks.at(i)))
                qWarning() << "block" << i << blocks.at(i).type << blocks.at(i).text << "→" << parsed.at(i).type << parsed.at(i).text
                           << parsed.at(i).indent << "\n" << md;
            QVERIFY(parsed.at(i) == blocks.at(i));
        }
        QCOMPARE(Notes::serializeBlocks(parsed), md); // stable
        QVERIFY(md.contains("- [x] Feito"));
        QVERIFY(md.contains("- [ ] Pendente"));
        QVERIFY(md.contains("> [!⚠️] Atenção"));
        QVERIFY(md.contains("1. Um\n2. Dois\n  1. Dois.a\n3. Três"));
        QVERIFY(md.contains("\n---\n"));
        QVERIFY(md.contains("](" + page.ref + ".md)"));
    }

    void parsesCommonMarkdown()
    {
        const auto blocks = Notes::parseBlocks("* estrela\n+ mais\n1) parêntese\n- [X] feito\n#### fundo\n***\n~~~\ncode\n~~~");
        QCOMPARE(blocks.size(), 7);
        QCOMPARE(blocks.at(0).type, QString("bullet"));
        QCOMPARE(blocks.at(1).type, QString("bullet"));
        QCOMPARE(blocks.at(2).type, QString("numbered"));
        QCOMPARE(blocks.at(3).type, QString("todo"));
        QVERIFY(blocks.at(3).checked);
        QCOMPARE(blocks.at(4).type, QString("h3"));
        QCOMPARE(blocks.at(5).type, QString("divider"));
        QCOMPARE(blocks.at(6).type, QString("code"));
        QCOMPARE(blocks.at(6).text, QString("code"));
        // GitHub alerts stay quotes, not callouts.
        QCOMPARE(Notes::parseBlocks("> [!NOTE]\n> texto").first().type, QString("quote"));
    }

    void inlineHtml()
    {
        const QString html = Notes::inlineToHtml("**b** *i* `c<d>` ~~s~~ [l](https://x.org) <script>");
        QVERIFY(html.contains("<b>b</b>"));
        QVERIFY(html.contains("<i>i</i>"));
        QVERIFY(html.contains("c&lt;d&gt;"));
        QVERIFY(html.contains("<s>s</s>"));
        QVERIFY(html.contains("<a href=\"https://x.org\">l</a>"));
        QVERIFY(!html.contains("<script>"));
        QVERIFY(!Notes::inlineToHtml("[x](javascript:alert(1))").contains("<a "));
    }

    void legacyMigration()
    {
        QTemporaryDir dir;
        const QString id = newId();
        const QString legacyBody = "## Plano\n- [ ] Café amanhã\n- item\n\nTexto **forte**\n\n| a | b |\n|---|---|\n| 1 | 2 |";
        writeAll(dir.path() + "/" + id + ".md", "# Projeto\n\n" + legacyBody);
        writeAll(dir.path() + "/notes.md", "# Não é uma página");
        NoteStore store(dir.path());
        QCOMPARE(store.pageCount(), 1);
        QCOMPARE(store.currentId(), id);
        QCOMPARE(store.title(), QString("Projeto"));
        const auto blocks = store.blocks()->blocks();
        QCOMPARE(blocks.size(), 5);
        QCOMPARE(blocks.at(0).type, QString("h2"));
        QCOMPARE(blocks.at(1).type, QString("todo"));
        QCOMPARE(blocks.at(1).text, QString("Café amanhã"));
        QCOMPARE(blocks.at(4).text, QString("| a | b |\n|---|---|\n| 1 | 2 |"));
        QCOMPARE(treeTitles(store), QStringList{"Projeto"});

        // Saving writes the frontmatter without losing the content.
        store.setTitle("Projeto 2");
        QVERIFY(store.save());
        const QString text = readAll(dir.path() + "/" + id + ".md");
        QVERIFY(text.startsWith("---\nid: " + id + "\n"));
        QVERIFY(text.contains("# Projeto 2\n"));
        QVERIFY(text.contains("## Plano\n"));
        QVERIFY(text.contains("- [ ] Café amanhã\n- item\n"));
        QVERIFY(text.contains("Texto **forte**\n\n| a | b |\n|---|---|\n| 1 | 2 |\n"));
        const Notes::PageFile page = Notes::parsePage(text);
        QVERIFY(page.hasFrontmatter);
        QCOMPARE(page.meta.id, id);
        QCOMPARE(page.meta.title, QString("Projeto 2"));
        QVERIFY(page.meta.parent.isEmpty());
        QCOMPARE(Notes::parseBlocks(page.body), blocks);
    }

    void pageTreeMoveTrashRestore()
    {
        QTemporaryDir dir;
        {
            NoteStore store(dir.path());
            const QString a = store.create();
            store.setTitle("A");
            const QString b = store.create(a);
            store.setTitle("B");
            const QString c = store.create(b);
            store.setTitle("C");
            const QString d = store.create();
            store.setTitle("D");
            QVERIFY(store.save());
            QVERIFY(!a.isEmpty() && !b.isEmpty() && !c.isEmpty() && !d.isEmpty());
            QCOMPARE(store.parentOf(c), b);
            QCOMPARE(treeTitles(store), (QStringList{"A", "  B", "    C", "D"}));
            QCOMPARE(store.breadcrumbs().size(), 1);
            QVERIFY(store.open(c));
            QCOMPARE(store.breadcrumbs().size(), 3);

            store.toggleExpanded(a);
            QCOMPARE(treeTitles(store), (QStringList{"A", "D"}));
            store.toggleExpanded(a);

            QVERIFY(!store.move(a, c)); // into its own descendant
            QVERIFY(!store.move(a, a));
            QVERIFY(store.move(c, QString()));
            QCOMPARE(treeTitles(store), (QStringList{"A", "  B", "D", "C"}));
            QVERIFY(store.moveSibling(c, -1));
            QCOMPARE(treeTitles(store), (QStringList{"A", "  B", "C", "D"}));
            QVERIFY(store.setFavorite(d, true));
            QCOMPARE(store.favorites().size(), 1);
            QVERIFY(store.setIcon(d, "🚀"));
            QCOMPARE(store.pageIcon(d), QString("🚀"));
            const QString dup = store.duplicate(d);
            QVERIFY(!dup.isEmpty());
            QCOMPARE(store.pageTitle(dup), QString("D (cópia)"));

            QVERIFY(store.move(c, a));
            QVERIFY(store.trash(a)); // takes B and C along
            QVERIFY(!store.exists(b));
            QVERIFY(QFile::exists(dir.path() + "/Trash/" + a + ".md"));
            QVERIFY(QFile::exists(dir.path() + "/Trash/" + b + ".md"));
            QCOMPARE(store.trashPages().size(), 1);
            QCOMPARE(store.trashPages().first().toMap().value("children").toInt(), 2);
            QVERIFY(store.search("A").isEmpty() || store.search("A").first().toMap().value("id") != a);
            QVERIFY(store.restore(a));
            QCOMPARE(store.parentOf(b), a);
            QVERIFY(store.trashPages().isEmpty());

            QVERIFY(store.trash(c));
            QVERIFY(store.trash(a));
            QVERIFY(store.restore(b)); // its parent is still in the trash: back at the root
            QCOMPARE(store.parentOf(b), QString());
            QVERIFY(store.deleteForever(a));
            QVERIFY(!QFile::exists(dir.path() + "/Trash/" + a + ".md"));
            // c (trashed earlier, still a child of a) goes along with it
            QVERIFY(!QFile::exists(dir.path() + "/Trash/" + c + ".md"));
            QVERIFY(store.trashPages().isEmpty());
            QVERIFY(!store.deleteForever(c));
        }
        NoteStore reopened(dir.path());
        QCOMPARE(reopened.pageCount(), 3); // B, D, D (cópia)
        const auto found = reopened.search("cópia");
        QCOMPARE(found.size(), 1);
        QCOMPARE(reopened.favorites().size(), 1);
    }

    void contentSearch()
    {
        QTemporaryDir dir;
        NoteStore store(dir.path());
        const QString id = store.create();
        store.setTitle("Mercado");
        store.blocks()->setText(0, "Comprar café e pão");
        QVERIFY(store.save());
        store.create();
        store.setTitle("Outra");
        QCOMPARE(store.search("CAFÉ").size(), 1);
        QCOMPARE(store.search("CAFÉ").first().toMap().value("id").toString(), id);
        QVERIFY(store.search("nada disso").isEmpty());
        QCOMPARE(store.search("").size(), 2);
    }

    void rejectsNonUuidIds()
    {
        QTemporaryDir dir;
        QDir().mkpath(dir.path() + "/sub");
        writeAll(dir.path() + "/sub/x.md", "# Fora");
        NoteStore store(dir.path());
        const QString id = store.create();
        QVERIFY(!id.isEmpty());
        QVERIFY(!store.open("../../outside"));
        QVERIFY(!store.open("sub/x"));
        QVERIFY(!store.open(""));
        QVERIFY(!store.open("{" + id + "}"));
        QVERIFY(!store.trash("../" + id));
        QVERIFY(!store.restore("../Trash/" + id));
        QVERIFY(!store.deleteForever("../../etc/passwd"));
        QVERIFY(!store.setIcon("../x", "x"));
        QVERIFY(!store.move(id, "../elsewhere"));
        QVERIFY(store.create("not-a-uuid").isEmpty());
        QVERIFY(!Notes::isValidId("00000000-0000-0000-0000-000000000000"));
        QVERIFY(Notes::isValidId(id));
        QCOMPARE(store.currentId(), id);
        QVERIFY(QFile::exists(dir.path() + "/sub/x.md"));
    }

    void blockEditing()
    {
        BlockModel m;
        m.setBlocks({});
        QCOMPARE(m.count(), 1);
        // Markdown shortcuts
        QVERIFY(m.setText(0, "# Olá", 2).value("applied").toBool());
        QCOMPARE(m.typeAt(0), QString("h1"));
        QCOMPARE(m.textAt(0), QString("Olá"));
        // Enter after a heading starts a paragraph
        auto f = m.split(0, 3);
        QCOMPARE(f.value("row").toInt(), 1);
        QCOMPARE(m.typeAt(1), QString("paragraph"));
        QVERIFY(m.setText(1, "- item", 2).value("applied").toBool());
        QCOMPARE(m.typeAt(1), QString("bullet"));
        // Enter in a list continues it; Enter on the empty item leaves it
        f = m.split(1, 4);
        QCOMPARE(m.typeAt(2), QString("bullet"));
        QVERIFY(m.changeIndent(2, 1));
        QCOMPARE(m.indentAt(2), 1);
        QVERIFY(!m.changeIndent(2, 1)); // can't go two levels deeper than its parent
        f = m.split(2, 0);              // empty nested item: outdent first
        QCOMPARE(m.indentAt(2), 0);
        f = m.split(2, 0);
        QCOMPARE(m.typeAt(2), QString("paragraph"));
        // Split in the middle and merge back
        m.setText(2, "abcdef");
        f = m.split(2, 3);
        QCOMPARE(m.textAt(2), QString("abc"));
        QCOMPARE(m.textAt(3), QString("def"));
        f = m.backspaceAtStart(3);
        QCOMPARE(f.value("row").toInt(), 2);
        QCOMPARE(f.value("pos").toInt(), 3);
        QCOMPARE(m.textAt(2), QString("abcdef"));
        // Backspace at the start of a list item turns it into text first
        f = m.backspaceAtStart(1);
        QCOMPARE(m.typeAt(1), QString("paragraph"));
        // Empty block is deleted
        const int before = m.count();
        m.insertBlock(3, "paragraph");
        f = m.backspaceAtStart(3);
        QCOMPARE(m.count(), before);
        QCOMPARE(f.value("row").toInt(), 2);
        // Todo and divider shortcuts
        int row = m.insertBlock(m.count(), "paragraph");
        QVERIFY(m.setText(row, "[] tarefa", 3).value("applied").toBool());
        QCOMPARE(m.typeAt(row), QString("todo"));
        row = m.insertBlock(m.count(), "paragraph");
        f = m.setText(row, "---", 3);
        QCOMPARE(m.typeAt(row), QString("divider"));
        QCOMPARE(f.value("row").toInt(), row + 1);
        // Toggle hides its children when collapsed
        row = m.insertBlock(m.count(), "toggle");
        const int child = m.insertBlock(m.count(), "paragraph", 1, "dentro");
        m.setCollapsed(row, true);
        QVERIFY(m.data(m.index(child), BlockModel::HiddenRole).toBool());
        QCOMPARE(m.nextEditable(row), -1);
        m.setCollapsed(row, false);
        QVERIFY(!m.data(m.index(child), BlockModel::HiddenRole).toBool());
        // Enter on an empty nested line steps out of the toggle
        const int empty = m.insertBlock(child + 1, "paragraph", 1);
        m.split(empty, 0);
        QCOMPARE(m.indentAt(empty), 0);
        m.deleteBlock(empty);
        // Moving a block takes its children along
        const int moved = m.moveBlock(row, -1);
        QCOMPARE(m.typeAt(moved), QString("toggle"));
        QCOMPARE(m.textAt(moved + 1), QString("dentro"));
        // Numbering
        m.setBlocks(Notes::parseBlocks("1. a\n2. b\n  - x\n3. c\n\npara\n\n1. d"));
        QCOMPARE(m.data(m.index(3), BlockModel::NumberRole).toInt(), 3);
        QCOMPARE(m.data(m.index(5), BlockModel::NumberRole).toInt(), 1);
        // Slash menu on a non-empty block inserts below
        f = m.applyType(0, "quote");
        QCOMPARE(m.typeAt(f.value("row").toInt()), QString("quote"));
        QCOMPARE(m.typeAt(0), QString("numbered"));
    }

    void failedSaveKeepsDraft()
    {
        QTemporaryDir dir;
        const auto path = dir.path() + "/notes";
        NoteStore store(path);
        QVERIFY(!store.create().isEmpty());
        store.setTitle("Rascunho");
        store.blocks()->setText(0, "Não perder");
        QVERIFY(QDir(path).removeRecursively());
        QVERIFY(!store.save());
        QVERIFY(store.create().isEmpty());
        QCOMPARE(store.blocks()->textAt(0), QString("Não perder"));
        QVERIFY(QDir().mkpath(path));
        QVERIFY(store.save());
    }

    void autosave()
    {
        QTemporaryDir dir;
        NoteStore store(dir.path());
        QVERIFY(!store.create().isEmpty());
        store.setTitle("Automático");
        store.blocks()->setText(0, "Persistido pelo timer");
        QSignalSpy saved(&store, &NoteStore::pagesChanged);
        QVERIFY(saved.wait(2000));
        QCOMPARE(store.status(), QString("Salvo"));
        NoteStore restored(dir.path());
        QCOMPARE(restored.title(), QString("Automático"));
        QCOMPARE(restored.blocks()->textAt(0), QString("Persistido pelo timer"));
    }
};

QTEST_GUILESS_MAIN(NoteStoreTest)
#include "notestoretest.moc"

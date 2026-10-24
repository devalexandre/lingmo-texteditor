#include "notesintents.h"
#include "notestore.h"

using LingmoIntents::IntentCall;
using LingmoIntents::IntentResult;

LingmoIntents::IntentService *publishNotesIntents(NoteStore *store, QObject *parent,
                                                  LingmoIntents::IntentService::Mode mode)
{
    auto *intents = new LingmoIntents::IntentService(QStringLiteral("lingmo-notes"), parent);
    intents->handle(QStringLiteral("notes.create"), [store](const IntentCall &call) {
        const QString title = call.params().value(QStringLiteral("title")).toString();
        const QString id = store->createPageWithContent(title, call.params().value(QStringLiteral("content")).toString());
        if (id.isEmpty())
            return IntentResult::failed(QObject::tr("Não foi possível criar a página."));
        return IntentResult::ok({{QStringLiteral("id"), id}, {QStringLiteral("title"), title}});
    });
    intents->handle(QStringLiteral("notes.append"), [store](const IntentCall &call) {
        const QString ref = call.params().value(QStringLiteral("page")).toString();
        const QString text = call.params().value(QStringLiteral("text")).toString();
        QString id = store->findPage(ref);
        if (id.isEmpty()) {
            id = store->createPageWithContent(ref, text);
            if (id.isEmpty())
                return IntentResult::failed(QObject::tr("Não foi possível criar a página."));
            return IntentResult::ok({{QStringLiteral("id"), id}, {QStringLiteral("title"), ref}, {QStringLiteral("created"), true}});
        }
        if (!store->appendToPage(id, text))
            return IntentResult::failed(QObject::tr("Não foi possível acrescentar o texto à página."));
        return IntentResult::ok({{QStringLiteral("id"), id}, {QStringLiteral("title"), store->pageTitle(id)},
                                 {QStringLiteral("created"), false}});
    });
    if (!intents->publish(mode)) {
        delete intents;
        return nullptr;
    }
    return intents;
}

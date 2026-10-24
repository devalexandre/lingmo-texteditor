#include <QApplication>
#include <QDir>
#include <QLockFile>
#include <QMessageBox>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardPaths>

#include "notestore.h"
#ifdef HAVE_LINGMO_INTENTS
#include "notesintents.h"
#endif

// `lingmo-notes --intents`: no window, serves notes.create / notes.append
// (D-Bus activation of com.lingmo.Notes) and quits when idle or when the
// Notes window starts and takes the bus name over.
static int runIntents(int argc, char **argv)
{
#ifdef HAVE_LINGMO_INTENTS
    QCoreApplication app(argc, argv);
    app.setOrganizationName("lingmoos");
    app.setApplicationName("lingmo-notes");
    const QString directory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(directory);
    NoteStore notes(directory);
    auto *intents = publishNotesIntents(&notes, &app, LingmoIntents::IntentService::Headless);
    if (!intents)
        return 1;
    intents->setIdleQuit(30000);
    return app.exec();
#else
    Q_UNUSED(argc)
    Q_UNUSED(argv)
    return 1;
#endif
}

int main(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i)
        if (qstrcmp(argv[i], "--intents") == 0)
            return runIntents(argc, argv);

    QApplication app(argc, argv);
    app.setOrganizationName("lingmoos");
    app.setApplicationName("lingmo-notes");
    app.setWindowIcon(QIcon::fromTheme("lingmo-texteditor"));
    const QString directory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(directory);
    QLockFile lock(directory + "/notes.lock");
    if (!lock.tryLock()) {
        QMessageBox::information(nullptr, QObject::tr("Notas"),
                                 QObject::tr("As notas já estão abertas ou a pasta está indisponível."));
        return 1;
    }
    // No QQuickStyle override: the Lingmo platform theme picks LingmoStyle, like the core apps.
    NoteStore notes(directory);
#ifdef HAVE_LINGMO_INTENTS
    // The window serves the actions itself (replacing a headless instance)
    publishNotesIntents(&notes, &app, LingmoIntents::IntentService::Primary);
#endif
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("notes", &notes);
    engine.load(QUrl("qrc:/notes/Main.qml"));
    if (engine.rootObjects().isEmpty())
        return 1;
    QObject::connect(&app, &QCoreApplication::aboutToQuit, &notes, &NoteStore::save);
    return app.exec();
}

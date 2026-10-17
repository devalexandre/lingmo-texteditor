#include <QApplication>
#include <QDir>
#include <QLockFile>
#include <QMessageBox>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardPaths>

#include "notestore.h"

int main(int argc, char **argv)
{
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
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("notes", &notes);
    engine.load(QUrl("qrc:/notes/Main.qml"));
    if (engine.rootObjects().isEmpty())
        return 1;
    QObject::connect(&app, &QCoreApplication::aboutToQuit, &notes, &NoteStore::save);
    return app.exec();
}

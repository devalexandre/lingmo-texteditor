/*
 * Copyright (C) 2023 Lingmo OS Team
 *
 * Author:     Lingmo OS Team <lingmoos@foxmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickImageProvider>
#include <QQmlContext>
#include <QFontDatabase>
#include <QFontInfo>
#include <QCommandLineParser>
#include <QMetaObject>
#include <QTranslator>
#include <QLocale>
#include <QIcon>
#include <QFile>

#include "documenthandler.h"
#include "highlightmodel.h"
#include "texteditor.h"

// Serves "image://icontheme/<name>" from the current icon theme
// (used by the About dialog icon).
class IconThemeImageProvider : public QQuickImageProvider
{
public:
    IconThemeImageProvider() : QQuickImageProvider(QQuickImageProvider::Pixmap) {}

    QPixmap requestPixmap(const QString &id, QSize *realSize, const QSize &requestedSize) override
    {
        const QSize size = requestedSize.isValid() ? requestedSize : QSize(64, 64);
        if (realSize)
            *realSize = size;

        QIcon icon = QIcon::fromTheme(id);
        if (icon.isNull())
            icon = QIcon::fromTheme(QStringLiteral("accessories-text-editor"));
        return icon.pixmap(size);
    }
};

// Returns a monospace font family, falling back to the first installed
// fixed-pitch family when the platform's fixed font does not resolve to one.
static QString monospaceFamily()
{
    const QFont fixed = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    if (QFontInfo(fixed).fixedPitch())
        return fixed.family();

    const QStringList preferred = {
        QStringLiteral("Noto Sans Mono"), QStringLiteral("Noto Mono"),
        QStringLiteral("DejaVu Sans Mono"), QStringLiteral("Liberation Mono"),
        QStringLiteral("Hack"), QStringLiteral("JetBrains Mono"), QStringLiteral("Roboto Mono"),
    };
    const QStringList families = QFontDatabase::families();
    for (const QString &family : preferred) {
        if (families.contains(family))
            return family;
    }
    for (const QString &family : families) {
        if (QFontDatabase::isFixedPitch(family) && !QFontDatabase::isPrivateFamily(family))
            return family;
    }
    return fixed.family();
}

QStringList formatUriList(const QStringList &list)
{
    QStringList val;

    for (const QString &i : list) {
        QFileInfo path(i);
        if (path.exists()) {
            QString absPath = path.absoluteFilePath();
            if (!val.contains(absPath, Qt::CaseSensitive))
                val.append(absPath);
        }
        else
            qDebug() << "lingmo-texteditor: " << i << "doesn't exist";
    }

    return val;
}

void openFile(QObject *qmlObj, QString &fileUrl)
{
    QVariant val_return;
    QVariant val_arg(fileUrl);
    QMetaObject::invokeMethod(qmlObj,
                            "addPath",
                            Q_RETURN_ARG(QVariant,val_return),
                            Q_ARG(QVariant,val_arg));
}

void newTab(QObject *qmlObj)
{
    QVariant val_return;
    QVariant val_arg;
    QMetaObject::invokeMethod(qmlObj,
                            "addTab",
                            Q_RETURN_ARG(QVariant,val_return));
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("Lingmo");
    app.setWindowIcon(QIcon::fromTheme("lingmo-texteditor"));

    /** 加载翻译 */
    QLocale locale;
    QString qmFilePath = QString("%1/%2.qm").arg("/usr/share/lingmo-texteditor/translations/").arg(locale.name());
    if (QFile::exists(qmFilePath)) {
        QTranslator *translator = new QTranslator(QApplication::instance());
        if (translator->load(qmFilePath)) {
            QApplication::installTranslator(translator);
        } else {
            translator->deleteLater();
        }
    }

    qmlRegisterType<DocumentHandler>("Lingmo.TextEditor", 1, 0, "DocumentHandler");
    qmlRegisterType<FileHelper>("Lingmo.TextEditor", 1, 0, "FileHelper");

    QCommandLineParser parser;
    parser.setApplicationDescription("A text editor specifically designed for LingmoOS.");
    parser.addHelpOption();
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsCompactedShortOptions);
    parser.addPositionalArgument("files", "Files", "[FILE1, FILE2,...]");

    parser.process(app);

    HighlightModel m;

    QQmlApplicationEngine engine;
    engine.addImageProvider(QStringLiteral("icontheme"), new IconThemeImageProvider);
    engine.rootContext()->setContextProperty(QStringLiteral("monospaceFamily"), monospaceFamily());
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);

    if (engine.rootObjects().isEmpty())
        return -1;

    QObject *root = engine.rootObjects().first();

    QStringList fileList = formatUriList(parser.positionalArguments());
    if (!fileList.isEmpty()) {
        for (QString &i : fileList)
            openFile(root, i);
    }
    else
        newTab(root);

    return app.exec();
}

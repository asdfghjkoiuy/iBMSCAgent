#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickStyle>
#include "editor/EditorDocument.h"
#include "audio/AudioEngine.h"
#include "app/AppController.h"
#include "app/AppSettings.h"
#include "app/ResourceTableModel.h"
#include "app/MeasureLengthModel.h"
#include "app/ThemeManager.h"
#include "ChartCanvasItem.h"

int main(int argc, char* argv[]) {
    QGuiApplication::setApplicationName("iBMSC");
    QGuiApplication::setOrganizationName("iBMSC");
    QGuiApplication::setApplicationVersion("0.1.0");

    QGuiApplication app(argc, argv);

    QQuickStyle::setStyle("Material");

    // ── Register custom QML types ─────────────────────────────────────────────
    qmlRegisterType<ChartCanvasItem>("iBMSC.Canvas", 1, 0, "ChartCanvas");

    // ── Core objects ──────────────────────────────────────────────────────────
    Editor::EditorDocument editorDoc;
    Audio::AudioEngine audioEngine;
    App::AppController controller(&editorDoc, &audioEngine);
    App::AppSettings::instance()->removeStaleRecentFiles();

    // ── Resource / measure-length models ─────────────────────────────────────
    App::ResourceTableModel wavResourceModel(App::ResourceKind::WAV, &editorDoc);
    App::ResourceTableModel bmpResourceModel(App::ResourceKind::BMP, &editorDoc);
    App::MeasureLengthModel measureLengthModel(&editorDoc);

    // ── Parse command-line arguments ─────────────────────────────────────────
    QStringList args = app.arguments();
    QString cliFile;
    double cliMeasure = -1;
    bool previewMode = false;
    for (int i = 1; i < args.size(); ++i) {
        if (args[i] == "-p") { previewMode = true; }
        else if (args[i] == "-m" && i + 1 < args.size()) { cliMeasure = args[++i].toDouble(); }
        else if (!args[i].startsWith('-')) { cliFile = args[i]; }
    }
    (void)previewMode;
    (void)cliMeasure;

    // ── QML engine ───────────────────────────────────────────────────────────
    QQmlApplicationEngine engine;
    QQmlContext* ctx = engine.rootContext();
    ctx->setContextProperty("editorDoc",          &editorDoc);
    ctx->setContextProperty("audioEngine",         &audioEngine);
    ctx->setContextProperty("appController",       &controller);
    ctx->setContextProperty("appSettings",         App::AppSettings::instance());
    ctx->setContextProperty("wavResourceModel",    &wavResourceModel);
    ctx->setContextProperty("bmpResourceModel",    &bmpResourceModel);
    ctx->setContextProperty("measureLengthModel",  &measureLengthModel);

    static App::ThemeManager themeManager;
    ctx->setContextProperty("themeManager",        &themeManager);

    engine.load(QUrl("qrc:/qml/Main.qml"));
    if (engine.rootObjects().isEmpty()) return -1;

    // Open CLI file after the window is up
    if (!cliFile.isEmpty())
        controller.openFile(cliFile);

    return app.exec();
}

#include "mainwindow.h"

#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("Neko Upscale");
    QApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("A GUI for realesrgan-ncnn-vulkan.");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("source", QApplication::translate("main", "Image file or directory to open."));

    parser.process(a);

    MainWindow w;
    const QStringList args = parser.positionalArguments();
    if (!args.isEmpty()) {
        w.setInputPath(args.first());
    }

    w.show();
    return a.exec();
}
#include "Widgets/MRNWidget.h"
#include <QApplication>
#include <iostream>
#include <thread>
#include <QTranslator>
#include <QFile>
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // initialize language
    QString   language = "zh_CN";

    QTranslator* translator = new QTranslator;
    translator->load(":/Translations/" + language + ".qm");
    app.installTranslator(translator);

    // initialize style
    QFile file(":/style.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    qApp->setStyleSheet(styleSheet);

    MRNWidget mrnwidget;
    mrnwidget.showMaximized();
    return app.exec();
}

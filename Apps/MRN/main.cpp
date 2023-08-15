#include <iostream>
#include <thread>
#include <QApplication>
#include "Widgets/MRNWidget.h"
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MRNWidget mrnwidget;
    mrnwidget.show();
    return app.exec();
}

#include "Widgets/MRNWidget.h"
#include <QApplication>
#include <iostream>
#include <thread>
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MRNWidget mrnwidget;
    mrnwidget.show();
    return app.exec();
}

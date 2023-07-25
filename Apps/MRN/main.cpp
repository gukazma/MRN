#include <iostream>
#include <thread>
#include <QApplication>
#include "Widgets/MRNDialog.h"
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MRNDialog* mrndialog = new MRNDialog();
    mrndialog->showMaximized();

    return app.exec();
}

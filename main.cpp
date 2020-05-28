#include "mainwindow.h"

#include <QApplication>
#include <initialization.h>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Initialization init;
    init.createMainWindow()->show();
    return a.exec();
}

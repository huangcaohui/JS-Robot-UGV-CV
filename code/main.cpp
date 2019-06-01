#include "mainwindow.h"
#include "control.h"
//#include <QApplication>

int main(int argc, char *argv[])
{
//    QApplication a(argc,argv);
//    MainWindow instance;
//    instance.show();

    JUSTUGV::Control instance;

    instance.run();

    return 0;

//    return a.exec();
}

#include "nixtraywidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NixTrayWidget w;
    //w.show();

    return a.exec();
}

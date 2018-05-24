#include "pihomewidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PiHomeWidget w;
    w.show();

    return a.exec();
}

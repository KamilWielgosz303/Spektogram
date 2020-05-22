#include "spektogram.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Spektogram w;
    w.show();
    return a.exec();
}

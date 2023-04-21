#include "ScatterPointTool.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ScatterPointTool w;
    w.show();
    return a.exec();
}

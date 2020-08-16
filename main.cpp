#include "Acceuil.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Acceuil w;
    w.setWindowFlags(Qt::Drawer);
    w.show();
    return a.exec();
}

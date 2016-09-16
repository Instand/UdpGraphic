#include "basegui.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    BaseGUI w;

    QFile file(":/appStyle.css");
    file.open(QFile::ReadOnly);
    qApp->setStyleSheet(file.readAll());

    w.show();
    w.setLogPos();

    return a.exec();
}

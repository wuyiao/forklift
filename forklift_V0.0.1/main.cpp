#include "widget.h"
#include <QLocalSocket>
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsProxyWidget>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Widget w;


    QGraphicsScene * scene = new QGraphicsScene(&a);
    QGraphicsProxyWidget  * ww = scene->addWidget(&w);

    ww->setRotation(90);
    QGraphicsView * view = new QGraphicsView(scene);
    view->resize(1024,600);
    view->show();

    w.show();
    return a.exec();
}

#include <QApplication>
#include "serialwidget.h"

int main(int argc,char** argv)
{
    QApplication app(argc,argv);

    SerialWidget widget;
    widget.show();



    return app.exec();
}

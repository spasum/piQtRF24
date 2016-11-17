#include <QCoreApplication>

#include "rf24interface.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    RF24Interface::instance().init();

    return a.exec();
}

#include <QCoreApplication>

#include "rf24interface.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    RF24Interface rf24;
    rf24.loop();

    return a.exec();
}

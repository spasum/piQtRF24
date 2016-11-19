#include <QCoreApplication>
#include <QDebug>

#include <signal.h>

#include "rf24interface.h"
#include "wiringpiinterface.h"

void closeApplication(int sig)
{
    qCritical() << "Close application"
             << QCoreApplication::applicationName()
             << "( signal =" << sig << "-" << strsignal(sig) << ")";

    QCoreApplication::quit();
}

void catchUnixSignal(const QList<int> &quitSignals)
{
        for(int sig = 0; sig < quitSignals.count(); sig++)
                signal(quitSignals.at(sig), closeApplication);
}

int main(int argc, char *argv[])
{
        QCoreApplication app(argc, argv);
        app.setOrganizationName("qt-examples");
        app.setApplicationName("rf24");

        QList <int> quitSignals = QList <int>()
                << SIGQUIT << SIGINT << SIGTERM << SIGHUP << SIGSEGV;
        catchUnixSignal(quitSignals);

        RF24Interface::instance().init();
        RF24Interface::instance().init();
        RF24Interface::instance().init();
        RF24Interface::instance().init();
        RF24Interface::instance().init();
        RF24Interface::instance().init();

        wiringPiInterface::instance().setupInterrupt("GPIO_25",
                                                     "INT_EDGE_FALLING",
                                                     &wiringPiInterface::interruptHandler,
                                                     NULL);

        return app.exec();
}

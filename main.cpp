#include <QCoreApplication>
#include <QDebug>

#include <signal.h>

#include "rf24interface.h"
#include "GPIO.h"

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

        GPIO gpio4("GPIO_4", INPUT, FALLING);
        GPIO gpio5("GPIO_5", INPUT, FALLING);
        GPIO gpio6("GPIO_6", INPUT, FALLING);
        GPIO gpio13("GPIO_13", INPUT, FALLING);
        GPIO gpio19("GPIO_19", INPUT, FALLING);
        GPIO gpio26("GPIO_26", INPUT, FALLING);
        GPIO gpio27("GPIO_27", INPUT, FALLING);

        return app.exec();
}

#include "GPIOFunctions.h"

#include <QDebug>
#include <QThread>
#include <QElapsedTimer>

GPIOFunctions::GPIOFunctions(QObject *parent) : QObject(parent)
{
    qDebug() << "GPIOFunctions constructor" << QThread::currentThreadId();
    m_stop = false;
}

GPIOFunctions::~GPIOFunctions()
{
    qDebug() << "GPIOFunctions destructor" << QThread::currentThreadId();
}

void GPIOFunctions::initInterruptRoutine(const unsigned int &gpio, const int &fd)
{
    qDebug() << "GPIOFunctions initInterruptRoutine()" << QThread::currentThreadId();
    m_gpio = gpio;
    m_fd = fd;

    setPriority(55);
}

void GPIOFunctions::stop()
{
    qDebug() << "GPIOFunctions stop()" << QThread::currentThreadId();
    m_stop = true;
}

void GPIOFunctions::polling()
{
    int ret;
    char *buf[MAX_BUF];

    qDebug() << "GPIOFunctions polling()" << QThread::currentThreadId();

    while(!m_stop)
    {
        struct pollfd fdset;

        fdset.fd = m_fd;
        fdset.events = POLLPRI;

        // continuous polling, setting timeout to -1
        ret = poll(&fdset, 1, -1);

        if(ret < 0)
            qCritical() << "poll() failed!";

        if(ret == 0)
            qDebug() << ".";

        if(fdset.revents & POLLPRI)
        {
            lseek(fdset.fd, 0, SEEK_SET);
            read(fdset.fd, buf, MAX_BUF);
            qDebug() << "Interrupt occurred on GPIO" << m_gpio;
            emit interruptDetected();
        }
    }

    emit finished();
}

int GPIOFunctions::setPriority(const int &pri)
{
    struct sched_param sched ;

    memset (&sched, 0, sizeof(sched)) ;

    if (pri > sched_get_priority_max (SCHED_RR))
        sched.sched_priority = sched_get_priority_max (SCHED_RR) ;
    else
        sched.sched_priority = pri ;

    return sched_setscheduler (0, SCHED_RR, &sched) ;
}

#include "rf24interface.h"
#include "rf24functions.h"

#include <QDebug>
#include <QThread>

bool RF24Interface::m_RF24InterfaceInitialized = false;

RF24Interface::RF24Interface(QObject *parent) : QObject(parent)

{
    qDebug() << "RF24Interface constructor" << QThread::currentThreadId();
}

RF24Interface::~RF24Interface()
{
    qDebug() << "RF24Interface destructor" << QThread::currentThreadId();
    if(thread)
        if(thread->isRunning())
            task->stop();
}

void RF24Interface::init()
{
    qDebug() << "RF24Interface init()" << QThread::currentThreadId()
             << RF24Interface::m_RF24InterfaceInitialized;

    if(RF24Interface::m_RF24InterfaceInitialized)
        return;

    thread = new QThread();
    thread->setObjectName("RF24Interface");

    task = new RF24Functions();
    task->moveToThread(thread);

    connect(thread, SIGNAL(started()), task, SLOT(loop()));
    connect(task, SIGNAL(finished()), this, SLOT(finished()));

    connect(thread, &QThread::finished, task, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    thread->start();

    RF24Interface::m_RF24InterfaceInitialized = true;
}

void RF24Interface::finished()
{
    qDebug() << "RF24Interface finished()" << QThread::currentThreadId();
}

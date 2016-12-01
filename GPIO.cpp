#include "GPIO.h"
#include "GPIOFunctions.h"

#include <QThread>
#include <QDebug>

#define SYSFS_GPIO_DIR "/sys/class/gpio"

GPIO::GPIO(const QString &gpio,
           const Direction &dir,
           const Edge &edge)
    : m_direction(dir)
    , m_edge(edge)
{
    qDebug() << "GPIO constructor" << QThread::currentThreadId();

    QString toRemove = "GPIO_";
    QString copy = gpio;
    if(gpio.contains(toRemove))
        m_GPIO = copy.remove(toRemove).toUInt();

    // @TODO check error codes for the following methods
    exportGPIO();
    setDirection();
    setEdge();
    fdOpen();

    if(m_direction == INPUT)
    {
        thread = new QThread();
        thread->setObjectName("GPIO_" + m_GPIO);

        task = new GPIOFunctions();
        task->moveToThread(thread);

        connect(thread, SIGNAL(started()), task, SLOT(polling()));
        connect(task, SIGNAL(finished()), this, SLOT(finished()));

        connect(thread, &QThread::finished, task, &QObject::deleteLater);
        connect(thread, &QThread::finished, thread, &QObject::deleteLater);

        connect(task, SIGNAL(interruptDetected()), this, SIGNAL(interruptOccured()));
        connect(task, SIGNAL(interruptDetected()), this, SLOT(interruptDetected()));

        thread->start();
    }
}

GPIO::~GPIO()
{
    qDebug() << "GPIO destructor" << QThread::currentThreadId();
    fdClose();
    if(thread)
        if(thread->isRunning())
            task->stop();
}

void GPIO::finished()
{
    qDebug() << "GPIO finished()" << QThread::currentThreadId();
}

void GPIO::interruptDetected()
{
    qDebug() << "Interrupt on GPIO" << m_GPIO;
}

unsigned int GPIO::getPin() const
{
    return m_GPIO;
}

Direction GPIO::getDirection() const
{
    return m_direction;
}

Edge GPIO::getEdge() const
{
    return m_edge;
}

int GPIO::exportGPIO() const
{
    int fd, len;
    char buf[MAX_BUF];

    fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
    if(fd < 0)
    {
        qCritical() << "Cannot export GPIO" << m_GPIO;
        return fd;
    }

    len = snprintf(buf, sizeof(buf), "%d", m_GPIO);
    write(fd, buf, len);
    close(fd);

    return 0;
}

int GPIO::unexportGPIO() const
{
    int fd, len;
    char buf[MAX_BUF];

    fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
    if(fd < 0)
    {
        qCritical() << "Cannot unexport GPIO" << m_GPIO;
        return fd;
    }

    len = snprintf(buf, sizeof(buf), "%d", m_GPIO);
    write(fd, buf, len);
    close(fd);
    return 0;
}

int GPIO::setDirection() const
{
    int fd;
    char buf[MAX_BUF];

    snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR  "/gpio%d/direction", m_GPIO);

    fd = open(buf, O_WRONLY);
    if(fd < 0)
    {
        qCritical() << "Cannot set direction for GPIO" << m_GPIO;
        return fd;
    }

    switch (m_direction)
    {
    case INPUT:
        write(fd, "in", 3);
        break;
    case OUTPUT:
        write(fd, "out", 4);
    default:
        qWarning() << "Direction not supported!";
        break;
    }

    close(fd);
    return 0;
}

int GPIO::setValue(unsigned int &value) const
{
    int fd;
    char buf[MAX_BUF];

    snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", m_GPIO);

    fd = open(buf, O_WRONLY);
    if(fd < 0)
    {
        qCritical() << "Cannot set value for GPIO" << m_GPIO;
        return fd;
    }

    if(value)
        write(fd, "1", 2);
    else
        write(fd, "0", 2);

    close(fd);
    return 0;
}

int GPIO::getValue(unsigned int *value) const
{
    int fd;
    char buf[MAX_BUF];
    char ch;

    snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", m_GPIO);

    fd = open(buf, O_RDONLY);
    if(fd < 0)
    {
        qCritical() << "Cannot read value for GPIO" << m_GPIO;
        return fd;
    }

    read(fd, &ch, 1);

    if(ch != '0')
        *value = 1;
    else
        *value = 0;

    close(fd);
    return 0;
}

int GPIO::setEdge() const
{
    int fd;
    char buf[MAX_BUF];

    snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/edge", m_GPIO);

    fd = open(buf, O_WRONLY);
    if(fd < 0) {
        qCritical() << "Cannot set edge for GPIO" << m_GPIO;
        return fd;
    }

    switch(m_edge)
    {
    case RISING:
        write(fd, "rising", strlen("rising") + 1);
        break;
    case FALLING:
        write(fd, "falling", strlen("falling") + 1);
        break;
    default:
        break;
    }
    close(fd);
    return 0;
}

void GPIO::fdOpen()
{
    int fd;
    char buf[MAX_BUF];

    snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", m_GPIO);

    fd = open(buf, O_RDONLY | O_NONBLOCK );

    if(fd < 0)
        qCritical() << "Cannot open fd for GPIO" << m_GPIO;

    m_fd = fd;
}

void GPIO::fdClose()
{
    close(m_fd);
}

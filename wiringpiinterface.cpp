#include "wiringpiinterface.h"

#include <QElapsedTimer>

#ifdef __RPI__
extern "C"
{
int wiringPiSetupGpio(void);
int wiringPiISR (int, int, void (*function)(void*), void*);
}

bool wiringPiInterface::m_wiringPiInitialized = false;;
#endif

wiringPiInterface::wiringPiInterface(QObject *parent) : QObject(parent)
{
}

wiringPiInterface::~wiringPiInterface()
{
}

bool wiringPiInterface::initWiringPi()
{
    bool ret = false;
#ifdef __RPI__
    if(!wiringPiInterface::m_wiringPiInitialized)
    {
        // Call for wiringPiSetupGpio to initialize wiringPi using Broadcom pin numbers
        if(wiringPiSetupGpio() < 0)
            qCritical() << "Unable to setup wiringPi: " << strerror(errno);
        else
        {
            wiringPiInterface::m_wiringPiInitialized = true;
            ret = true;
        }
    }
    else
        ret = true;
#endif
    return ret;
}

void wiringPiInterface::setupInterrupt(const QString &GPIO,
                                       const QString &edgeType,
                                       void (*function)(void *),
                                       void *arg)
{
    qDebug() << "setupInterrupt(" << GPIO << ", " << edgeType << ", " << function << ", "
             << arg << ")";
    if(!initWiringPi())
        qCritical() << "Cannot setup interrupt on " << GPIO << "wiringPi cannot be initialized!";
#ifdef __RPI__
    int pin = StringToGPIO(GPIO);
    int edge = StringToEdge(edgeType);

    // Call for wiringPiISR() interrupt initalization function
    // the edgeType can be: INT_EDGE_FALLING, INT_EDGE_RISING,
    // INT_EDGE_BOTH or INT_EDGE_SETUP
    //
    // The pin number is supplied in the current mode – native
    // wiringPi, BCM_GPIO, physical or Sys modes.
    //
    // This function will work in any mode, and does not need root
    // privileges to work.
    //
    if(wiringPiISR(pin, edge, function, arg) < 0)
        qCritical() << "Unable to setup ISR on " << pin << " : " << strerror(errno);
    else
        qDebug() << "Setup interrupt on " << pin << " arg: " << this << " was successfull!";
#endif
}

void wiringPiInterface::interruptHandler(void *userData)
{
    Q_UNUSED(userData);
    QElapsedTimer timer;
    timer.start();
    qDebug() << "interruptHandler() took " << timer.elapsed() << "ms";
}

#ifndef WIRINGPIINTERFACE_H
#define WIRINGPIINTERFACE_H

#include <QObject>
#include <QDebug>

#ifdef __RPI__
#include <wiringPi.h>
#include <errno.h>

inline int StringToGPIO(const QString &gpio)
{
    int returnValue = -1;
    QString toRemove = "GPIO_";
    QString copy = gpio;
    if(gpio.contains(toRemove))
        returnValue = copy.remove(toRemove).toInt();

    return returnValue;
}

inline int StringToEdge(const QString &edge)
{
    int returnValue = -1;
    if(edge.contains("SETUP"))
        returnValue = 0; //INT_EDGE_SETUP
    if(edge.contains("FALLING"))
        returnValue = 1; //INT_EDGE_FALLING;
    if(edge.contains("RISING"))
        returnValue = 2; //INT_EDGE_RISING;
    if(edge.contains("BOT"))
        returnValue = 3; //INT_EDGE_BOTH;
    if(returnValue == -1)
        qCritical() << "Wrong edge string provided!";

    return returnValue;
}

inline QString EdgeToString(const int &edge)
{
    QString ret;
    switch (edge)
    {
    case  INT_EDGE_SETUP:
        ret = " INT_EDGE_SETUP";
        break;
    case INT_EDGE_FALLING:
        ret = "INT_EDGE_FALLING";
        break;
    case INT_EDGE_RISING:
        ret = "INT_EDGE_RISING";
        break;
    case INT_EDGE_BOTH:
        ret = "INT_EDGE_BOTH";
        break;
    default:
        qCritical() << "Wrong edge value provided!";
        break;
    }

    return ret;
}
#endif

class wiringPiInterface : public QObject
{
    Q_OBJECT

    explicit wiringPiInterface(QObject *parent = 0);
    virtual ~wiringPiInterface();
    Q_DISABLE_COPY(wiringPiInterface)

    bool initWiringPi();

#ifdef __RPI__
    static bool m_wiringPiInitialized;
#endif

public:
    static wiringPiInterface &instance()
    {
        static wiringPiInterface obj;
        return obj;
    }
    static void interruptHandler(void*);

    void setupInterrupt(const QString&,
                        const QString&,
                        void (*function)(void *),
                        void*);

};

#endif // WIRINGPIINTERFACE_H

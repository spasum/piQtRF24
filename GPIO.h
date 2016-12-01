#ifndef GPIO_H
#define GPIO_H

#include <QObject>

class QThread;
class GPIOFunctions;

namespace piHome {
typedef enum {
    INPUT = 0x3,
    OUTPUT = 0x4
} Direction;

typedef enum {
    FALLING = 0x10,
    RISING = 0x20,
    BOTH = 0x40
} Edge;
}

using namespace piHome;

class GPIO : public QObject
{
    Q_OBJECT

public:
    explicit GPIO(const QString&,
                  const Direction&,
                  const Edge&);
    virtual ~GPIO();

    unsigned int getPin() const;
    Direction getDirection() const;
    Edge getEdge() const;
    int getValue(unsigned int*) const;
    int setValue(unsigned int&) const;

signals:
    void interruptOccured();

private slots:
    int exportGPIO() const;
    int unexportGPIO() const;
    int setDirection() const;
    int setEdge() const;
    void fdOpen();
    void fdClose();

    void finished();
    void interruptDetected();

private:
    unsigned int m_GPIO;
    Direction m_direction;
    Edge m_edge;
    int m_fd;

    GPIOFunctions *task;
    QThread *thread;
};

#endif // GPIO_H

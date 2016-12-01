#ifndef GPIOFUNCTIONS_H
#define GPIOFUNCTIONS_H

#include <QObject>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#define MAX_BUF 64

class GPIOFunctions : public QObject
{
    Q_OBJECT

protected:
    explicit GPIOFunctions(QObject *parent = 0);
    virtual ~GPIOFunctions();
    Q_DISABLE_COPY(GPIOFunctions)

    friend class GPIO;

protected slots:
    void initInterruptRoutine(const unsigned int&, const int&);
    void stop();
    void polling();

signals:
    void finished();
    void interruptDetected();

private:
    int setPriority(const int&);

    bool m_stop;

    unsigned int m_gpio;
    int m_fd;
};

#endif // GPIOFUNCTIONS_H

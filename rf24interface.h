#ifndef RF24INTERFACE_H
#define RF24INTERFACE_H

#include <QObject>

class QThread;
class RF24Functions;

class RF24Interface : public QObject
{
    Q_OBJECT

    explicit RF24Interface(QObject *parent = 0);
    virtual ~RF24Interface();
    Q_DISABLE_COPY(RF24Interface)

public:
    static RF24Interface &instance()
    {
        static RF24Interface obj;
        return obj;
    }

    void init();

private slots:
    void finished();

private:
    QThread *thread;
    RF24Functions *task;

    static bool m_RF24InterfaceInitialized;
};

#endif // RF24INTERFACE_H

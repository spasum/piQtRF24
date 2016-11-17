#ifndef RF24FUNCTIONS_H
#define RF24FUNCTIONS_H

#include <QObject>

#ifdef __RPI__
#include <RF24/RF24.h>
#include <RF24Network/RF24Network.h>
#include <RF24Mesh/RF24Mesh.h>
#endif

class RF24Functions : public QObject
{
    Q_OBJECT

protected:
    explicit RF24Functions(QObject *parent = 0);
    virtual ~RF24Functions();
    Q_DISABLE_COPY(RF24Functions)

    static void interruptHandler();

    friend class RF24Interface;

protected slots:
    void loop();
    void stop();

signals:
    void finished();

private:
    bool m_stop;

#ifdef __RPI__
    static RF24 m_radio;
    static RF24Network m_network;
    static RF24Mesh m_mesh;

    // RF payload transmition:
    struct t_payload
    {
      unsigned short int IDnode; // Node ID - redundant information
      unsigned char val[4];
    };
#endif
};

#endif // RF24FUNCTIONS_H

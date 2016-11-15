#ifndef RF24INTERFACE_H
#define RF24INTERFACE_H

#include <QObject>

#ifdef __RPI__
#include <RF24/RF24.h>
#include <RF24Network/RF24Network.h>
#include <RF24Mesh/RF24Mesh.h>
#endif

class RF24Interface : public QObject
{
    Q_OBJECT
public:
    explicit RF24Interface(QObject *parent = 0);
    virtual ~RF24Interface();

    static void interruptHandler();

public slots:
    void loop();

private:
    static quint64 m_counter;

#ifdef __RPI__
    RF24 m_radio;
    RF24Network m_network;
    RF24Mesh m_mesh;

    // RF payload transmition:
    struct t_payload
    {
      unsigned short int IDnode; // Node ID - redundant information
      unsigned char val[4];
    };
#endif
};

#endif // RF24INTERFACE_H

#include "rf24interface.h"

#include <QDebug>
#include <QThread>
#include <QElapsedTimer>

#include <cstdlib>
#include <unistd.h>

#define RF24InterruptGPIO 17 //@TODO read this from settings file and take care it's BCM

quint64 RF24Interface::m_counter = 0;

RF24Interface::RF24Interface(QObject *parent) : QObject(parent)
  #ifdef __RPI__
  , m_radio(RPI_V2_GPIO_P1_15, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_8MHZ)
  , m_network(m_radio)
  , m_mesh(m_radio,m_network)
  #endif
{
    qDebug() << "RF24Interface constructor";

#ifdef __RPI__
    attachInterrupt(RF24InterruptGPIO,
                    INT_EDGE_FALLING,
                    interruptHandler);

    // Set the nodeID to 0 for the master node
    m_mesh.setNodeID(0);

    // Connect to the mesh
    m_mesh.begin();

    // Mask interrupts in order to have only on receive package
    m_radio.maskIRQ(1, 1, 0);
#endif
}

RF24Interface::~RF24Interface()
{
    qDebug() << "RF24Interface destructor";
}

void RF24Interface::interruptHandler()
{
    QElapsedTimer timer;
    timer.start();
    qDebug() << "RF24Interface::interruptHandler()" << QThread::currentThreadId();
#ifdef __RPI__
    qDebug() << "RF24Functions::interruptHandler counter " << ++RF24Interface::m_counter;
#endif
    qDebug() << "RF24Interface::interruptHandler took " << timer.elapsed() << "ms";
}

void RF24Interface::loop()
{
    qDebug() << "RF24Interface::loop: " << QThread::currentThreadId();

    static quint64 counter;

    while(1)
    {
#ifdef __RPI__ //@TODO change this to something more suggestive
        // Keep the network updated
        m_mesh.update();

        // DHCP service runs on the master node and assign addresses to the unit nodes
        m_mesh.DHCP();

        // @TODO move this to the interruptHandler.
        if(m_network.available())
        {
            t_payload pp; // Payload used on RF
            RF24NetworkHeader hdr;
            size_t dataSize = m_network.peek(hdr);
            float *value = NULL;

            m_network.read(hdr, &pp, dataSize);

            qDebug() << "New RF24 packet received: " << ++counter;

            qDebug() << "Received data: size=" << dataSize << " from node: "
                     << m_mesh.getNodeID(hdr.from_node) << " (" << hdr.from_node
                     << ") - " << "msgID=" << hdr.id;

            value = (float *) pp.val;

            if(hdr.type == 'T' || hdr.type == 'H' || hdr.type == 'P' || hdr.type == 'G')
            {
                qDebug() << "<" << hdr.type << "> from addr: " << hdr.from_node <<
                            " nodeID: " << pp.IDnode << "value: " << *value;
                qDebug() << "array: " << pp.val[0] << pp.val[1] << pp.val[2] << pp.val[3];
            }
            else
            {
                m_network.read(hdr,0,0);
                qDebug() << "Header: " << hdr.type << " from node: " << hdr.from_node;
            }
        }
#else
        qDebug() << "RF24Interface::loop: " << ++counter;
#endif
        usleep(250000); //250ms
    }
}

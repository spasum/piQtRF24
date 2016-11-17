#include "rf24functions.h"

#include <QDebug>
#include <QThread>
#include <QElapsedTimer>

#include <cstdlib>
#include <unistd.h>

#define RF24InterruptGPIO 17 //@TODO read this from settings file and take care it's BCM

quint64 RF24Functions::m_counter = 0;

RF24Functions::RF24Functions(QObject *parent) : QObject(parent)
  #ifdef __RPI__
  , m_radio(RPI_V2_GPIO_P1_15, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_8MHZ)
  , m_network(m_radio)
  , m_mesh(m_radio,m_network)
  #endif
{
    qDebug() << "RF24Functions constructor" << QThread::currentThreadId();
    m_stop = false;

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

RF24Functions::~RF24Functions()
{
    qDebug() << "RF24Functions destructor" << QThread::currentThreadId();
}

void RF24Functions::interruptHandler()
{
    QElapsedTimer timer;
    timer.start();
    qDebug() << "RF24Functions::interruptHandler()" << QThread::currentThreadId();
#ifdef __RPI__
    qDebug() << "RF24Functions::interruptHandler counter " << ++RF24Functions::m_counter;
#endif
    qDebug() << "RF24Functions::interruptHandler took " << timer.elapsed() << "ms";
}

void RF24Functions::stop()
{
    qDebug() << "RF24Functions::stop() " << QThread::currentThreadId();
    m_stop = true;
}

void RF24Functions::loop()
{
    qDebug() << "RF24Functions::loop() " << QThread::currentThreadId();

    static quint64 counter;

    while(!m_stop)
    {
#ifdef __RPI__
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
        qDebug() << "RF24Functions::loop: " << ++counter;
#endif
        usleep(250000); //250ms
    }

    emit finished();
}

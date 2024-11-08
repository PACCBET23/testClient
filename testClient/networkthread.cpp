#include "NetworkThread.h"
#include <QHostAddress>
#include <QDebug>

NetworkThread::NetworkThread(QObject *parent)
    : QThread(parent), port(0)
{
}

void NetworkThread::setupSocket(quint16 port)
{
    this->port = port;
    if (isRunning()) {
        quit();
        wait();
    }
    start();
}

void NetworkThread::run() {
    QUdpSocket udpSocket;

    if (!udpSocket.bind(QHostAddress::LocalHost, port)) {
        qWarning() << "не удалось привязать сокет к порту" << port;
        return;
    }
    qDebug() << "сокет привязан к порту" << port;

    connect(&udpSocket, &QUdpSocket::readyRead, this, [&]() {
            while (udpSocket.hasPendingDatagrams()) {
                QByteArray datagram;
                datagram.resize(udpSocket.pendingDatagramSize());
                udpSocket.readDatagram(datagram.data(), datagram.size());
                emit dataReceived(datagram);
            }
        }, Qt::DirectConnection);
    exec();
}

NetworkThread::~NetworkThread() {
    quit();
    wait();
}

#include "client.h"

#include <QString>

Client::Client(QObject *parent) : QObject(parent)
{}

Client::~Client()
{
    qDeleteAll(sockets);
}

bool Client::initFromList(const QStringList &addrs)
{
    for (auto &addr : addrs) {
        auto host_port = addr.split(":");
        auto s = new QTcpSocket();
        qDebug() << "Connection to" << host_port;
        s->connectToHost(host_port[0], host_port[1].toInt());
        if (!s->waitForConnected()) {
            return false;
        }
        sockets.push_back(s);
    }
    return true;
}

bool Client::writeData(const QByteArray &data)
{
    auto res = true;
    for (auto &socket : sockets) {
        if(socket->state() == QAbstractSocket::ConnectedState)
        {
            socket->write(data); //write the data itself
            res = res && socket->waitForBytesWritten();
        }
    }

    return res;
}

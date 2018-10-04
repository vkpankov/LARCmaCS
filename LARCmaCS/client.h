#pragma once

#include <QtCore>
#include <QtNetwork>
#include <QtNetwork>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = 0);

public slots:
    bool connectToHost(QString host, quint16 port);
    bool writeData(QByteArray data);

private:
    QByteArray IntToArray(qint32 source);
    QTcpSocket *socket;
};

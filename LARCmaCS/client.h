#pragma once

#include <QtCore>
#include <QtNetwork>
#include <QVector>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = 0);
    ~Client();
    bool initFromList(const QStringList &hosts);

public slots:
    bool writeData(const QByteArray &data);

private:
    QByteArray IntToArray(qint32 source);
    QVector<QTcpSocket *> sockets;
};

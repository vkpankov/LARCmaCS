#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <QObject>
#include <QFile>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QHostAddress>
#include <QThread>

#include <QtCore>
#include <QtNetwork>
#include <QTimer>
#include <QMap>

using std::map;
using std::vector;
#include <Set>

struct ConnectorWorker : QObject
{
    Q_OBJECT
public:
    ConnectorWorker(){}

    int shutdownconnector;
    char *curRuleArray;

    QString filename;
    QFile *ipFile;
    QUdpSocket *udpSocket;
    map<int const, QString> robotAddrMap;
    int connectedAllSocketsFlag;
    QMap<int,QString> numIP;
    QMap<QString,QString> macIP;

    QList<QString> macList;

    int gotPacketsNum;
    QTimer* timer;

    int connectedSockets;
    int connectedRobots;
    QByteArray command;

    char dat[12];


signals:
    void receivedDatagram(QByteArray datagram);
    void receivedAddresses(QByteArray addressDatagram);
    void gotStringAddresses(QByteArray stringAddressDatagram);
    void allNeededRobotsEnabled();
    void allTasksDone();
    void robotAdded(QString);
    void sendMacs(QList<QString>);
    void sendPortList(QStringList);

public slots:
    void init();

    void start();
    void stop();
    void run(int N, QByteArray command);
    void udpProcessPendingDatagrams();

    void receiveMacArray(QString*);
    void addIp(int id, QString ip);
};

struct Connector : QObject
{
    Q_OBJECT

public:
    ConnectorWorker worker;
    QThread thread;

    explicit Connector(){}
    ~Connector()
    {
        stop();
        thread.wait(100);
        thread.terminate();
        thread.wait(100);
    }

    void init()
    {
        worker.moveToThread(&thread);
        qDebug() << "Init connector ok";
        connect(this, SIGNAL(wstart()), &worker, SLOT(start()));
        connect(this, SIGNAL(wstop()), &worker, SLOT(stop()));
        connect(&thread, SIGNAL(finished()), &worker, SLOT(deleteLater()));
    }

    void start()
    {
        thread.start();
        thread.setPriority(QThread::HighestPriority);
        qDebug() << "Connector thread start";
        emit wstart();
    }

    void stop() { emit wstop(); }

signals:
    void wstart();
    void wstop();
};

#endif // CONNECTOR_H

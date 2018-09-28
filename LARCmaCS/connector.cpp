#include <stdlib.h>
#include <iostream>
#include <QMap>

#include "connector.h"

void ConnectorWorker::start()
{
    qDebug() << "Connector worker started";
    shutdownconnector = false;
    init();
}

void ConnectorWorker::stop()
{
    shutdownconnector = true;
}

void ConnectorWorker::init()
{
    qDebug() << "Initializing connector.worker";

    connectedSockets = 0;
    connectedRobots = 0;
    //filename = "numMAC.txt";
    command.resize(6);
    command[0]='r';
    command[1]='u';
    command[2]='l';
    command[3]='e';
    command[4]=' ';


    dat[0]='<';
    dat[1]='b';
    dat[2]='r';
    dat[3]='o';
    dat[4]='a';
    dat[5]='d';
    dat[6]=' ';

    dat[10]='c';
    dat[11]='>';
    udpSocket = new QUdpSocket(this);

    connectedAllSocketsFlag = false;
    curRuleArray = new char[4 * 7]; // FIXME!

    gotPacketsNum = 0;

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),SLOT(udpBroadcastRequestIP()));
    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(udpProcessPendingDatagrams()));

    numIP[3] = "192.168.43.75";

    qDebug()<<"INIT CONNECTOR OK";
}

void ConnectorWorker::receiveMacArray(QString * macArray)
{
    for (int i=0;i<12;++i)
    {
        qDebug()<<macArray[i];
        if (macArray[i].compare(""))//if robot exists
        {
            if (macArray[i].contains("bro")) //if it's arduino
                numIP[i+1] = macArray[i];
            else
                numIP[i+1] = macIP[macArray[i]]; //if it's TRIK
        }
    }
    qDebug()<<numIP;
    connectedAllSocketsFlag = true;
}

void ConnectorWorker::run(int N, QByteArray command){
    udpSocket->writeDatagram(command, QHostAddress(numIP[N]), 10000);
}

void ConnectorWorker::addIp(int id, QString ip) {
    numIP[id+1] = ip;
    qDebug() << numIP;
}

void ConnectorWorker::udpProcessPendingDatagrams()
{
      qDebug()<<"DATAGRAM!";
      QByteArray datagram;
      QHostAddress *robotAddress = new QHostAddress();
      while (udpSocket->hasPendingDatagrams()) {

          datagram.resize(udpSocket->pendingDatagramSize());
          udpSocket->readDatagram(datagram.data(), datagram.size(),robotAddress);
          QString str(datagram);

          QStringList lst = str.split(' ');
          if (!lst[0].compare("mac"))
          {
            if (!macList.contains(lst[1]))
            {
                macList.push_back(lst[1]);
                macIP[lst[1]] = robotAddress->toString();
                emit robotAdded(lst[1]);
            }
          }
      }
}

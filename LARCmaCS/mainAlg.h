#pragma once

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QSharedPointer>
#include <QFile>
#include "packetSSL.h"
#include <iostream>
#include "mlData.h"
#include "client.h"
#define MAX_NUM_ROBOTS 12

using namespace std;
#include <time.h>       /* clock_t, clock(), CLOCKS_PER_SEC */

struct MainAlgWorker : public QObject
{
    Q_OBJECT
    clock_t timer,timer_s,timer_m,timer_max;
    int Time_count;
    bool Send2BT[MAX_NUM_ROBOTS];
    Client client;

public:
    MainAlgWorker();
    ~MainAlgWorker() {}
signals:
    void sendToConnector(int N,QByteArray command);
    void sendToBTtransmitter(char * message);
    void mainAlgFree();
    void StatusMessage(QString message);
    void UpdatePauseState(QString message);
public slots:

    void start()
    {
        shutdowncomp = false;
        cout << "MainAlg worker start" << endl;
        init();
    }
    void Send2BTChangeit(bool *send2BT_);

    void stop() { shutdowncomp = true; }
    void run(PacketSSL packetssl);
    void Pause();
    void run_matlab();
    void stop_matlab();    
    void EvalString(QString s);

private:
    void init();

    char m_buffer[256]; // matlab buffer
    MlData fmldata;
    bool fmtlab;
    bool shutdowncomp;
    bool pause;
};

struct MainAlg : public QObject
{
    Q_OBJECT

public:
    MainAlgWorker worker;
    QThread thread;

    explicit MainAlg(){}
    ~MainAlg()
    {
        stop();
        thread.wait(100);
        thread.terminate();
        thread.wait(100);
    }

    void init()
    {
        worker.moveToThread(&thread);
        cout << "Init mainAlg ok" << endl;
        connect(this, SIGNAL(wstart()), &worker, SLOT(start()));
        connect(this, SIGNAL(wstop()), &worker, SLOT(stop()));
        connect(&thread, SIGNAL(finished()), &worker, SLOT(deleteLater()));
    }

    void start()
    {
        thread.start();
        cout << "Thread start" << endl;
        emit wstart();
    }

    void stop() { emit wstop(); }

signals:
    void wstart();
    void wstop();

public slots:

};

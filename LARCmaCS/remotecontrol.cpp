#include "remotecontrol.h"
#include "ui_remotecontrol.h"

#define RC_TIMER_CD 100

#include <QDebug>
RemoteControl::RemoteControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RemoteControl)
{
    ui->setupUi(this);
    qDebug()<<"START REMOTE CONTROL";
    connect(&timer,SIGNAL(timeout()),this,SLOT(RC_send()));
    connect(this,SIGNAL(destroyed()),this,SLOT(TimerStop()));
//    connect(this,SIGNAL()
    for(int i=0; i<256; i++)
        keys[i]=0;
    key_shift=0;

}

void RemoteControl::TimerStart()
{
    if (!timer.isActive())
    {

        qDebug()<<"<RemContril>: Start";
        timer.start(RC_TIMER_CD);
    }
}
void RemoteControl::TimerStop()
{
    if (timer.isActive())
    {
        emit RC_control(0, 0, 0, 0, 0);
        qDebug()<<"<RemContril>: Stop";
        timer.stop();
    }
}

RemoteControl::~RemoteControl()
{
    delete ui;
}


#define QT_KEY_SHIFT_NATIVE 16
#define QT_KEY_ESCAPE_NATIVE 27

void RemoteControl::keyPressEvent(QKeyEvent * key)
{
    quint32 nkey=key->nativeVirtualKey();
    //qDebug()<<key<<" "<<key->key()<<" "<<key->nativeVirtualKey();
    if (nkey<256)
        keys[nkey]=1;
    if (nkey==QT_KEY_ESCAPE_NATIVE)
    {
        this->close();
    }
    if (nkey==QT_KEY_SHIFT_NATIVE)
    {
        key_shift=1;
    }
}

void RemoteControl::keyReleaseEvent(QKeyEvent * key)
{
    quint32 nkey=key->nativeVirtualKey();
    //qDebug()<<key<<" "<<key->key()<<" "<<key->nativeVirtualKey();
    if (nkey<256)
        keys[nkey]=0;
    if (nkey==QT_KEY_SHIFT_NATIVE)
    {
        key_shift=0;
    }
    if(nkey == ' '){
        qDebug() << "Space release";
    }
}


void RemoteControl::RC_send(void)
{
    int L=0,R=0,S=0,K=0,B=0;

    int xVel = 0, yVel = 0, r = 0;
    bool kickUp = false;

    if(keys[' ']) {
        kickUp = true;
    }

    if (keys['W'] )
    {
        yVel = 20;
    }
    if (keys['S'])
    {
        yVel = -20;
    }
    if (keys['A'])
    {
        xVel = -20;
    }
    if (keys['D'])
    {
        xVel = 20;
    }
    if (keys['R'])
    {
        r = 10;
    }

    qDebug() << "emit " << xVel << " " << yVel;
    emit RC_control(xVel, yVel, r, 0, kickUp);

    return;

    if (keys['W'] )
    {

        L+=50;
        R+=50;
    }
    if (keys['S'])
    {
        L-=50;
        R-=50;
    }
    if (keys['A'])
    {
        L-=25;
        R+=25;
    }
    if (keys['D'])
    {
        L+=25;
        R-=25;
    }
    if (keys[' ']||keys['Q'])
        K=1;
    if (keys['E'])
        K=-1;
    if (keys['Z']||keys['B'])
        B=1;
    if (key_shift)
    {
        L=L*2;
        R=R*2;
        if (abs(L)>100) L=L/abs(L)*100;
        if (abs(R)>100) R=R/abs(R)*100;
    }
    ui->label_state->setText(" L=" +QString::number(L)+
                             " R=" +QString::number(R)+
                             " K=" +QString::number(K)+
                             " B=" +QString::number(B));
    //qDebug()<<L<<" "<<R<<" "<<K<<" "<<S;
    emit RC_control(L,R,K,B, kickUp);
}

void RemoteControl::closeEvent(QCloseEvent *)
{
    TimerStop();
}

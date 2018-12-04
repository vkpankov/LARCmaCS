#pragma once

#include <QWidget>
#include <QTimer>
#include "grSim_Packet.pb.h"
#include "grSim_Commands.pb.h"
#include "grSim_Replacement.pb.h"
#include "grSim_client.h"

namespace Ui {
class RemoteControl;
}

#include <QKeyEvent>
class RemoteControl : public QWidget
{
    Q_OBJECT

    void closeEvent(QCloseEvent *);

public:
    explicit RemoteControl(QWidget *parent = 0);
    ~RemoteControl();
private:
    QTimer timer;
    GrSim_Client grSimClient;

    void keyPressEvent(QKeyEvent *key);
    void keyReleaseEvent(QKeyEvent * key);
    int keys[256];
    int key_shift;
    int effort;

private slots:
    void RC_send(void);
public slots:
    void TimerStart();
    void TimerStop();
private:
    Ui::RemoteControl *ui;
signals:
    void RC_control(int left,int right,int kick,int beep, bool kickUp);
};


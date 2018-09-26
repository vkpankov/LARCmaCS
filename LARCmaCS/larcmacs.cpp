#include <math.h>
#include "larcmacs.h"
#include "ui_larcmacs.h"
#include "packetSSL.h"

LARCmaCS::LARCmaCS(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LARCmaCS),
    scalingRequested(true),
    sizescene(10),
    drawscale(1)
{
    ui->setupUi(this);

    fieldscene = new FieldScene();
    ui->fieldView->setScene(fieldscene);

    macsArray = new QString[12];

    receiver.init();
    mainalg.init();
    sceneview.init();
    connector.init();
    bttransmitter.init();

    //robots connect
//    connect(&wifiform,SIGNAL(initRobots()),&connector.worker, SLOT(startBroadcast()));
//    connect(&wifiform,SIGNAL(stopInit()),&connector.worker, SLOT(stopBroadcast()));
//    connect(&connector.worker,SIGNAL(robotAdded(QString)),&wifiform,SLOT(addRobot(QString)));
//    connect(&connector.worker,SIGNAL(allNeededRobotsEnabled()),&wifiform,SLOT(initEnded()));

    // GUIS
    connect(&wifiform,SIGNAL(PickWifiRobot(QString)),this, SLOT(PickWifiRobot(QString)));
    connect(&connector.worker,SIGNAL(sendPortList(QStringList)),this,SLOT(displayPorts(QStringList)));
    connect(this,SIGNAL(openPort(QString)),&connector.worker,SLOT(openPort(QString)));

    //algorithm connect
    connect(this, SIGNAL(MLEvalString(QString)),&mainalg.worker,SLOT(EvalString(QString)));
    connect(this, SIGNAL(MatlabPause()), &mainalg.worker, SLOT(Pause()));
    connect(&receiver.worker, SIGNAL(activateMA(PacketSSL)), &mainalg.worker, SLOT(run(PacketSSL)));
    connect(&mainalg.worker, SIGNAL(mainAlgFree()), &receiver.worker, SLOT(MainAlgFree()));
    //reciever
    connect(this,SIGNAL(ChangeMaxPacketFrequencyMod(bool)),&receiver.worker,SLOT(ChangeMaxPacketFrequencyMod(bool)));

    //send command to robots
//    connect(this,SIGNAL(receiveMacArray(QString*)),&connector.worker,SLOT(receiveMacArray(QString*)));
    connect(&mainalg.worker, SIGNAL(sendToConnector(int,QByteArray)), &connector.worker, SLOT(run(int,QByteArray)));

    //gui connector
    connect(&sceneview.worker, SIGNAL(updateView()), this, SLOT(updateView()));
    connect(ui->sceneslider, SIGNAL(valueChanged(int)), this, SLOT(scaleView(int)));

    //info GUI
    connect(&mainalg.worker,SIGNAL(UpdatePauseState(QString)),this,SLOT(UpdatePauseState(QString)));
    connect(&mainalg.worker, SIGNAL(StatusMessage(QString)), this, SLOT(UpdateStatusBar(QString)));
    connect(&receiver.worker, SIGNAL(UpdateSSLFPS(QString)), this, SLOT(UpdateSSLFPS(QString)));
    connect(&bttransmitter.worker, SIGNAL(UpdatePipeStatus(bool)), this, SLOT(UpdatePipeStatus(bool)));

    //SendToBTtransmitter
    connect(&mainalg.worker,SIGNAL(sendToBTtransmitter(char*)),&bttransmitter.worker,SLOT(addmessage(char*)));

    //remotecontrol
    connect(&remotecontol,SIGNAL(RC_control(int,int,int,int, bool)),this,SLOT(remcontrolsender(int, int,int, int, bool)));
    connect(this,SIGNAL(sendToConnectorRM(int,QByteArray)),&connector.worker,SLOT(run(int,QByteArray)));

    QObject::connect(this, SIGNAL(addIp(int, QString)),
                         &connector.worker, SLOT(addIp(int, QString)));


    //fieldScene Update
    connect(&receiver.worker,SIGNAL(activateGUI()),this,SLOT(fieldsceneUpdateRobots()));
    connect(this,SIGNAL(updateRobots()),fieldscene,SLOT(update()));
    //    connect(&receiver.worker, SIGNAL(activateGUI(PacketSSL)), &sceneview.worker, SLOT(repaintScene(PacketSSL)));

    //BTsenderform
    connect(&btform,SIGNAL(Send2BTChange(bool *)),this,SLOT(Send2BTChangeit(bool *)));
    connect(&btform,SIGNAL(Send2BTChange(bool *)),&mainalg.worker,SLOT(Send2BTChangeit(bool *)));

    sceneview.start();
    receiver.start();
    mainalg.start();
    connector.start();
    bttransmitter.start();
    UpdateStatusBar("Waiting SSL connection...");
    UpdateSSLFPS("FPS=0");
    Send2BTChangeit(btform.Send2BT);

    ui->robotIndex->addItem("1");
    ui->robotIndex->addItem("2");
    ui->robotIndex->addItem("3");
    ui->robotIndex->addItem("4");
    ui->robotIndex->addItem("5");
    ui->robotIndex->addItem("6");


}

void LARCmaCS::displayPorts(QStringList portList)
{
    //for (int i=0;i<portList.size();++i)
    ui->ard_comboBox->addItems(portList);
}

void LARCmaCS::Send2BTChangeit(bool * BTbox)
{
    ui->checkBox_BT->setChecked(btform.Send2BT[ui->RobotComboBox->currentIndex()]);
}

quint32 Crc32(QByteArray buf, int len)
{
    uint *crc_table = new uint[256];
    uint crc;
    uint i;
    for (i = 0; i < 256; i++)
    {
        uint j;
        crc = i;
        for (j = 0; j < 8; j++)
        {
            if ((crc & 1) > 0)
            {
                crc = (crc >> 1) ^ 0xEDB88320;
            }
            else
            {
                crc = crc >> 1;
            }
        }
        crc_table[i] = crc;
    };
    crc = 0xFFFFFFFF;
    uint vsp = 0;
    while (len-- > 0)
    {
        crc = crc_table[(crc ^ buf[vsp]) & 0xFF] ^ (crc >> 8);
        vsp++;
    }
    return crc ^ 0xFFFFFFFF;
}
struct CommandData {
    quint32 speed_x;
    quint32 speed_y;
    quint32 speed_r;
    quint32 speed_dribbler;
    quint8 dribbler_enable;
    quint32 kicker_volatage_level;
    quint8 kicker_charge_enable;
    quint8 kick_up;
    quint8 kick_forward;
};
QByteArray serializeCommand(CommandData data) {
    QByteArray byteArray;
    QDataStream baWriter(&byteArray, QIODevice::WriteOnly | QIODevice::Append);
    baWriter.setByteOrder(QDataStream::LittleEndian);
    baWriter << (quint8)0xAA;
    baWriter << (quint8)0xAA;
    baWriter << (quint8)0xAA;
    baWriter << (quint8)0xAA;
    baWriter << data.speed_x;
    baWriter << data.speed_y;
    baWriter << data.speed_r;
    baWriter << data.speed_dribbler;
    baWriter << data.dribbler_enable;
    baWriter << data.kicker_volatage_level;
    baWriter << data.kicker_charge_enable;
    baWriter << data.kick_up;
    baWriter << data.kick_forward;
    quint32 crc =Crc32(byteArray, 28);
    baWriter << crc;
    return byteArray;
}



void LARCmaCS::remcontrolsender(int l, int r,int k, int b, bool kickUp)
{
    QString ip = ui->lineEditRobotIp->text();
    CommandData data;
    data.speed_x = l;
    data.speed_y = r;
    data.speed_r = k;
    data.speed_dribbler = 0;
    data.dribbler_enable = 0;

    if(b!=-1){
        data.kicker_volatage_level = 4;
        data.kicker_charge_enable = 1;
        data.kick_up = kickUp;
        data.kick_forward = 0;
    }
    else{
        data.kicker_volatage_level = 0;
        data.kicker_charge_enable = 0;
        data.kick_up = 0;
        data.kick_forward = 0;
    }

    QByteArray byteData = serializeCommand(data);

    if(socket.ConnectedState == QUdpSocket::ConnectedState) {
        socket.writeDatagram(byteData, byteData.length(), QHostAddress(ip), 10000);
    }
    else
    {
        socket.connectToHost(ip, 10000);
        if(socket.ConnectedState == QUdpSocket::ConnectedState) {
        socket.writeDatagram(byteData, byteData.length(), QHostAddress(ip), 10000);
        }
    }

    return;

    QByteArray command;
    command.append(QString("rule ").toUtf8());
    command.append(l);
    command.append(r);
    command.append(k);
    command.append(b);
}



void LARCmaCS::fieldsceneUpdateRobots()
{
    fieldscene->UpdateRobots(receiver.worker.detection);
    emit updateRobots();
}


LARCmaCS::~LARCmaCS()
{
    delete ui;
}

void LARCmaCS::UpdatePauseState(QString message)
{
    ui->label_Pause->setText(message);
}
void LARCmaCS::UpdatePipeStatus(bool status)
{
    ui->pipe_checkBox->setChecked(status);
}
void LARCmaCS::UpdateSSLFPS(QString message)
{
    ui->label_SSL_FPS->setText(message);
}
void LARCmaCS::UpdateStatusBar(QString message)
{
    ui->StatusLabel->setText(message);
}
void LARCmaCS::scaleView(int _sizescene)
{
//    cout << _sizescene << "  " << sizescene;
//    qreal scaleFactor = (drawScale-1) - (qreal)_scaleFactor/100;
//    cout << scaleFactor << "  ";
//    qreal factor = ui->view->matrix().scale ( scaleFactor, scaleFactor ).mapRect ( QRectF ( 0, 0, 1, 1 ) ).width();
//    cout << factor << "  ";
//    if ( factor > 0.07 && factor < 100.0 )
//    drawscale = 1 - (float)(sizescene-_sizescene)/10;
    drawscale = pow(0.9, _sizescene-sizescene);
    sizescene = _sizescene;
//    ui->view->wheelEvent();
//    cout << " DS " << drawscale << endl;
    scalingRequested = true;

}

void LARCmaCS::updateView()
{
//  static float lastScale = 0;
//  if ( shutdownSoccerView )
//  {
//    return;
//  }
  if ( scalingRequested )
  {
      qreal factor = ui->fieldView->matrix().scale ( drawscale, drawscale ).mapRect ( QRectF ( 0, 0, 1, 1 ) ).width();
      if ( factor > 0.07 && factor < 100.0 )
          ui->fieldView->scale ( drawscale, drawscale );
      scalingRequested = false;
      ui->fieldView->viewport()->update();
  }

}
void LARCmaCS::on_pushButton_clicked()
{
    btform.hide();
    btform.show();
    btform.init();
}

void LARCmaCS::on_pushButton_Pause_clicked()
{
    emit MatlabPause();
}

#include <QFileDialog>
void LARCmaCS::on_pushButton_SetMLdir_clicked()
{
    QString  dir = QFileDialog::getExistingDirectory();
    QString  s="cd "+dir;
    qDebug()<<"New Matlab directory = "<<s;
    emit MLEvalString(s);
}

void LARCmaCS::on_PickRobot_pushButton_clicked()
{
    wifiform.hide();
    wifiform.show();
}

void LARCmaCS::PickWifiRobot(QString addr)
{
    wifiaddrdata[ui->RobotComboBox->currentIndex()]=addr;
    QString temp;
    temp.setNum(ui->RobotComboBox->currentIndex()+1);
    temp=temp+") "+addr;
    ui->RobotComboBox->setItemText(ui->RobotComboBox->currentIndex(),temp);
    qDebug()<<"PICK"<<addr;
    emit receiveMacArray(wifiaddrdata);
}

void LARCmaCS::on_pushButton_RC_clicked()
{
    remotecontol.hide();
    remotecontol.show();
    remotecontol.TimerStart();
}



void LARCmaCS::on_checkBox_BT_stateChanged(int arg1)
{
    btform.Send2BT[ui->RobotComboBox->currentIndex()]=arg1;
    btform.init();
}

void LARCmaCS::on_RobotComboBox_currentIndexChanged(int index)
{
     ui->checkBox_BT->setChecked(btform.Send2BT[ui->RobotComboBox->currentIndex()]);
}

void LARCmaCS::on_openPortButton_clicked()
{
    emit openPort(ui->ard_comboBox->currentText());
}

void LARCmaCS::on_checkBox_MlMaxFreq_stateChanged(int arg1)
{
    emit(ChangeMaxPacketFrequencyMod(arg1>0));
}



void LARCmaCS::on_AddRobot_pushButton_clicked()
{


    QString robotIp = ui->lineEditRobotIp->text();
    int foundIndex = ui->robotIpList->findText(robotIp);
    if(foundIndex==-1)
        ui->robotIpList->addItem(robotIp, ui->robotIndex->currentIndex());
    else
        ui->robotIpList->setCurrentIndex(foundIndex);

    emit(addIp(ui->robotIndex->currentIndex(), robotIp));

}

void LARCmaCS::on_robotIpList_activated(const QString &arg1)
{
    ui->lineEditRobotIp->setText(arg1);
    ui->robotIndex->setCurrentIndex(ui->robotIpList->itemData((ui->robotIpList->currentIndex())).Int - 1);
}

void LARCmaCS::on_pushButton_RemoteControl_clicked()
{
    remotecontol.hide();
    remotecontol.show();
    remotecontol.TimerStart();
}

void LARCmaCS::on_robotIndex_currentIndexChanged(int index)
{

    int id =  ui->robotIpList->findData(index);
    if(id>-1){
        ui->robotIpList->setCurrentIndex(id);
        ui->lineEditRobotIp->setText(ui->robotIpList->currentText());
    }
}

#include "grSim_client.h"
#include "grSim_Packet.pb.h"
#include "grSim_Commands.pb.h"
#include "grSim_Replacement.pb.h"
#include <QDateTime>

GrSim_Client::GrSim_Client(QObject *parent) :
    QObject(parent)
{
    // create a QUDP socket
    socket = new QUdpSocket(this);

    this->_addr.setAddress("224.5.23.2");
    this->_port = quint16(20011);

    socket->bind(this->_addr, this->_port);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

void GrSim_Client::sendCommand(double velX, double velY, double velAngular, double kickSpeedX, double kickSpeedY, int id){
    double zero = 0.0;
    grSim_Packet packet;
    bool yellow = false;
    packet.mutable_commands()->set_isteamyellow(yellow);
    packet.mutable_commands()->set_timestamp(QDateTime::currentMSecsSinceEpoch());
    grSim_Robot_Command* command = packet.mutable_commands()->add_robot_commands();
    command->set_id(id);

    command->set_wheelsspeed(!true);
    command->set_wheel1(zero);
    command->set_wheel2(zero);
    command->set_wheel3(zero);
    command->set_wheel4(zero);
    command->set_veltangent(velX);
    command->set_velnormal(velY);
    command->set_velangular(velAngular);

    command->set_kickspeedx(kickSpeedX);
    command->set_kickspeedz(kickSpeedY);
    command->set_spinner(false);


    QByteArray dgram;

    dgram.resize(packet.ByteSize());


    packet.SerializeToArray(dgram.data(), dgram.size());
    if(socket->writeDatagram(dgram, QHostAddress("127.0.0.1"), 20011) > -1){
        printf("send data to robot %d: vel_normal: %f, vel_tangent: %f, vel_angular: %f\n", command->id(), command->velnormal(), command->veltangent(), command->velangular());
    }
}

void GrSim_Client::readyRead(){
    // when data comes in
    QByteArray buffer;
    buffer.resize(socket->pendingDatagramSize());

    QHostAddress sender;
    quint16 senderPort;

    socket->readDatagram(buffer.data(), buffer.size(),
                         &sender, &senderPort);

}

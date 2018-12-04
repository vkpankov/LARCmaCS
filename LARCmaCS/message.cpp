#include "message.h"
#include <QDataStream>

Message::Message() :
    mSpeedX(0)
  , mSpeedY(0)
  , mSpeedR(0)
  , mSpeedDribbler(0)
  , mDribblerEnable(0)
  , mKickerVoltageLevel(0)
  , mKickerChargeEnable(0)
  , mKickUp(0)
  , mKickForward(0)
{
}

void Message::setSpeedX(int speedX)
{
    mSpeedX = speedX;
}

void Message::setSpeedY(int speedY)
{
    mSpeedY = speedY;
}

void Message::setSpeedR(int speedR)
{
    mSpeedR = speedR;
}

void Message::setSpeedDribbler(int speedDribbler)
{
    mSpeedDribbler = speedDribbler;
}

void Message::setDribblerEnable(int dribblerEnable)
{
    mDribblerEnable = dribblerEnable;
}

void Message::setKickVoltageLevel(int kickerVoltageLevel)
{
    mKickerVoltageLevel = kickerVoltageLevel;
}

void Message::setKickerChargeEnable(int kickerChargeEnable)
{
    mKickerChargeEnable = kickerChargeEnable;
}

void Message::setKickUp(int kickUp)
{
    mKickUp = kickUp;
}

void Message::setKickForward(int kickForward)
{
    mKickForward = kickForward;
}

QByteArray Message::generateByteArray()
{
    QByteArray ba;
    quint8 sync = 0xAA;
    QDataStream ds(&ba, QIODevice::WriteOnly);
    ds.setByteOrder(QDataStream::LittleEndian);
    for (int i = 0; i < 4; i++) {
        ds << sync;
    }

    ds << (quint32)mSpeedX;
    ds << (quint32)mSpeedY;
    ds << (quint32)mSpeedR;

    ds << (quint32)mSpeedDribbler;
    ds << (quint8)mDribblerEnable;

    ds << (quint32)mKickerVoltageLevel;
    ds << (quint8)mKickerChargeEnable;
    ds << (quint8)mKickUp;
    ds << (quint8)mKickForward;

    quint32 crc = calculateCRC(ba, 28);
    ds << (quint32)crc;
    return ba;
}

quint32 Message::calculateCRC(const QByteArray &buf, int len)
{
    quint32 crc_table[256];

    quint32 crc;

    quint32 i;

    for (i = 0; i < 256; i++) {
        quint32 j;
        crc = i;

        for (j = 0; j < 8; j++) {
            if ((crc & 1) > 0) {
                crc = (crc >> 1) ^ 0xEDB88320;
            } else {
                crc = crc >> 1;
            }
        }
        crc_table[i] = crc;
    }

    crc = 0xFFFFFFFF;

    quint32 vsp = 0;

    while (len-- > 0) {
        crc = crc_table[(crc ^ buf[vsp]) & 0xFF] ^ (crc >> 8);
        vsp++;
    }

    return crc ^ 0xFFFFFFFF;
}

int Message::getSpeedX(){
    return mSpeedX;
}

int Message::getSpeedY(){
    return mSpeedY;
}

int Message::getSpeedR(){
    return mSpeedR;
}

int Message::getKickForward(){
    return mKickForward;
}

int Message::getKickUp(){
    return mKickUp;
}

int Message::getSpeedDribbler(){
    return mSpeedDribbler;
}

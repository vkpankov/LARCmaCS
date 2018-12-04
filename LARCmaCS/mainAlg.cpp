#include "mainAlg.h"
#include <iostream>
#include <fstream>
//#include <TCHAR.H>
#include <windows.h>

#include <QtWidgets/QApplication>
using namespace std;

#include "QDebug"

#include "message.h"

int initConfig(RCConfig *config){
    ifstream configFile;
    string line;

    configFile.open("LARCmaCS.cnf", ios::in);

    if(!configFile.is_open()) {
        cerr << "Can't open configuration file" << endl;
        return -1;
    }

    if(!config) {
        cerr << "Can't parse configuration file" << endl;
        return -2;
    }

    while(!configFile.eof()){
        getline(configFile, line);

        if(line.empty())
            continue;

        if(line.find("rfclient.name") != string::npos) {
            config->name = line.substr(line.find('=') + 1);

            if(config->name.size() > 16) {
                cerr << "[WARNING] rfclient.name value is too long." << endl << "It will be truncated up to 16 characters." << endl;
                config->name.resize(16);
            }
        }

        if(line.find("rfclient.file_of_matlab") != string::npos) {
            string fom = line.substr(line.find('=') + 1);

            if(fom.size() > 16) {
                cerr << "[WARNING] rfclient.file_of_matlab value is too long." << endl << "It will be truncated up to 16 characters." << endl;
                fom.resize(16);
            }
            char *str = new char[fom.length() + 1];
            strcpy(str, fom.c_str());
            config->file_of_matlab = str;
        }

        if(line.find("rfclient.RULE_AMOUNT") != string::npos){
            config->RULE_AMOUNT = atoi(line.substr(line.find('=') + 1).c_str());
        }

        if(line.find("rfclient.RULE_LENGTH") != string::npos){
            config->RULE_LENGTH = atoi(line.substr(line.find('=') + 1).c_str());
        }

        if(line.find("rfclient.BACK_AMOUNT") != string::npos){
            config->BACK_AMOUNT = atoi(line.substr(line.find('=') + 1).c_str());
        }

        if(line.find("rfclient.BACK_LENGTH") != string::npos){
            config->BACK_LENGTH = atoi(line.substr(line.find('=') + 1).c_str());
        }
    }

    configFile.close();

    return 0;
}

void MainAlgWorker::init(){
    RCConfig rcconfig;

    cout << "Initialization config file..." << endl;

    if(!initConfig(&rcconfig)) {
        cout << rcconfig.file_of_matlab << endl;
        cout << rcconfig.name << endl;
        cout << rcconfig.BACK_AMOUNT << endl;
        cout << rcconfig.BACK_LENGTH << endl;
        cout << rcconfig.RULE_AMOUNT << endl;
        cout << rcconfig.RULE_LENGTH << endl;

        cout << "...successful" << endl;
    }
    else {
        cerr << "...bad" << endl;
        char *str = new char[16];
		int i = 0;
		for (char c : "main") {
			str[i++] = c;
		}
		rcconfig.file_of_matlab = str;
		i = 0;

		for (char c : "Robofootball") {
			str[i++] = c;
		}
        rcconfig.name = str;

        rcconfig.RULE_AMOUNT=5;
        rcconfig.RULE_LENGTH=7;
        rcconfig.BACK_AMOUNT=10;
        rcconfig.BACK_LENGTH=8;

    }

    MlData mtl(rcconfig);
    fmldata = mtl;

    run_matlab();
}

void MainAlgWorker::run_matlab()
{
    cout << "Work is started..." << endl;

    if (!(fmldata.ep = engOpen(NULL))) {
        cerr << "Can't open Matlab Engine" << endl;
        fmtlab = false;
        return;
    }

    m_buffer[255] = '\0';
    engOutputBuffer(fmldata.ep, m_buffer, 255);
    printf("Matlab Engine is opened\n");

    // Path_Of_Files
   // TCHAR CurrentPath[MAX_PATH];
   // char StringWithPath[MAX_PATH],StringPath[MAX_PATH];

    /*
    // Detecting Directory
    printf("Start to detect directory\n");
    GetCurrentDirectory(sizeof(CurrentPath), CurrentPath);

#ifdef UNICODE
    wcstombs(StringPath, CurrentPath, MAX_PATH - 1);
#else
    CharToOem(StringPath, CurrentPath);
#endif

    sprintf(StringWithPath, "cd %s;", StringPath);
    engEvalString(fmldata.ep, StringWithPath);
    printf("We are on %s\n",StringWithPath);
*/
    //-----create Rules-----
    char sendString[256];
    sprintf (sendString, "Rules=zeros(%d, %d)", fmldata.config.RULE_AMOUNT, fmldata.config.RULE_LENGTH);
    engEvalString(fmldata.ep, sendString);
//    engEvalString(fmldata.ep, "disp(1)");

    QString dirPath = "cd " + QCoreApplication::applicationDirPath() + "/MLscripts";
    engEvalString(fmldata.ep, dirPath.toUtf8().data());
    fmtlab = true;
    pause = false;
}

void MainAlgWorker::stop_matlab()
{
    fmtlab = false;
}
void MainAlgWorker::EvalString(QString s)
{
    engEvalString(fmldata.ep,s.toUtf8().data());
}
//void MainAlgWorker::Pause()
//{
//    if (pause = !pause) {}
//    engEvalString(fmldata.ep, "PAUSE();");
//}

MainAlgWorker::MainAlgWorker()
{
        timer_s=0;
        timer_m=clock();
        Time_count=0;
        QFile addrFile("gamepads.txt");
        if (!addrFile.open(QIODevice::ReadOnly)) {
            qDebug() << "File with addresses is not opened!!!";
        }

        QTextStream in(&addrFile);
        auto allAddrs = in.readAll().split("\n", QString::SkipEmptyParts).filter(QRegExp("^[^#;]"));
        client.initFromList(allAddrs);

        for (int i=0; i<MAX_NUM_ROBOTS; i++)
        {
            Send2BT[i]=true;
        }
}

void MainAlgWorker::Send2BTChangeit(bool * send2BT_)
{
    for (int i=0; i<MAX_NUM_ROBOTS; i++)
    {
        Send2BT[i]=send2BT_[i];
    }
}
void MainAlgWorker::run(PacketSSL packetssl)
{
    if (shutdowncomp)
        return;
    timer = clock();
    Time_count++;

// Заполнение массивов Balls Blues и Yellows и запуск main-функции

    memcpy(mxGetPr(fmldata.Ball), packetssl.balls, BALL_COUNT_d);
    memcpy(mxGetPr(fmldata.Blue), packetssl.robots_blue, TEAM_COUNT_d);
    memcpy(mxGetPr(fmldata.Yellow), packetssl.robots_yellow, TEAM_COUNT_d);

    engPutVariable(fmldata.ep, "Balls", fmldata.Ball);
    engPutVariable(fmldata.ep, "Blues", fmldata.Blue);
    engPutVariable(fmldata.ep, "Yellows", fmldata.Yellow);


    engEvalString(fmldata.ep, fmldata.config.file_of_matlab);

// Забираем Rules и очищаем его в воркспейсе

    fmldata.Rule = engGetVariable(fmldata.ep, "Rules");
    double *ruleArray = (double *)malloc(fmldata.config.RULE_AMOUNT * fmldata.config.RULE_LENGTH * sizeof(double));
    if (fmldata.Rule!=0)
        memcpy(ruleArray, mxGetPr(fmldata.Rule), fmldata.config.RULE_AMOUNT * fmldata.config.RULE_LENGTH * sizeof(double));
    else
        memset(ruleArray,0,fmldata.config.RULE_AMOUNT * fmldata.config.RULE_LENGTH * sizeof(double));

    char sendString[256];
    sprintf(sendString, "Rules=zeros(%d, %d);", fmldata.config.RULE_AMOUNT, fmldata.config.RULE_LENGTH);
    engEvalString(fmldata.ep, sendString);

// Разбор пришедшего пакета и переправка его строк на connector и BT

    for (int i = 0; i < fmldata.config.RULE_AMOUNT; i++) {
        char newmess[100];
        for (int j = 0; j < fmldata.config.RULE_LENGTH; j++) {
            newmess[j]=ruleArray[j * fmldata.config.RULE_AMOUNT + i];
        }

        if (newmess[0]==1)
        {

            char * newmessage=new char[100];
            memcpy(newmessage,newmess,100);
            if ((newmess[1]>=0) && (newmess[1]<=MAX_NUM_ROBOTS) && ((newmess[1]==0) || (Send2BT[newmess[1]-1]==true)))
                emit sendToBTtransmitter(newmessage);

            Message msg;
            msg.setKickVoltageLevel(12);
            msg.setKickerChargeEnable(1);

            if (!isPause) {
               if (newmess[1]>0){
                   qDebug("MESSAGE %d, %d, %d", (int)newmess[2], (int)newmess[3], (int)newmess[5]);
                   grSimClient.sendCommand((int)newmess[2]/100.0, (int)newmess[3]/100.0, (int)newmess[5]/100.0,
                                           0,0, (int)newmess[1]);

                }

                msg.setSpeedX(newmess[2]);
                msg.setSpeedY(newmess[3]);
                msg.setSpeedR(newmess[5]);


              
                msg.setKickForward(newmess[4]);


            } else {
                msg.setSpeedX(0);
                msg.setSpeedY(0);
                msg.setSpeedR(0);
              
                msg.setKickForward(0);
            }
          
//            if (newmess[8]) {
//                QString stop_sig = "1";
//                client.writeData(stop_sig.toUtf8());
//            }


            QByteArray command = msg.generateByteArray();

            if (newmess[1]==0)
                for (int i=1; i<=MAX_NUM_ROBOTS; i++){
                    grSimClient.sendCommand(msg.getSpeedX(), msg.getSpeedY(), msg.getSpeedR(),
                                            5*msg.getKickForward(), 5*msg.getKickUp(), i);
                    emit sendToConnector(i,command);
                }
            if ((newmess[1]>0) && (newmess[1]<=MAX_NUM_ROBOTS))
            {

                qDebug("MESSAGE");
               // grSimClient.sendCommand(msg.getSpeedX(), msg.getSpeedY(), msg.getSpeedR(),
                //                        5*msg.getKickForward(), 5*msg.getKickUp(), newmess[1]);
                emit sendToConnector(newmess[1],command);
            }
        }
    }

// Раз в секунду создание пакета статистики и переправка в GUI

    clock_t timer_c=clock()-timer;
    if (timer_c>timer_max)
        timer_max=timer_c;
    timer_s=timer_s+timer_c;
    if (clock()-timer_m>CLOCKS_PER_SEC)
    {
        timer_m=clock();
        QString temp;
        QString ToStatus="Using Matlab: Count=";
        temp.setNum(Time_count);
        ToStatus=ToStatus+temp;

        ToStatus=ToStatus+" ~time=";
        temp.setNum(timer_s/Time_count);
        ToStatus=ToStatus+temp;

        ToStatus=ToStatus+" maxtime=";
        temp.setNum(timer_max);
        ToStatus=ToStatus+temp;

        ToStatus=ToStatus+" fulltime=";
        temp.setNum(timer_s);
        ToStatus=ToStatus+temp;

        timer_s=0;
        timer_max=0;
        Time_count=0;
        emit StatusMessage(ToStatus);

        engEvalString(fmldata.ep,"ispause=RP.Pause");
        mxArray *mxitpause=engGetVariable(fmldata.ep,"ispause");
        isPause = true;
        if (mxitpause!=0)
        {
            double *itpause=mxGetPr(mxitpause);
            if (itpause!=0)
            {
                if ((*itpause)==0)
                {
                    mxArray *mxzMain_End=engGetVariable(fmldata.ep,"zMain_End");
                    if (mxzMain_End!=0)
                    {
                        double *zMain_End=mxGetPr(mxzMain_End);
                        if (zMain_End!=0)
                        {
                            if ((*zMain_End)==0)
                                emit UpdatePauseState("main br");
                            else {
                                emit UpdatePauseState("WORK");
                                isPause = false;
                            }
                        }
                        else emit UpdatePauseState("-err-z");
                    }
                    else emit UpdatePauseState("-err-mz");
                }
                else emit UpdatePauseState("PAUSE");
            }
            else emit UpdatePauseState("-err-p"); //Ответ от матлаба повреждён
        }
        else emit UpdatePauseState("-err-mp"); //Нет ответа от матлаб
    }

    if (isPause) {
        Message msg;
        msg.setKickVoltageLevel(12);
        msg.setKickerChargeEnable(1);

        msg.setSpeedX(0);
        msg.setSpeedY(0);
        msg.setSpeedR(0);

        msg.setKickForward(0);



        QByteArray command = msg.generateByteArray();
        for (int i = 1; i <= 12; i++) {
            emit sendToConnector(i, command);
            grSimClient.sendCommand(0, 0, 0, 0, 0, i - 1);
            qDebug("Pause command sent");
        }
    }

// Сообщение ресиверу о готовности обработки нового пакета.

    QApplication::processEvents();
    emit mainAlgFree();
//    qDebug()<<"MainAlg!";

}

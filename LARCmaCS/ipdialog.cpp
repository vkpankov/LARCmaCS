#include "ipdialog.h"
#include "ui_ipdialog.h"

IpDialog::IpDialog(ConnectorWorker &worker,  QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IpDialog)
{
    receiver.init();
    QObject::connect(this, SIGNAL(addIp(int, QString)),
                         &worker, SLOT(addIp(int, QString)));

    ui->setupUi(this);
    ui->lineEditIp1->setText(worker.numIP[1]);
    ui->lineEditIp2->setText(worker.numIP[2]);
    ui->lineEditIp3->setText(worker.numIP[3]);
    ui->lineEditIp4->setText(worker.numIP[4]);
    ui->lineEditIp5->setText(worker.numIP[5]);
    ui->lineEditIp6->setText(worker.numIP[6]);
    ui->lineEditIp7->setText(worker.numIP[7]);
    ui->lineEditIp8->setText(worker.numIP[8]);
    ui->lineEditIp9->setText(worker.numIP[9]);
    ui->lineEditIp10->setText(worker.numIP[10]);
    ui->lineEditIp11->setText(worker.numIP[11]);
    ui->lineEditIp12->setText(worker.numIP[12]);

    qDebug() << connector.worker.numIP;

}

IpDialog::~IpDialog()
{
    delete ui;
}


void IpDialog::on_buttonBox_accepted()
{
    emit(addIp(1, ui->lineEditIp1->text()));
    emit(addIp(2, ui->lineEditIp2->text()));
    emit(addIp(3, ui->lineEditIp3->text()));
    emit(addIp(4, ui->lineEditIp4->text()));
    emit(addIp(5, ui->lineEditIp5->text()));
    emit(addIp(6, ui->lineEditIp6->text()));
    emit(addIp(7, ui->lineEditIp7->text()));
    emit(addIp(8, ui->lineEditIp8->text()));
    emit(addIp(9, ui->lineEditIp9->text()));
    emit(addIp(10, ui->lineEditIp10->text()));
    emit(addIp(11, ui->lineEditIp11->text()));
    emit(addIp(12, ui->lineEditIp12->text()));

    this->close();

    qDebug("OK");
}

void IpDialog::on_buttonBox_rejected()
{
    this->close();
}

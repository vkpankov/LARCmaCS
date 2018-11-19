#include "reference.h"
#include "ui_reference.h"
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

Reference::Reference(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Help)
{
    ui->setupUi(this);
}

Reference::~Reference()
{
    delete ui;
}

#include "reference.h"
#include "ui_reference.h"

Reference::Reference(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
}

Reference::~Reference()
{
    delete ui;
}


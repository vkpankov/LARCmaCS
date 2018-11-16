#include "reference.h"
#include "ui_reference.h"
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

Reference::Reference(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);

    QFile file(QCoreApplication::applicationDirPath() + "\\help.html");

    if(!file.open(QIODevice ::ReadOnly)) {
        QMessageBox::information(0, "File not found!", file.errorString());
    }

    QTextStream in(&file);

    ui->textBrowser->setText(in.readAll());
}

Reference::~Reference()
{
    delete ui;
}

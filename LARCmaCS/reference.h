#pragma once

#include <QWidget>

namespace Ui {
class Form;
}

class Reference : public QWidget {
    Q_OBJECT

public:
    explicit Reference(QWidget *parent = 0);
    ~Reference();
signals:
    void showReference();
private slots:
    //void on_pushButton_clicked();
private:
    Ui::Form *ui;
};

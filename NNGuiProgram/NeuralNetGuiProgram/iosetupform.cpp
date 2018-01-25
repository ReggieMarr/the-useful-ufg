#include "iosetupform.h"
#include "ui_iosetupform.h"

IOSetupForm::IOSetupForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IOSetupForm)
{
    ui->setupUi(this);

    ui->IOTable->setRowCount(2);
    ui->IOTable->setColumnCount(3);
    ui->IOTable->setHorizontalHeaderLabels(QString("Tag Name;Input;Output").split(";"));
}

IOSetupForm::~IOSetupForm()
{
    delete ui;
}

#include "dataloggerwindow.h"
#include "ui_dataloggerwindow.h"
#include "QCloseEvent"
dataLoggerWindow::dataLoggerWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::dataLoggerWindow)
{
    ui->setupUi(this);

    ui->sourceTypeCombo->addItem("CSV File");
    ui->sourceTypeCombo->addItem("Json File");
    ui->sourceTypeCombo->addItem("MySQL Database");
}

dataLoggerWindow::~dataLoggerWindow()
{
    delete ui;
}

void dataLoggerWindow::on_historicalSetupBtn_clicked()
{

}

void dataLoggerWindow::on_sourceTypeCombo_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
        ui->editDatabase->setVisible(false);
        ui->editHostname->setVisible(false);
        ui->editPassword->setVisible(false);
        ui->portSpinBox->setVisible(false);
        ui->textLabel2->setVisible(false);
        ui->textLabel3->setVisible(false);
        ui->textLabel4->setVisible(false);
        ui->textLabel4_2->setVisible(false);
        ui->textLabel5->setVisible(false);
        ui->textLabel5_2->setVisible(false);
        ui->editUsername->setVisible(false);

        break;
    case 1:
        ui->editDatabase->setVisible(false);
        ui->editHostname->setVisible(false);
        ui->editPassword->setVisible(false);
        ui->portSpinBox->setVisible(false);
        ui->textLabel2->setVisible(false);
        ui->textLabel3->setVisible(false);
        ui->textLabel4->setVisible(false);
        ui->textLabel4_2->setVisible(false);
        ui->textLabel5->setVisible(false);
        ui->textLabel5_2->setVisible(false);
        ui->editUsername->setVisible(false);
        break;
    case 2:
        ui->editDatabase->setVisible(true);
        ui->editHostname->setVisible(true);
        ui->editPassword->setVisible(true);
        ui->portSpinBox->setVisible(true);
        ui->textLabel2->setVisible(true);
        ui->textLabel3->setVisible(true);
        ui->textLabel4->setVisible(true);
        ui->textLabel4_2->setVisible(true);
        ui->textLabel5->setVisible(true);
        ui->textLabel5_2->setVisible(true);
        ui->editUsername->setVisible(true);
        break;
    default:
        break;
    }
}

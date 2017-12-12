#include "modbussetupwindow.h"
#include "ui_modbussetupwindow.h"
//#include "qextserialport.h"
#include "qextserialenumerator.h"
#include "QSettings"

modbusSetupWindow::modbusSetupWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::modbusSetupWindow)
{
    QSettings s;

    ui->setupUi(this);

    portIndex = 0;
    int i = 0;

    ui->serialPort->disconnect();
    ui->serialPort->clear();

    foreach (QextPortInfo port, QextSerialEnumerator::getPorts()) {
#ifdef Q_OS_WIN
        ui->serialPort->addItem( port.friendName );
#else
        ui->serialPort->addItem( port.physName );
#endif
        if( port.friendName == s.value( "serialinterface" ) )
        {
            portIndex = i;
        }
        ++i;

    }
    ui->serialPort->setCurrentIndex( portIndex );

    ui->baud->addItem("1200");
    ui->baud->addItem("4800");
    ui->baud->addItem("9600");
    ui->baud->addItem("19200");
    ui->baud->addItem("38400");
    ui->baud->addItem("57600");
    ui->baud->addItem("115200");

    ui->dataBits->addItem("7");
    ui->dataBits->addItem("8");

    ui->stopBits->addItem("1");
    ui->stopBits->addItem("1.5");
    ui->stopBits->addItem("2");

    ui->parity->addItem("Odd");
    ui->parity->addItem("Even");
}

modbusSetupWindow::~modbusSetupWindow()
{
    delete ui;
}

void modbusSetupWindow::on_setupConnectionBtn_clicked()
{

}

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

modbusSetupWindow::modbusSetupWindow(QWidget *parent, modbusConfig &modbusConnConfig) :
    QMainWindow(parent),
    ui(new Ui::modbusSetupWindow)
{
    //this is just to compensate for not having a default
    parent = 0;


    //modbusConnConfig = userConfig;
    ui->setupUi(this);

    portIndex = 0;
    int i = 0;
        QSettings s;

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

    modbusConnConfig = userConfig;
}

modbusSetupWindow::~modbusSetupWindow()
{
    delete ui;
}

void modbusSetupWindow::on_setupConnectionBtn_clicked()
{
    //apparently this is how to parse QString to char * and int
    QString tempQString = ui->serialPort->currentText();
    QByteArray tempByteArray = tempQString.toLatin1();
    userConfig.serialPortOrIPAddress = tempByteArray.data();
    userConfig.baudOrTCPPort = ui->baud->currentText().toInt();
    userConfig.dataBits = ui->dataBits->currentText().toInt();
    userConfig.stopBits = ui->stopBits->currentText().toFloat();
    tempQString = ui->parity->currentText();
    tempByteArray = tempQString.toLatin1();
    userConfig.parity = tempByteArray.data();
}

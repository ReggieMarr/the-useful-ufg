#include "modbussetupwindow.h"
#include "ui_modbussetupwindow.h"
#include "qextserialport.h"
#include "qextserialenumerator.h"
#include "QSettings"
#include "exception"
#include "iostream"
#include "modbus/modbus.h"
#include "modbus/modbus-tcp.h"
#include "modbus/modbus-version.h"

modbusSetupWindow::modbusSetupWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::modbusSetupWindow)
{
    QSettings s;
    //userConfig = modbusConnConfig;
    //userConfig = {"/dev/ttyS0",9600,8,1.5,"Odd"};

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

modbusSetupWindow::modbusSetupWindow(QWidget *parent, modbusConfig *modbusConnConfig, int connType) :
    QMainWindow(parent),
    ui(new Ui::modbusSetupWindow)
{
    userConfig = modbusConnConfig;

    //this is just to compensate for not having a default
    parent = 0;



    userConfig->serialPortOrIPAddress = "/dev/ttyS0";
    userConfig->baudOrTCPPort = 9600;
    userConfig->dataBits = 8;
    userConfig->stopBits=1.5;
    userConfig->parity = "Odd";

    ui->setupUi(this);

    portIndex = 0;
    int i = 0;
    QSettings s;

    if(connType > 1)
    {
        modbusEthernetMode = false;
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

        ui->editIPClassifier->setVisible(false);
        ui->editIPAdd2->setVisible(false);
        ui->editIPAdd3->setVisible(false);
        ui->editIPAdd4->setVisible(false);
        ui->editPort->setVisible(false);
        ui->portLabel_2->setVisible(false);

        modbusConnConfig = userConfig;
        //userConfig = &modbusConnConfig;
    }
    else
    {
        ui->setupConnectionBtn->setVisible(false);
        modbusEthernetMode = true;

        ui->editIPClassifier->setVisible(true);
        ui->editIPAdd2->setVisible(true);
        ui->editIPAdd3->setVisible(true);
        ui->editIPAdd4->setVisible(true);
        ui->editPort->setVisible(true);
        ui->portLabel_2->setVisible(true);

        ui->parity->setVisible(false);
        ui->stopBits->setVisible(false);
        ui->dataBits->setVisible(false);
        ui->baud->setVisible(false);
        ui->serialPort->setVisible(false);
        ui->parityLabel->setVisible(false);
        ui->stopBitsLabel->setVisible(false);
        ui->dataBitsLabel->setVisible(false);
        ui->baudLabel->setVisible(false);
        ui->portLabel->setText("IP Address:");
    }


}

modbusSetupWindow::~modbusSetupWindow()
{
    delete userConfig;
    delete ui;
}

void TCPConnTry(char * IP,int portNum)
{
    uint8_t dest[1024];
    uint16_t * dest16 = (uint16_t *) dest;

    modbus_t * m_modbus = modbus_new_tcp(IP, portNum);
    modbus_connect(m_modbus);

    memset( dest, 0, 1024 );

    modbus_set_slave( m_modbus, 1 );
}

void modbusSetupWindow::on_setupConnectionBtn_clicked()
{
    //this is how to parse QString to char * and int
    //this is no longer neccesary here, at least for now
    //as the userConfig stores QStrings rather than char *
    //but this must be changed when actually making the modbus connection
    QString tempQString = ui->serialPort->currentText();
    QByteArray tempByteArray = tempQString.toLatin1();


    userConfig->serialPortOrIPAddress = inputIP;//ui->serialPort->currentText();//tempByteArray.data();
    userConfig->baudOrTCPPort = inputPort.toInt(nullptr,10);//ui->baud->currentText().toInt();
    userConfig->dataBits = ui->dataBits->currentText().toInt();
    userConfig->stopBits = ui->stopBits->currentText().toFloat();
    //tempQString = ui->parity->currentText();
    //tempByteArray = tempQString.toLatin1();
    userConfig->parity = ui->parity->currentText();//tempByteArray.data();


    if(modbusEthernetMode)
    {
        uint8_t dest[1024];
        uint16_t * dest16 = (uint16_t *) dest;

        tempQString = userConfig->serialPortOrIPAddress;//ui->parity->currentText();
        tempByteArray = tempQString.toLatin1();
        char * IPAddress = tempByteArray.data();//"192.168.2.80";
        int modbusPortNum = userConfig->baudOrTCPPort;//502;

        try
        {
            //throw TCPConnTry(IPAddress,modbusPortNum);
            modbus_t * m_modbus = modbus_new_tcp(IPAddress, modbusPortNum);
            modbus_connect(m_modbus);

            memset( dest, 0, 1024 );

            modbus_set_slave( m_modbus, 1 );
        }
        catch(std::exception& e)
        {
            std::cout << "Exception found: " << e.what() << std::endl;
        }



    }
    else
    {

    }

}

void modbusSetupWindow::settext(const QString &t)
{
    QStringList sl = t.split(QChar('.'));
    for (int i=0; i < sl.size(); i++) {
        switch(i) {
        case 0: ui->editIPClassifier->setText(sl[0]); break;
        case 1: ui->editIPAdd2->setText(sl[1]); break;
        case 2: ui->editIPAdd3->setText(sl[2]); break;
        case 3: ui->editIPAdd4->setText(sl[3]); break;
        default: break;
        }
    }
    emit textChanged(text());
}

QString modbusSetupWindow::text() const
{
    return ui->editIPClassifier->text() + QChar('.') + ui->editIPAdd2->text() + QChar('.')
         + ui->editIPAdd3->text() + QChar('.') + ui->editIPAdd4->text();
}

void modbusSetupWindow::on_editIPClassifier_textChanged(const QString &arg1)
{
    if (arg1.length() == 3) {
        ui->editIPAdd2->setFocus(Qt::OtherFocusReason);
    }
    emit textChanged(text());
}

void modbusSetupWindow::on_editIPAdd2_textChanged(const QString &arg1)
{
    if (arg1.length() == 3) {
        ui->editIPAdd3->setFocus(Qt::OtherFocusReason);
    }
    emit textChanged(text());
}

void modbusSetupWindow::on_editIPAdd3_textChanged(const QString &arg1)
{
    if (arg1.length() == 3) {
        ui->editIPAdd4->setFocus(Qt::OtherFocusReason);
    }
    emit textChanged(text());
}

void modbusSetupWindow::on_editIPAdd4_textChanged(const QString &arg1)
{
    if (arg1.length() == 3) {
        ui->editPort->setFocus(Qt::OtherFocusReason);

    }
    emit textChanged(text());
}

void modbusSetupWindow::on_editPort_textChanged(const QString &arg1)
{
    if (arg1.length() > 2)
    {
        ui->setupConnectionBtn->setVisible(true);
        inputPort = arg1;
        inputIP = ui->editIPClassifier->text() + "." + ui->editIPAdd2->text()
                + "." + ui->editIPAdd3->text() + "." + ui->editIPAdd4->text();

    }
}

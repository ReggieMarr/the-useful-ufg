#include "runtime_window.h"
#include "ui_runtime_window.h"
#include "iostream"

const int DataTypeColumn = 0;
const int AddrColumn = 1;
const int DataColumn = 2;

runtime_Window::runtime_Window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::runtime_Window)
{
    ui->setupUi(this);
    QWidget::showMaximized();

    ui->controlSelectComboBox->addItem("Predictive Control");
    ui->controlSelectComboBox->addItem("Static Adaptive Control");
    ui->controlSelectComboBox->addItem("Dynamic Adaptive Control");
    ui->controlSelectComboBox->setCurrentIndex(0);

    ui->interactionSelect->addItem("Offline");
    ui->interactionSelect->addItem("Modbus");
    connectionType = 0;
    ui->tabWidget->setCurrentIndex(0);
    //ui->interactionSelect->setCurrentIndex(connectionType);
    //ui->tabWidget->setTabBarAutoHide(true);
    ui->tabWidget->setTabEnabled(1,false);
    ui->tabWidget->setTabEnabled(2,false);
    processTypeOpened = false;

}

runtime_Window::~runtime_Window()
{
    delete ui;
}

void runtime_Window::on_controlSelectComboBox_currentIndexChanged(int index)
{

}

void runtime_Window::on_IPEdit0_textChanged(const QString &arg1)
{
    if (arg1.length() == 3) {
        ui->IPEdit1->setFocus(Qt::OtherFocusReason);
    }
    IPAddress[0] = arg1;
}

void runtime_Window::on_IPEdit1_textChanged(const QString &arg1)
{
    if (arg1.length() == 3) {
        ui->IPEdit2->setFocus(Qt::OtherFocusReason);
    }
    IPAddress[1] = arg1;
}

void runtime_Window::on_IPEdit2_textChanged(const QString &arg1)
{
    if (arg1.length() == 3) {
        ui->IPEdit3->setFocus(Qt::OtherFocusReason);
    }
    IPAddress[2] = arg1;
}

void runtime_Window::on_IPEdit3_textChanged(const QString &arg1)
{
    if (arg1.length() == 3) {
        ui->portEdit->setFocus(Qt::OtherFocusReason);
    }
    IPAddress[3] = arg1;
}

void runtime_Window::on_portEdit_textChanged(const QString &arg1)
{
    if (arg1.length() == 3) {
        ui->connectBtn->setFocus(Qt::OtherFocusReason);
    }
   portNum = arg1.toInt();
}

void runtime_Window::on_connectBtn_clicked()
{

    QString tempQString = IPAddress[0] + "." + IPAddress[1] + "." +IPAddress[2] + "." +IPAddress[3];
    QByteArray tempByteArray = tempQString.toLatin1();
    char * ModbusIP = tempByteArray.data();//"192.168.2.80";

    releaseTcpModbus();

    m_tcpModbus = modbus_new_tcp( ModbusIP, portNum );
    if( modbus_connect( m_tcpModbus ) == -1 )
    {
        //emit connectionError( tr( "Could not connect tcp/ip port!" ) );

        std::cout << "Could not connect tcp/ip port!" << std::endl;

        releaseTcpModbus();
    }
}

void runtime_Window::updateRegisterView( void )
{

    const int addr = ui->startingAddressSelectBox->value();

    int rowCount = 0;
    switch( ui->messageBox->currentIndex())
    {
        case 4:
        //Write Single Coil (0x05)
        case 5:
        //Write Single Register(0x06)
            ui->numberofCoilsSpin->setEnabled( false );
            rowCount = 1;
            break;
        case 6:
        //Write Multiple Coils (0x0f)
        case 7:
        //Write Multiple Registers (0x10)
            rowCount = ui->numberofCoilsSpin->value();
        default:
            ui->numberofCoilsSpin->setEnabled( true );
            break;
    }

    ui->dataDisplayTable->setRowCount( rowCount );
    for( int i = 0; i < rowCount; ++i )
    {
        QTableWidgetItem * dtItem = new QTableWidgetItem( ui->messageBox->currentIndex() );
        QTableWidgetItem * addrItem =
            new QTableWidgetItem( QString::number( addr+i ) );
        QTableWidgetItem * dataItem =
            new QTableWidgetItem( QString::number( 0 ) );
        dtItem->setFlags( dtItem->flags() & ~Qt::ItemIsEditable	);
        addrItem->setFlags( addrItem->flags() & ~Qt::ItemIsEditable );
        ui->dataDisplayTable->setItem( i, DataTypeColumn, dtItem );
        ui->dataDisplayTable->setItem( i, AddrColumn, addrItem );
        ui->dataDisplayTable->setItem( i, DataColumn, dataItem );
    }

    //ui->dataDisplayTable->setColumnWidth( 0, 150 );
}

void runtime_Window::on_sendDataBtn_clicked()
{

    if( m_tcpModbus == NULL )
    {
        return;
    }

    const int slave = ui->IDValBox->value();

    const int addr = ui->startingAddressSelectBox->value();
    int num = ui->numberofCoilsSpin->value();
    uint8_t dest[1024];
    uint16_t * dest16 = (uint16_t *) dest;

    memset( dest, 0, 1024 );

    int ret = -1;
    bool is16Bit = false;
    bool writeAccess = false;
    //const QString dataType = descriptiveDataTypeName( func );

    modbus_set_slave( m_tcpModbus, slave );

    switch (ui->messageBox->currentIndex()) {
    case 0:
    {
        //Read Coils (0x01)
        ret = modbus_read_bits( m_tcpModbus, addr, num, dest );
        break;
    }
    case 1:
    {
        //Read Discrete Inputs (0x02)
        ret = modbus_read_input_bits( m_tcpModbus, addr, num, dest );
        break;
    }
    case 2:
    {
        //Read Holding Registers (0x03)
        ret = modbus_read_registers( m_tcpModbus, addr, num, dest16 );
        is16Bit = true;
        break;
    }
    case 3:
    {
        //Read Input Registers (0x04)
        ret = modbus_read_input_registers( m_tcpModbus, addr, num, dest16 );
        is16Bit = true;
        break;
    }
    case 4:
    {
        //Write Single Coil (0x05)
        ret = modbus_write_bit( m_tcpModbus, addr,
                ui->dataDisplayTable->item( 0, DataColumn )->
                    text().toInt(0, 0) ? 1 : 0 );
        writeAccess = true;
        num = 1;
        break;
    }
    case 5:
    {
        //Write Single Register(0x06)
        ret = modbus_write_register( m_tcpModbus, addr,
                ui->dataDisplayTable->item( 0, DataColumn )->
                    text().toInt(0, 0) );
        writeAccess = true;
        num = 1;
        break;
    }
    case 6:
    {
        //Write Multiple Coils (0x0f)
        uint8_t * data = new uint8_t[num];
        for( int i = 0; i < num; ++i )
        {
            data[i] = ui->dataDisplayTable->item( i, DataColumn )->
                            text().toInt(0, 0);
        }
        ret = modbus_write_bits( m_tcpModbus, addr, num, data );
        delete[] data;
        writeAccess = true;
        break;
    }
    case 7:
    {
        //Write Multiple Registers (0x10)
        uint16_t * data = new uint16_t[num];
        for( int i = 0; i < num; ++i )
        {
            data[i] = ui->dataDisplayTable->item( i, DataColumn )->
                            text().toInt(0, 0);
        }
        ret = modbus_write_registers( m_tcpModbus, addr, num, data );
        delete[] data;
        writeAccess = true;
        break;
    }

    }
}

void runtime_Window::on_tabWidget_currentChanged(int index)
{
    switch (index) {
    case 0:
        if(processTypeOpened)
        {
            processTypeOpened = false;
            ui->connectionSelectionBox->removeItem(1);
            ui->connectionSelectionBox->removeItem(0);

            ui->messageBox->removeItem(7);
            ui->messageBox->removeItem(6);
            ui->messageBox->removeItem(5);
            ui->messageBox->removeItem(4);
            ui->messageBox->removeItem(3);
            ui->messageBox->removeItem(2);
            ui->messageBox->removeItem(1);
            ui->messageBox->removeItem(0);
        }

        ui->interactionSelect->setCurrentIndex(connectionType);
        break;
    case 1:
        if(!processTypeOpened)
        {
//            for(int i = 0; i< ui->connectionSelectionBox->maxVisibleItems();i++)
//            {
//                ui->connectionSelectionBox->removeItem(i);
//            }
            ui->connectionSelectionBox->addItem("Offline");
            ui->connectionSelectionBox->addItem("Modbus");
            ui->connectionSelectionBox->setCurrentIndex(connectionType);

            ui->messageBox->addItem("Read Coils (0x01)");
            ui->messageBox->addItem("Read Discrete Inputs (0x02)");
            ui->messageBox->addItem("Read Holding Registers (0x03)");
            ui->messageBox->addItem("Read Input Registers (0x03)");
            ui->messageBox->addItem("Write Single Coil (0x04)");
            ui->messageBox->addItem("Write Single Register(0x05)");
            ui->messageBox->addItem("Write Multiple Coils (0x0f)");
            ui->messageBox->addItem("Write Multiple Registers (0x10)");
            ui->messageBox->setCurrentText(0);

            processTypeOpened = true;
        }

        break;
    default:
        break;
    }
}

void runtime_Window::on_connectionSelectionBox_currentIndexChanged(int index)
{
    if(index == 0 && processTypeOpened)
    {
        connectionType = index;
        ui->tabWidget->setCurrentIndex(0);
        ui->tabWidget->setTabEnabled(1,false);
    }
    else if (index != 0 && processTypeOpened)
    {
        connectionType = index;
    }
}

void runtime_Window::on_interactionSelect_currentIndexChanged(int index)
{
    if(index > 0)
    {
        ui->tabWidget->setTabEnabled(1,true);
    }
    connectionType = index;

}

void runtime_Window::releaseTcpModbus()
{
    if( m_tcpModbus )
    {
        modbus_close( m_tcpModbus );
        modbus_free( m_tcpModbus );
        m_tcpModbus = NULL;
    }
}

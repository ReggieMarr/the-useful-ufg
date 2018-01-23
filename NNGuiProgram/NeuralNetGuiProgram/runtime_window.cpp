#include "runtime_window.h"
#include "ui_runtime_window.h"
#include "iostream"
#include <QSqlDatabase>
#include "QSqlQuery"
#include "QSqlError"
#include "QSqlDriver"
#include "QSqlRecord"
#include "QSqlField"
#include "qsqlconnectiondialog.h"


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



    QStringList drivers = QSqlDatabase::drivers();

    // remove compat names
    drivers.removeAll("QMYSQL3");
    drivers.removeAll("QOCI8");
    drivers.removeAll("QODBC3");
    drivers.removeAll("QPSQL7");
    drivers.removeAll("QTDS7");

//    if (!drivers.contains("QSQLITE"))
//        ui.dbCheckBox->setEnabled(false);





    if (QSqlDatabase::drivers().isEmpty())
        QMessageBox::information(this, tr("No database drivers found"),
                                 tr("This demo requires at least one Qt database driver. "
                                    "Please check the documentation how to build the "
                                    "Qt SQL plugins."));

}

runtime_Window::~runtime_Window()
{
    delete ui;
}

void runtime_Window::exec()
{
    QSqlQueryModel *model = new QSqlQueryModel(ui->historicalDataTableView);
    model->setQuery(QSqlQuery(ui->sqlEdit->toPlainText(), ui->connectionWidget->currentDatabase()));
    ui->historicalDataTableView->setModel(model);


    if (model->lastError().type() != QSqlError::NoError)
        emit statusMessage(model->lastError().text());
    else if (model->query().isSelect())
        emit statusMessage(tr("Query OK."));
    else
        emit statusMessage(tr("Query OK, number of affected rows: %1").arg(
                           model->query().numRowsAffected()));

    updateActions();
}

QSqlError runtime_Window::addConnection(const QString &driver, const QString &dbName, const QString &host,
                            const QString &user, const QString &passwd, int port)
{
    static int cCount = 0;

    QSqlError err;
    QSqlDatabase db = QSqlDatabase::addDatabase(driver, QString("Browser%1").arg(++cCount));
    db.setDatabaseName(dbName);
    db.setHostName(host);
    db.setPort(port);
    if (!db.open(user, passwd)) {
        err = db.lastError();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase(QString("Browser%1").arg(cCount));
    }
    ui->connectionWidget->refresh();

    return err;
}

void runtime_Window::addConnection()
{
    QSqlConnectionDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    if (dialog.useInMemoryDatabase()) {
        QSqlDatabase::database("in_mem_db", false).close();
        QSqlDatabase::removeDatabase("in_mem_db");
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "in_mem_db");
        db.setDatabaseName(":memory:");
        if (!db.open())
            QMessageBox::warning(this, tr("Unable to open database"), tr("An error occurred while "
                                                                         "opening the connection: ") + db.lastError().text());
        QSqlQuery q("", db);
        q.exec("drop table Movies");
        q.exec("drop table Names");
        q.exec("create table Movies (id integer primary key, Title varchar, Director varchar, Rating number)");
        q.exec("insert into Movies values (0, 'Metropolis', 'Fritz Lang', '8.4')");
        q.exec("insert into Movies values (1, 'Nosferatu, eine Symphonie des Grauens', 'F.W. Murnau', '8.1')");
        q.exec("insert into Movies values (2, 'Bis ans Ende der Welt', 'Wim Wenders', '6.5')");
        q.exec("insert into Movies values (3, 'Hardware', 'Richard Stanley', '5.2')");
        q.exec("insert into Movies values (4, 'Mitchell', 'Andrew V. McLaglen', '2.1')");
        q.exec("create table Names (id integer primary key, Firstname varchar, Lastname varchar, City varchar)");
        q.exec("insert into Names values (0, 'Sala', 'Palmer', 'Morristown')");
        q.exec("insert into Names values (1, 'Christopher', 'Walker', 'Morristown')");
        q.exec("insert into Names values (2, 'Donald', 'Duck', 'Andeby')");
        q.exec("insert into Names values (3, 'Buck', 'Rogers', 'Paris')");
        q.exec("insert into Names values (4, 'Sherlock', 'Holmes', 'London')");
        ui->connectionWidget->refresh();
    } else {
        QSqlError err = addConnection(dialog.driverName(), dialog.databaseName(), dialog.hostName(),
                           dialog.userName(), dialog.password(), dialog.port());
        if (err.type() != QSqlError::NoError)
            QMessageBox::warning(this, tr("Unable to open database"), tr("An error occurred while "
                                       "opening the connection: ") + err.text());
    }
}

void runtime_Window::showTable(const QString &t)
{
    QSqlTableModel *model = new CustomModel(ui->historicalDataTableView, ui->connectionWidget->currentDatabase());
    model->setEditStrategy(QSqlTableModel::OnRowChange);
    model->setTable(ui->connectionWidget->currentDatabase().driver()->escapeIdentifier(t, QSqlDriver::TableName));
    model->select();
    if (model->lastError().type() != QSqlError::NoError)
        emit statusMessage(model->lastError().text());
    ui->historicalDataTableView->setModel(model);
    ui->historicalDataTableView->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed);

    connect(ui->historicalDataTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(currentChanged()));
    updateActions();
}

void runtime_Window::showMetaData(const QString &t)
{
    QSqlRecord rec = ui->connectionWidget->currentDatabase().record(t);
    QStandardItemModel *model = new QStandardItemModel(ui->historicalDataTableView);

    model->insertRows(0, rec.count());
    model->insertColumns(0, 7);

    model->setHeaderData(0, Qt::Horizontal, "Fieldname");
    model->setHeaderData(1, Qt::Horizontal, "Type");
    model->setHeaderData(2, Qt::Horizontal, "Length");
    model->setHeaderData(3, Qt::Horizontal, "Precision");
    model->setHeaderData(4, Qt::Horizontal, "Required");
    model->setHeaderData(5, Qt::Horizontal, "AutoValue");
    model->setHeaderData(6, Qt::Horizontal, "DefaultValue");


    for (int i = 0; i < rec.count(); ++i) {
        QSqlField fld = rec.field(i);
        model->setData(model->index(i, 0), fld.name());
        model->setData(model->index(i, 1), fld.typeID() == -1
                ? QString(QMetaType::typeName(fld.type()))
                : QString("%1 (%2)").arg(QMetaType::typeName(fld.type())).arg(fld.typeID()));
        model->setData(model->index(i, 2), fld.length());
        model->setData(model->index(i, 3), fld.precision());
        model->setData(model->index(i, 4), fld.requiredStatus() == -1 ? QVariant("?")
                : QVariant(bool(fld.requiredStatus())));
        model->setData(model->index(i, 5), fld.isAutoValue());
        model->setData(model->index(i, 6), fld.defaultValue());
    }

    ui->historicalDataTableView->setModel(model);
    ui->historicalDataTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    updateActions();
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

            if(ui->hexDisplayChk->isChecked())
            {
                ui->messageBox->addItem("Read Coils (0x01)");
                ui->messageBox->addItem("Read Discrete Inputs (0x02)");
                ui->messageBox->addItem("Read Holding Registers (0x03)");
                ui->messageBox->addItem("Read Input Registers (0x04)");
                ui->messageBox->addItem("Write Single Coil (0x05)");
                ui->messageBox->addItem("Write Single Register(0x06)");
                ui->messageBox->addItem("Write Multiple Coils (0x0f)");
                ui->messageBox->addItem("Write Multiple Registers (0x10)");
            }
            else
            {
                ui->messageBox->addItem("Read Coils (1)");
                ui->messageBox->addItem("Read Discrete Inputs (2)");
                ui->messageBox->addItem("Read Holding Registers (3)");
                ui->messageBox->addItem("Read Input Registers (4)");
                ui->messageBox->addItem("Write Single Coil (5)");
                ui->messageBox->addItem("Write Single Register(6)");
                ui->messageBox->addItem("Write Multiple Coils (15)");
                ui->messageBox->addItem("Write Multiple Registers (16)");
            }

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

void runtime_Window::on_dataLogChkBox_stateChanged(int arg1)
{
    if(arg1 == 1)
    {
        ui->tabWidget->setTabEnabled(2,true);
    }
    else
    {
        ui->tabWidget->setTabEnabled(2,true);
    }
}

void runtime_Window::on_sendQueryButton_clicked()
{

}

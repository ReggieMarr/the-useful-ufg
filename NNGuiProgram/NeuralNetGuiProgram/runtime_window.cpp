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
#include "startupwindow.h"
#include "loadtrainingdata.h"
#include "fstream"

const int DataTypeColumn = 0;
const int AddrColumn = 1;
const int DataColumn = 2;

void runtime_Window::updateCombobox(int listIndex)
{
   uint boxSize =  ui->messageBox->count();
   ui->messageBox->addItems(MessageNames[listIndex]);

   for(int comboCount = boxSize -1;comboCount >= 0;comboCount--)
   {
       ui->messageBox->removeItem(comboCount);
   }
}

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
    dataLoggingSetup = false;


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
    {
        QMessageBox::information(this, tr("No database drivers found"),
                                 tr("This demo requires at least one Qt database driver. "
                                    "Please check the documentation how to build the "
                                    "Qt SQL plugins."));
    }

    //connect(dataLoggerWindow,dataLoggerWindow::closeEvent(QCloseEvent),runtime_Window,closeExternalWindow(dataLoggerWindow));


    MessageNames[0] << "Read Coils (1)" << "Read Discrete Inputs (2)" << "Read Holding Registers (3)"
                    << "Read Input Registers (4)" << "Write Single Coil (5)" << "Write Single Register(6)"
                    << "Write Multiple Coils (15)" << "Write Multiple Registers (16)";

    MessageNames[1] << "Read Coils (0x01)" << "Read Discrete Inputs (0x02)" << "Read Holding Registers (0x03)"
                    << "Read Input Registers (0x04)" << "Write Single Coil (0x05)" << "Write Single Register(0x06)"
                    << "Write Multiple Coils (0x0f)" << "Write Multiple Registers (0x10)";
    MessageNames[2] << "Discrete Coils" << "Discrete Input Contacts" << "Analog Input Registers" << "Analog Holding Registers";

    dataAddress = 0;
    slaveAddress = 0;

    ui->graphTableWidget->setRowCount(0);
    ui->graphTableWidget->setColumnCount(3);
    ui->graphTableWidget->setHorizontalHeaderLabels(QString("Var Name;X;Y").split(";"));

    ui->tagTableWidget->setRowCount(0);
    ui->tagTableWidget->setColumnCount(3);
    ui->tagTableWidget->setHorizontalHeaderLabels(QString("Tag Name;Input;Output").split(";"));

    ui->addTagButton->setEnabled(true);
    firstTagMade = false;
    tagSet = false;

    ui->confirmTagsBtn->setEnabled(false);

    ui->jsonTreeWidget->setVisible(false);

    deviceInputs.clear();

    deviceOutputs.clear();

    dbLoaded = false;

}

runtime_Window::~runtime_Window()
{
    delete ui;
}

void runtime_Window::updateDatabase(void)
{
    QString extension;

    extension = dbInformation.database.split(".",QString::SkipEmptyParts).at(1);

    ofstream fileToWrite (dbInformation.database.toStdString());

    if(fileToWrite.is_open())
    {
        if(extension == "txt")
        {
            fileToWrite << "topology: 2 4 1" << endl;
            for(uint entryVal = 0; entryVal < deviceInputs.size();entryVal++)
            {
                fileToWrite << "in: ";
                for(uint itemVal = 0;itemVal<deviceInputs.at(entryVal).size();itemVal++)
                {
                    fileToWrite << deviceInputs.at(entryVal).at(itemVal) << " ";
                }

                fileToWrite << endl << "out: ";

                for(uint itemVal = 0;itemVal<deviceOutputs.at(entryVal).size();itemVal++)
                {
                    fileToWrite << deviceOutputs.at(entryVal).at(itemVal) << " ";
                }
                fileToWrite << endl;
            }
            fileToWrite << endl;
        }
        fileToWrite.close();
    }
    else
    {
        cout << "Something went wrong \n";
    }

}

void runtime_Window::dbSourceConfigured(sourceInformation receivedSourceConfig)
{
    dbInformation = receivedSourceConfig;

    if(dbInformation.sourceType == 1 || dbInformation.sourceType == 0)
    {
       ui->sendQueryButton->setVisible(false);
       ui->clearButton->setVisible(false);
       ui->sqlEdit->setVisible(false);
       ui->dbLogConfigureBox->setTitle("Configure Database Update");

       QString extensionType = dbInformation.database.split(".",QString::SkipEmptyParts).at(1);

       vector<double> inputVals, currentTargetVals;

       if(extensionType == "txt")
       {
           const string dataFile = dbInformation.database.toStdString();

           TrainingData trainData(dataFile);

           //cout << "Got training data \n";
           // e.g., { 3, 2, 1 }
           vector<unsigned> topology;
           trainData.getTopology(topology);
           int trainingPass = 0;
           ui->historicalDataTableWidget->setColumnCount(3);
           ui->historicalDataTableWidget->setRowCount(1);

           while (!trainData.isEof()) {
               ++trainingPass;

               // Get new input data and feed it forward:
               if (trainData.getNextInputs(inputVals) != topology[0]) {
                   //cout << "\n you broke it, it went to this " << trainData.getNextInputs(inputVals) << endl;
                   break;
               }
               deviceInputs.push_back(inputVals);
               //trainData.getNextInputs(inputVals);
               QTableWidgetItem *inputCellValues = new QTableWidgetItem;

               inputCellValues->setText("in:");
               ui->historicalDataTableWidget->setRowCount(ui->historicalDataTableWidget->rowCount()+2);
               //ui->historicalDataTableWidget->insertRow(ui->historicalDataTableWidget->rowCount()+1);
               ui->historicalDataTableWidget->setItem(trainingPass,0,inputCellValues);
               for(uint i = 0;i<inputVals.size();i++)
               {
                   QTableWidgetItem *newInputCellValues = new QTableWidgetItem;
                   newInputCellValues->setText(QString::number(inputVals.at(i)));
                   ui->historicalDataTableWidget->setItem(trainingPass,i+1,newInputCellValues);
               }
               ++trainingPass;
               // Train the net what the outputs should have been:
               trainData.getTargetOutputs(currentTargetVals);

               deviceOutputs.push_back(currentTargetVals);

               //showVectorVals("Targets:", targetVals);
               assert(currentTargetVals.size() == topology.back());
               QTableWidgetItem *outputCellValues = new QTableWidgetItem;
               outputCellValues->setText("out:");
               //ui->historicalDataTableWidget->insertRow(ui->historicalDataTableWidget->rowCount());
               ui->historicalDataTableWidget->setItem(trainingPass,0,outputCellValues);
               for(uint i = 0;i<currentTargetVals.size();i++)
               {
                   QTableWidgetItem *newOutputCellValues = new QTableWidgetItem;
                   newOutputCellValues->setText(QString::number(currentTargetVals.at(i)));
                   ui->historicalDataTableWidget->setItem(trainingPass,i+1,newOutputCellValues);
               }
           }
           dbLoaded = true;
       }
       else if(extensionType == "csv")
       {

       }
       else if(extensionType == "json")
       {
           ui->jsonTreeWidget->setVisible(true);
       }
    }
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
        {
            //Write Multiple Registers (0x10)
            rowCount = ui->numberofCoilsSpin->value();
        }
        default:
        {
            ui->numberofCoilsSpin->setEnabled( true );
        }
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
            updateCombobox(1);
        }

        ui->interactionSelect->setCurrentIndex(connectionType);
        ui->tagsTabWidget->setCurrentIndex(0);
        break;
    case 1:
        if(!processTypeOpened)
        {

            ui->connectionSelectionBox->addItem("Offline");
            ui->connectionSelectionBox->addItem("Modbus");
            ui->connectionSelectionBox->setCurrentIndex(connectionType);

            if(ui->hexDisplayChk->isChecked())
            {
                updateCombobox(1);
                //ui->messageBox->addItems(MessageNames[1]);

            }
            else
            {
                updateCombobox(0);
                //ui->messageBox->addItems(MessageNames[0]);
            }
            ui->messageBox->setCurrentText(0);
            processTypeOpened = true;
        }

        break;
    case 2:
        if(!dataLoggingSetup)
        {
            //datasetup = new dataLogDialogWindow(this,dbType);
            datasetup  = new dataLogDialogWindow(this,dbType);
            connect(datasetup,SIGNAL(sendConfigInfo(sourceInformation)),this,SLOT(dbSourceConfigured(sourceInformation)));
            datasetup->show();
            //ui->tabWidget->setEnabled(false);
            dataLoggingSetup = true;
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
    if(arg1 == 2)
    {
        ui->tabWidget->setTabEnabled(2,true);
    }
    else
    {
        ui->tabWidget->setTabEnabled(2,false);
    }
}

void runtime_Window::on_sendQueryButton_clicked()
{

}

void runtime_Window::on_messageBox_currentIndexChanged(int index)
{
    if(ui->tagsTabWidget->currentIndex() == 0)
    {
        if(ui->hexDisplayChk->isChecked())
        {
            ui->tagNameEdit->setText("SA" + QString::number(slaveAddress) + "MT" + QString(ui->messageBox->currentIndex()) + "DA" + QString::number(dataAddress));
        }
        else
        {
            ui->tagNameEdit->setText("SA" + QString::number(slaveAddress) + "MT" + QString(ui->messageBox->currentIndex()) + "DA" + QString::number(dataAddress));
        }
    }
    else
    {

        ui->tagNameEdit->setText("SA" + QString::number(slaveAddress) + "MT" + QString(ui->messageBox->currentIndex()) + "DA" + QString::number(dataAddress));
    }

    ui->addTagButton->setEnabled(true);
}

void runtime_Window::on_startingAddressSelectBox_valueChanged(int arg1)
{
    dataAddress = arg1;
    if(ui->tagsTabWidget->currentIndex() == 0)
    {
        if(ui->hexDisplayChk->isChecked())
        {
            ui->tagNameEdit->setText("SA" + QString::number(slaveAddress) + "MT" + QString(ui->messageBox->currentIndex()) + "DA" + QString::number(dataAddress));
        }
        else
        {
            ui->tagNameEdit->setText("SA" + QString::number(slaveAddress) + "MT" + QString(ui->messageBox->currentIndex()) + "DA" + QString::number(dataAddress));
        }
    }
    else
    {

        ui->tagNameEdit->setText("SA" + QString::number(slaveAddress) + "MT" + QString(ui->messageBox->currentIndex()) + "DA" + QString::number(dataAddress));
    }
    ui->addTagButton->setEnabled(true);
}

void runtime_Window::on_IDValBox_valueChanged(int arg1)
{
    slaveAddress = arg1;
    if(ui->tagsTabWidget->currentIndex() == 0)
    {
        if(ui->hexDisplayChk->isChecked())
        {
            ui->tagNameEdit->setText("SA" + QString::number(slaveAddress) + "MT" + QString(ui->messageBox->currentIndex()) + "DA" + QString::number(dataAddress));
        }
        else
        {
            ui->tagNameEdit->setText("SA" + QString::number(slaveAddress) + "MT" + QString(ui->messageBox->currentIndex()) + "DA" + QString::number(dataAddress));
        }
    }
    else
    {

        ui->tagNameEdit->setText("SA" + QString::number(slaveAddress) + "MT" + QString(ui->messageBox->currentIndex()) + "DA" + QString::number(dataAddress));
    }
}

void runtime_Window::on_addTagButton_clicked()
{

    TagObj.tagDataAddress.push_back(dataAddress);
    TagObj.tagSlaveAddress.push_back(slaveAddress);
    TagObj.IOState.push_back(0);
    TagObj.tagName.push_back(QString(ui->tagNameEdit->text()).toStdString());
    TagObj.tagMessageType.push_back(ui->messageBox->currentIndex());

    ui->tagTableWidget->setRowCount(ui->tagTableWidget->rowCount()+1);
    int tagNum = TagObj.tagDataAddress.size()-1;

    QTableWidgetItem *tagNameItem = new QTableWidgetItem;

    QStringList nameList;
    for(uint t = TagObj.tagSlaveAddress.size()-1; t < TagObj.tagName.size();t++)
    {
        nameList += QString::fromStdString(TagObj.tagName.at(t));
    }

    tagNameItem->setText(QString::fromStdString(TagObj.tagName.at(tagNum)));
    ui->tagTableWidget->setItem(tagNum,0,tagNameItem);


    // Create an element, which will serve as a checkbox
    QTableWidgetItem *inputItem = new QTableWidgetItem;

    switch (ui->messageBox->currentIndex())
    {
        case 0:
        {
            //Discrete Output Coils (Read-Write)
            inputItem->setCheckState(Qt::Unchecked);
            break;
        }
        case 1:
        {
            //Discrete Input Contacts (Read Only)
            inputItem->setCheckState(Qt::Checked);
            break;
        }
        case 2:
        {
            //Analog Input Registers (Read Only)
            inputItem->setCheckState(Qt::Checked);

            break;
        }
        case 3:
        {
            //Analog Output Holding Registers (Read-Write)
            inputItem->setCheckState(Qt::Unchecked);
            break;
        }
    }
    inputItem->setFlags(inputItem->flags() | (Qt::ItemIsUserCheckable));
    ui->tagTableWidget->setItem(tagNum,1,inputItem);
    // Set the checkbox in the second column    
    QTableWidgetItem *outputItem = new QTableWidgetItem;

    switch (ui->messageBox->currentIndex())
    {
        case 0:
        {
            //Discrete Output Coils (Read-Write)
            outputItem->setCheckState(Qt::Checked);
            break;
        }
        case 1:
        {
            //Discrete Input Contacts (Read Only)
            outputItem->setCheckState(Qt::Unchecked);
            break;
        }
        case 2:
        {
            //Analog Input Registers (Read Only)
            outputItem->setCheckState(Qt::Unchecked);
            break;
        }
        case 3:
        {
            //Analog Output Holding Registers (Read-Write)
            outputItem->setCheckState(Qt::Checked);
            break;
        }
    }
    outputItem->setFlags(outputItem->flags() | (Qt::ItemIsUserCheckable));
    ui->tagTableWidget->setItem(tagNum,2,outputItem);

    ui->tagTableWidget->resizeColumnsToContents();
    ui->tagTableWidget->resizeRowsToContents();
    ui->addTagButton->setEnabled(false);

    if(firstTagMade)
    {
        tagSet = true;
    }

    firstTagMade = true;

    ui->confirmTagsBtn->setEnabled(true);
}

void runtime_Window::on_hexDisplayChk_stateChanged(int arg1)
{
    if(ui->tagsTabWidget->currentIndex() < 1)
    {
        if(arg1 == 2)
        {
            updateCombobox(1);
        }
        else
        {
            updateCombobox(0);
        }
    }

}

void runtime_Window::on_tagsTabWidget_currentChanged(int index)
{
    if(index > 0)
    {
        updateCombobox(2);
    }
    else
    {
        if(ui->hexDisplayChk->isChecked())
        {
            updateCombobox(1);
        }
        else
        {
            updateCombobox(0);
        }

    }
}

void runtime_Window::on_tagTableWidget_cellClicked(int row, int column)
{
    if(firstTagMade)
    {
        if(column == 1)
        {
            if(ui->tagTableWidget->item(row,column)->checkState() == Qt::Checked)
            {
                    ui->tagTableWidget->item(row,column+1)->setCheckState(Qt::Unchecked);
            }
            else if(ui->tagTableWidget->item(row,column+1)->checkState() == Qt::Unchecked || (
                    TagObj.tagMessageType.at(row) == 1 || TagObj.tagMessageType.at(row) == 2))
            {
                ui->tagTableWidget->item(row,column)->setCheckState(Qt::Checked);
            }
        }
        else if(column == 2)
        {
            if(ui->tagTableWidget->item(row,column)->checkState() == Qt::Checked)
            {
                if(TagObj.tagMessageType.at(row) == 1 || TagObj.tagMessageType.at(row) == 2)
                {
                    ui->tagTableWidget->item(row,column-1)->setCheckState(Qt::Checked);
                    ui->tagTableWidget->item(row,column)->setCheckState(Qt::Unchecked);
                }
                else
                {
                    ui->tagTableWidget->item(row,column-1)->setCheckState(Qt::Unchecked);
                }
            }
            else if(ui->tagTableWidget->item(row,column-1)->checkState() == Qt::Unchecked )
            {
                ui->tagTableWidget->item(row,column)->setCheckState(Qt::Checked);
            }
        }
        tagSet = false;
    }
}

void runtime_Window::on_confirmTagsBtn_clicked()
{
    QTableWidgetItem *tempWidget = new QTableWidgetItem;


    for(uint rowNum = 0;rowNum < TagObj.tagName.size();rowNum++)
    {
        TagObj.tagName.at(rowNum) = ui->tagTableWidget->item(rowNum,0)->text().toStdString();
        if(ui->tagTableWidget->item(rowNum,1)->checkState() == Qt::Checked)
        {
            TagObj.IOState.at(rowNum) = 1;
        }
        else if(ui->tagTableWidget->item(rowNum,1)->checkState() == Qt::Unchecked)
        {
            TagObj.IOState.at(rowNum) = -1;
        }
    }
}

void runtime_Window::on_mainMenuBtn_clicked()
{
    StartupWindow *mainMenu;
    this->close();
    mainMenu = new StartupWindow(this);
    mainMenu->show();
}

void runtime_Window::on_manualUpdateButton_clicked()
{
    updateDatabase();
}

void runtime_Window::on_historicalDataTableWidget_cellChanged(int row, int column)
{
    if(dbLoaded)
    {
        //deviceInputs/Outputs position of stored data must be offset due to
        //topology taking first row and in or out taking up the first column
        if(ui->historicalDataTableWidget->item(row,0)->text() == "in:")
        {
            if(row>2)
            {
                deviceInputs.at(row-2).at(column-1) = ui->historicalDataTableWidget->item(row,column)->text().toDouble();
            }
            else
            {
                deviceInputs.at(row-1).at(column-1) = ui->historicalDataTableWidget->item(row,column)->text().toDouble();
            }
        }
        else if(ui->historicalDataTableWidget->item(row,0)->text() == "out:")
        {
            if(row>2)
            {
                deviceOutputs.at(row).at(column-1) = ui->historicalDataTableWidget->item(row,column)->text().toDouble();
            }
            else
            {
                deviceOutputs.at(row).at(column-1) = ui->historicalDataTableWidget->item(row,column)->text().toDouble();
            }
            deviceOutputs.at(row).at(column-1) = ui->historicalDataTableWidget->item(row,column)->text().toDouble();
        }
    }
}

/*
 * Copyright © 2001-2011 Reginald Marr <reginald.t.marr@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 *
 */

#include "runtime_window.h"
#include "ui_runtime_window.h"
#include "iostream"
#include <QSqlDatabase>
#include "QSqlQuery"
#include "QSqlError"
#include "QSqlDriver"
#include "QSqlRecord"
#include "QSqlField"
//#include "qsqlconnectiondialog.h"
#include "startupwindow.h"
#include "loadtrainingdata.h"
#include "fstream"
#include "qextserialenumerator.h"
#include "qextserialport.h"
#include "executelogicsetupwindow.h"
#include "custommethodconstructorwindow.h"
#include "nntotagslinkagewindow.h"
#include "controlmethodmanagement.h"


const int DataTypeColumn = 0;
const int AddrColumn = 1;
const int DataColumn = 2;
std::atomic_bool run;

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
    ui->tabWidget->setTabEnabled(2,false);
    ui->tabWidget->setTabEnabled(3,false);
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

    QSettings s;

    int portIndex = 0;
    int i = 0;

    foreach( QextPortInfo port, QextSerialEnumerator::getPorts() )
    {
#ifdef Q_OS_WIN
        ui->serialComboBox->addItem( port.friendName );
#else
        ui->serialComboBox->addItem( port.physName );
#endif
        if( port.friendName == s.value( "serialinterface" ) )
        {
            portIndex = i;
        }
        ++i;
    }

    dbConvertOccurred = false;
    ui->saveToFileChkBox->setEnabled(false);

}

runtime_Window::~runtime_Window()
{
    if(controlThreads[0].joinable())
    {
        run = false;
        controlThreads[0].join();
    }

    delete ui;
}

void runtime_Window::updateDatabase(bool tableOnly,int dbType)
{

    switch (dbType) {
    case 0:
    {
        if(!tableOnly)
        {
        QStringList directoryList = dbInformation.database.split("/",QString::SkipEmptyParts);
        QString defaultName = dbInformation.database.split(".",QString::SkipEmptyParts).at(0) + ".txt";
        QString initialDirectory;

        for(uint dirCycle = 0;dirCycle < directoryList.size()-1;dirCycle++)
        {
            initialDirectory += directoryList.at(dirCycle) + "/";
        }

        QString savedir = QFileDialog::getExistingDirectory(
                    this, tr("Open Directory"),
                    initialDirectory,
                    QFileDialog::ShowDirsOnly
                    | QFileDialog::DontResolveSymlinks);

        QStringList nameBreakdown = defaultName.split("/",QString::SkipEmptyParts);

        defaultName = savedir + "/" + nameBreakdown.at(nameBreakdown.size()-1);

        dbInformation.database = defaultName;

        ofstream updatedFile;

        updatedFile.open(defaultName.toStdString());

        //write the topology here
        updatedFile << "topology: ";
        for(uint topologyCycle = 0;topologyCycle < savedTopology.size();topologyCycle++)
        {
            updatedFile << QString::number(savedTopology.at(topologyCycle)).toStdString() + " ";
        }
        updatedFile << endl;

        for(uint columnFill =0;columnFill<deviceInputs.size();columnFill++)
        {

            uint inputRowFill = 0;

            updatedFile << "in: ";
            for(inputRowFill = 0;inputRowFill<deviceInputs.at(columnFill).size();inputRowFill++)
            {
                //QTableWidgetItem *csvInputTableItems = new QTableWidgetItem;

                updatedFile << QString::number(deviceInputs.at(columnFill).at(inputRowFill)).toStdString() + " ";
            }
            uint outputRowFill;
            updatedFile << endl;

            updatedFile << "out: ";
            for(outputRowFill = inputRowFill;outputRowFill<deviceOutputs.at(columnFill).size()+inputRowFill;outputRowFill++)
            {
                //QTableWidgetItem *csvOutputTableItems = new QTableWidgetItem;

                updatedFile << QString::number(deviceOutputs.at(columnFill).at(outputRowFill-inputRowFill)).toStdString() + " ";
            }
            updatedFile << endl;
        }

        updatedFile.close();

        }
    }
        break;
    case 1:
        {
            if(!tableOnly)
            {
            QStringList directoryList = dbInformation.database.split("/",QString::SkipEmptyParts);
            QString defaultName = dbInformation.database.split(".",QString::SkipEmptyParts).at(0) + ".csv";
            QString initialDirectory;

            for(uint dirCycle = 0;dirCycle < directoryList.size()-1;dirCycle++)
            {
                initialDirectory += directoryList.at(dirCycle) + "/";
            }

            QString savedir = QFileDialog::getExistingDirectory(
                        this, tr("Open Directory"),
                        initialDirectory,
                        QFileDialog::ShowDirsOnly
                        | QFileDialog::DontResolveSymlinks);
            QStringList nameBreakdown = defaultName.split("/",QString::SkipEmptyParts);

            defaultName = savedir + "/" + nameBreakdown.at(nameBreakdown.size()-1);

            dbInformation.database = defaultName;
            //start writing the file with the default name here
            ofstream updatedFile;

            updatedFile.open(defaultName.toStdString());

            //write the topology here
            for(uint topologyCycle = 0;topologyCycle < savedTopology.size()+1;topologyCycle++)
            {
                if(topologyCycle == 0)
                {
                    updatedFile << "topology: ,";
                }
                else
                {
                    updatedFile << QString::number(savedTopology.at(topologyCycle-1)).toStdString() + ",";
                }
            }
            updatedFile << endl;

            //denote input and output status here
            uint inCycle;
            for(inCycle = 0;inCycle < deviceInputs.at(0).size();inCycle++)
            {
                updatedFile << "I,";
            }

            for(uint outCycle = inCycle;outCycle < deviceOutputs.at(0).size()+inCycle;outCycle++)
            {
                updatedFile << "O,";
            }
            updatedFile << endl;

            for(uint columnFill =0;columnFill<deviceInputs.size();columnFill++)
            {

                uint inputRowFill = 0;

                for(inputRowFill = 0;inputRowFill<deviceInputs.at(columnFill).size();inputRowFill++)
                {
                    QTableWidgetItem *csvInputTableItems = new QTableWidgetItem;

                    updatedFile << QString::number(deviceInputs.at(columnFill).at(inputRowFill)).toStdString() + ",";
                }
                uint outputRowFill;

                for(outputRowFill = inputRowFill;outputRowFill<deviceOutputs.at(columnFill).size()+inputRowFill;outputRowFill++)
                {
                    QTableWidgetItem *csvOutputTableItems = new QTableWidgetItem;

                    updatedFile << QString::number(deviceOutputs.at(columnFill).at(outputRowFill-inputRowFill)).toStdString() + ",";
                }
                updatedFile << endl;
            }

            updatedFile.close();

            }
        }
        break;
    case 2:
        {
            if(!tableOnly)
            {
                QStringList directoryList = dbInformation.database.split("/",QString::SkipEmptyParts);
                QString defaultName = dbInformation.database.split(".",QString::SkipEmptyParts).at(0) + ".json";
                QString initialDirectory;

                for(uint dirCycle = 0;dirCycle < directoryList.size()-1;dirCycle++)
                {
                    initialDirectory += directoryList.at(dirCycle) + "/";
                }

                QString savedir = QFileDialog::getExistingDirectory(
                            this, tr("Open Directory"),
                            initialDirectory,
                            QFileDialog::ShowDirsOnly
                            | QFileDialog::DontResolveSymlinks);

                dbInformation.database = defaultName;
            }
        }
        break;
    default:
        break;
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

           ui->convertComboBox->addItems(QString("txt to csv;txt to MySQL table;txt to JSON").split(";"));
           TrainingData trainData(dataFile);

           //cout << "Got training data \n";
           // e.g., { 3, 2, 1 }

           savedTopology.clear();
           trainData.getTopology(savedTopology);
           int trainingPass = 0;
           ui->historicalDataTableWidget->setColumnCount(3);
           ui->historicalDataTableWidget->setRowCount(1);
           bool firstRun = true;

           while (!trainData.isEof())
           {
               //++trainingPass;

               if(firstRun)
               {
                   QTableWidgetItem *firstRowItem = new QTableWidgetItem;
                   firstRowItem->setText("Topology:");
                   ui->historicalDataTableWidget->setRowCount(ui->historicalDataTableWidget->rowCount()+2);
                   ui->historicalDataTableWidget->setItem(trainingPass,0,firstRowItem);
                   ui->historicalDataTableWidget->setColumnCount(savedTopology.size()+1);
                   for(uint i = 0;i<savedTopology.size();i++)
                   {
                       QTableWidgetItem *fillFirstRowItem = new QTableWidgetItem;
                       fillFirstRowItem->setText(QString::number(savedTopology.at(i)));
                       ui->historicalDataTableWidget->setItem(trainingPass,i+1,fillFirstRowItem);
                   }

                   trainingPass++;
                   firstRun = false;
               }
               else
               {
                   trainingPass++;
               }

               // Get new input data and feed it forward:
               if (trainData.getNextInputs(inputVals) != savedTopology[0])
               {
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
               assert(currentTargetVals.size() == savedTopology.back());
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
            ui->convertComboBox->addItems(QString("csv to txt;csv to MySQL table;csv to JSON").split(";"));

            const string dataFile = dbInformation.database.toStdString();

            ifstream dbFile(dataFile);

            dbFile.exceptions(std::ifstream::failbit|std::ifstream::badbit);
            string checkStr;
            unsigned inputSize = 0;
            unsigned outputSize = 0;
            savedTopology.clear();
            string::size_type sz;
            int trainingPass = 0;
            QStringList headerList;

            //read the topology then figure out the Inputs and outputs
            getline(dbFile,checkStr,'\n');
            stringstream ssTopLine(checkStr);
            while(getline(ssTopLine,checkStr,','))
            {
                if(checkStr=="topology: " )
                {
                    // i guess this would be the topology found
                }
                else
                {
                    savedTopology.push_back(stoi(checkStr,&sz));
                }
            }

            QTableWidgetItem *firstRowItem = new QTableWidgetItem;
            firstRowItem->setText("topology:");
            ui->historicalDataTableWidget->setColumnCount(savedTopology.size() + 1);
            ui->historicalDataTableWidget->setRowCount(ui->historicalDataTableWidget->rowCount()+2);
            ui->historicalDataTableWidget->setItem(trainingPass,0,firstRowItem);


            for(uint i = 0;i<savedTopology.size();i++)
            {
                QTableWidgetItem *fillFirstRowItem = new QTableWidgetItem;
                fillFirstRowItem->setText(QString::number(savedTopology.at(i)));
                ui->historicalDataTableWidget->setItem(trainingPass,i+1,fillFirstRowItem);
            }
            trainingPass++;

            //figure out how many inputs and output exist per line
            getline(dbFile,checkStr,'\n');
            stringstream ssIOLine(checkStr);
            while(getline(ssIOLine,checkStr,','))
            {
                QTableWidgetItem *ioCellValues = new QTableWidgetItem;
                //checkStr.erase(remove_if(checkStr.begin(), checkStr.end(), isspace), checkStr.end());
                if(checkStr=="I")
                {
                    ioCellValues->setText("I");
                    headerList.push_back("Input");
                    ui->historicalDataTableWidget->setItem(trainingPass,inputSize,ioCellValues);
                    inputSize++;
                }
                else if(checkStr=="O")
                {
                    ioCellValues->setText("O");
                    headerList.push_back("Output");
                    if(inputSize + outputSize + 1 > savedTopology.size() + 2)
                    {
                        ui->historicalDataTableWidget->setColumnCount(inputSize + outputSize + 1);
                    }
                    ui->historicalDataTableWidget->setItem(trainingPass,inputSize+outputSize,ioCellValues);
                    outputSize++;
                }
            }

            ui->historicalDataTableWidget->setHorizontalHeaderLabels(headerList);

            //figure out how many inputs and output exist per line
            vector<double> lineVals;
            deviceInputs.clear();
            deviceOutputs.clear();

            try
            {
                int cycleIn;


                while(getline(dbFile,checkStr))
                {

                    trainingPass++;
                    ui->historicalDataTableWidget->setRowCount(trainingPass+1);
                    stringstream ssFillLine(checkStr);
                    lineVals.clear();

                    for(cycleIn = 0;cycleIn < inputSize;cycleIn++)
                    {
                        getline(ssFillLine, checkStr,',');
                        lineVals.push_back(stod(checkStr,&sz));
                        QTableWidgetItem *inputFillValues = new QTableWidgetItem;
                        QString fillString = QString::number(stod(checkStr,&sz));
                        inputFillValues->setText(fillString);
                        ui->historicalDataTableWidget->setItem(trainingPass,cycleIn,inputFillValues);
                    }
                    deviceInputs.push_back(lineVals);
                    lineVals.clear();

                    for(int cycleOut = cycleIn;cycleOut < outputSize + cycleIn;cycleOut++)
                    {
                        getline(ssFillLine, checkStr,',');
                        lineVals.push_back(stod(checkStr,&sz));
                        QTableWidgetItem *outputFillValues = new QTableWidgetItem;
                        QString fillString = QString::number(stod(checkStr,&sz));
                        outputFillValues->setText(fillString);
                        ui->historicalDataTableWidget->setItem(trainingPass,cycleOut,outputFillValues);
                    }
                    deviceOutputs.push_back(lineVals);
                }
            }
            catch(std::ifstream::failure e)
            {
                std::cerr << "Exception happened: " << e.what() << "\n"
                  << "Error bits are: "
                  << "\nfailbit: " << dbFile.fail()
                  << "\neofbit: " << dbFile.eof()
                  << "\nbadbit: " << dbFile.bad() << std::endl;

                cout << "Finished File \n";
                dbFile.close();
            }


            dbLoaded = true;

       }
       else if(extensionType == "json")
       {
           ui->convertComboBox->addItems(QString("JSON to txt;JSON to csv;JSON to MySQL Object").split(";"));
           ui->jsonTreeWidget->setVisible(true);
       }
    }
}

void runtime_Window::on_convertDbButton_clicked()
{
    //note that the number of inputs and outputs must remain constant, even if a null identifier is issued
    //otherwise things will get a tad fucked
    dbConvertOccurred = true;
    dbLoaded = false;

    ui->saveToFileChkBox->setEnabled(true);
    ui->saveToFileChkBox->setChecked(false);

    if(dbInformation.sourceType == 0 || dbInformation.sourceType == 1)
    {
        if(dbInformation.database.split(".",QString::SkipEmptyParts).at(1) == "txt")
        {
            switch (ui->convertComboBox->currentIndex())
            {
            case 0:
                {
                    ui->historicalDataTableWidget->clearContents();
                    ui->historicalDataTableWidget->setColumnCount(3);
                    ui->historicalDataTableWidget->setRowCount(deviceInputs.size());
                    ui->historicalDataTableWidget->setHorizontalHeaderLabels(QString("Input 1;Input 2;Output").split(";"));
                    uint columnFill =0;
                    int largestColumn = 0;
                    uint inCycle = 0;

                    if(savedTopology.size() + 2 > deviceInputs.size() + deviceOutputs.size())
                    {
                        ui->historicalDataTableWidget->setColumnCount(savedTopology.size() + 2);
                    }
                    else
                    {
                        ui->historicalDataTableWidget->setColumnCount(deviceInputs.size() + deviceOutputs.size());
                    }


                    //we up by one to account for denoting topology
                    for(uint topologyCycle = 0;topologyCycle < savedTopology.size()+1;topologyCycle++)
                    {
                        QTableWidgetItem *csvTopDefineLine = new QTableWidgetItem;
                        if(topologyCycle == 0)
                        {
                            csvTopDefineLine->setText("topology:");
                            ui->historicalDataTableWidget->setItem(columnFill,topologyCycle,csvTopDefineLine);
                        }
                        else
                        {
                            csvTopDefineLine->setText(QString::number(savedTopology.at(topologyCycle-1)));
                            ui->historicalDataTableWidget->setItem(columnFill,topologyCycle,csvTopDefineLine);
                        }
                    }

                    columnFill++;
                    for(inCycle = 0;inCycle < deviceInputs.at(0).size();inCycle++)
                    {
                        QTableWidgetItem *csvInDefineLine = new QTableWidgetItem;

                        csvInDefineLine->setText("I");
                        ui->historicalDataTableWidget->setItem(columnFill,inCycle,csvInDefineLine);
                    }

                    for(uint outCycle = inCycle;outCycle < deviceOutputs.at(0).size()+inCycle;outCycle++)
                    {
                        QTableWidgetItem *csvOutDefineLine = new QTableWidgetItem;

                        csvOutDefineLine->setText("O");
                        ui->historicalDataTableWidget->setItem(columnFill,outCycle,csvOutDefineLine);
                    }
                    columnFill++;

                    for(columnFill;columnFill<deviceInputs.size()+2;columnFill++)
                    {
                        if(largestColumn < deviceInputs.at(columnFill-2).size()+ deviceOutputs.at(columnFill-2).size())
                        {
                            largestColumn = deviceInputs.at(columnFill-2).size()+ deviceOutputs.at(columnFill-2).size();
                        }

                        ui->historicalDataTableWidget->setColumnCount(largestColumn);

                        uint inputRowFill = 0;

                        for(inputRowFill = 0;inputRowFill<deviceInputs.at(columnFill-2).size();inputRowFill++)
                        {
                            QTableWidgetItem *csvInputTableItems = new QTableWidgetItem;

                            csvInputTableItems->setText(QString::number(deviceInputs.at(columnFill-2).at(inputRowFill)));
                            ui->historicalDataTableWidget->setItem(columnFill,inputRowFill,csvInputTableItems);
                        }
                        uint outputRowFill;

                        for(outputRowFill = inputRowFill;outputRowFill<deviceOutputs.at(columnFill-2).size()+inputRowFill;outputRowFill++)
                        {
                            QTableWidgetItem *csvOutputTableItems = new QTableWidgetItem;

                            csvOutputTableItems->setText(QString::number(deviceOutputs.at(columnFill-2).at(outputRowFill-inputRowFill)));
                            ui->historicalDataTableWidget->setItem(columnFill,outputRowFill,csvOutputTableItems);
                        }
                    }
                }
                break;
            case 1:
                {
                    ui->historicalDataTableWidget->clear();
                }
                break;
            case 2:
                {

                }
                break;
            }
        }
        else if(dbInformation.database.split(".",QString::SkipEmptyParts).at(1) == "csv")
        {
            switch (ui->convertComboBox->currentIndex())
            {
            case 0:
                {
                    ui->historicalDataTableWidget->clearContents();
                    ui->historicalDataTableWidget->setColumnCount(deviceInputs.at(0).size() + deviceOutputs.at(0).size());
                    ui->historicalDataTableWidget->setRowCount(deviceInputs.size());

                    //this only accounts for consistent numbers of inputs and outputs
                    QStringList headerList;
                    for(uint i = 0;i<deviceInputs.at(0).size();i++)
                    {
                        //headerList += "Input " + QString::number(i);
                        headerList.push_back("Input " + QString::number(i));
                    }

                    for(uint i = 0;i<deviceOutputs.at(0).size();i++)
                    {
                        //headerList += "Output " + QString::number(i);
                        headerList.push_back("Output " + QString::number(i));
                    }

                    uint trainingPass = 0;

                    ui->historicalDataTableWidget->setHorizontalHeaderLabels(headerList);

                    QTableWidgetItem *firstRowItem = new QTableWidgetItem;
                    firstRowItem->setText("topology:");
                    ui->historicalDataTableWidget->setColumnCount(savedTopology.size() + 1);
                    ui->historicalDataTableWidget->setRowCount(ui->historicalDataTableWidget->rowCount()+2);
                    ui->historicalDataTableWidget->setItem(trainingPass,0,firstRowItem);


                    for(uint i = 0;i<savedTopology.size();i++)
                    {
                        QTableWidgetItem *fillFirstRowItem = new QTableWidgetItem;
                        fillFirstRowItem->setText(QString::number(savedTopology.at(i)));
                        ui->historicalDataTableWidget->setItem(trainingPass,i+1,fillFirstRowItem);
                    }
                    trainingPass++;

                    int ioPullPass = 0;

                    ui->historicalDataTableWidget->setColumnCount(savedTopology.size() + 1);

                    while(ioPullPass < deviceInputs.size())
                    {
                        ui->historicalDataTableWidget->setRowCount(trainingPass+2);

                        QTableWidgetItem *csvInputLabelItem = new QTableWidgetItem;
                        csvInputLabelItem->setText("in:");
                        ui->historicalDataTableWidget->setItem(trainingPass,0,csvInputLabelItem);
                        for(uint inputRowFill = 1;inputRowFill<deviceInputs.at(ioPullPass).size()+1;inputRowFill++)
                        {
                            QTableWidgetItem *csvInputTableItems = new QTableWidgetItem;
                            csvInputTableItems->setText(QString::number(deviceInputs.at(ioPullPass).at(inputRowFill-1)));
                            ui->historicalDataTableWidget->setItem(trainingPass,inputRowFill,csvInputTableItems);
                        }

                        trainingPass++;
                        QTableWidgetItem *csvOutputLabelItem = new QTableWidgetItem;
                        csvOutputLabelItem->setText("out:");
                        ui->historicalDataTableWidget->setItem(trainingPass,0,csvOutputLabelItem);
                        for(uint outputRowFill = 1;outputRowFill<deviceOutputs.at(ioPullPass).size()+1;outputRowFill++)
                        {
                            QTableWidgetItem *csvOutputTableItems = new QTableWidgetItem;
                            csvOutputTableItems->setText(QString::number(deviceInputs.at(ioPullPass).at(outputRowFill-1)));
                            ui->historicalDataTableWidget->setItem(trainingPass,outputRowFill,csvOutputTableItems);
                        }
                        trainingPass++;
                        ioPullPass++;

                    }

                }
                break;
            case 1:
                {
                    ui->historicalDataTableWidget->clear();
                }
                break;
            case 2:
                {

                }
                break;
            }
        }
        else if(dbInformation.database.split(".",QString::SkipEmptyParts).at(1) == "json")
        {

        }
    }

    dbLoaded = true;
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
            ui->messageBox->setCurrentIndex(0);

            processTypeOpened = true;
        }

        break;
    case 3:
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
        ui->tabWidget->setTabEnabled(2,true);
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
    methodSetup tester2 = *fillableSetup;
    if(arg1 == 2)
    {
        ui->tabWidget->setTabEnabled(3,true);
    }
    else
    {
        ui->tabWidget->setTabEnabled(3,false);
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
    updateDatabase(true,0);
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

void runtime_Window::on_autoUpdateConfigButton_clicked()
{
//    executeLogicSetupWindow *newLogicSetup = new executeLogicSetupWindow;
//    newLogicSetup->show();
    methodSetup testSetup;
    customMethodConstructorWindow *newMethodSetup = new customMethodConstructorWindow(this);
    newMethodSetup->show();
}

void runtime_Window::on_saveToFileChkBox_stateChanged(int arg1)
{
    if(arg1 == 2)
    {

        if(dbInformation.sourceType == 0 || dbInformation.sourceType == 1)
        {
            if(dbInformation.database.split(".",QString::SkipEmptyParts).at(1) == "txt")
            {
                switch (ui->convertComboBox->currentIndex())
                {
                case 0:
                    {
                    QStringList directoryList = dbInformation.database.split("/",QString::SkipEmptyParts);
                    QString defaultName = dbInformation.database.split(".",QString::SkipEmptyParts).at(0) + ".csv";
                    QString initialDirectory;

                    for(uint dirCycle = 0;dirCycle < directoryList.size()-1;dirCycle++)
                    {
                        initialDirectory += directoryList.at(dirCycle) + "/";
                    }

                    QString savedir = QFileDialog::getExistingDirectory(
                                this, tr("Open Directory"),
                                initialDirectory,
                                QFileDialog::ShowDirsOnly
                                | QFileDialog::DontResolveSymlinks);
                    QStringList nameBreakdown = defaultName.split("/",QString::SkipEmptyParts);

                    defaultName = savedir + "/" + nameBreakdown.at(nameBreakdown.size()-1);

                    dbInformation.database = defaultName;
                    //start writing the file with the default name here
                    ofstream updatedFile;

                    updatedFile.open(defaultName.toStdString());

                    //write the topology here
                    for(uint topologyCycle = 0;topologyCycle < savedTopology.size()+1;topologyCycle++)
                    {
                        if(topologyCycle == 0)
                        {
                            updatedFile << "topology: ,";
                        }
                        else
                        {
                            updatedFile << QString::number(savedTopology.at(topologyCycle-1)).toStdString() + ",";
                        }
                    }
                    updatedFile << endl;

                    //denote input and output status here
                    uint inCycle;
                    for(inCycle = 0;inCycle < deviceInputs.at(0).size();inCycle++)
                    {
                        updatedFile << "I,";
                    }

                    for(uint outCycle = inCycle;outCycle < deviceOutputs.at(0).size()+inCycle;outCycle++)
                    {
                        updatedFile << "O,";
                    }
                    updatedFile << endl;

                    for(uint columnFill =0;columnFill<deviceInputs.size();columnFill++)
                    {

                        uint inputRowFill = 0;

                        for(inputRowFill = 0;inputRowFill<deviceInputs.at(columnFill).size();inputRowFill++)
                        {
                            QTableWidgetItem *csvInputTableItems = new QTableWidgetItem;

                            updatedFile << QString::number(deviceInputs.at(columnFill).at(inputRowFill)).toStdString() + ",";
                        }
                        uint outputRowFill;

                        for(outputRowFill = inputRowFill;outputRowFill<deviceOutputs.at(columnFill).size()+inputRowFill;outputRowFill++)
                        {
                            QTableWidgetItem *csvOutputTableItems = new QTableWidgetItem;

                            updatedFile << QString::number(deviceOutputs.at(columnFill).at(outputRowFill-inputRowFill)).toStdString() + ",";
                        }
                        updatedFile << endl;
                    }

                    updatedFile.close();

                    }
                    break;
                case 1:
                    {
                    QStringList directoryList = dbInformation.database.split("/",QString::SkipEmptyParts);
                    QString defaultName = dbInformation.database.split(".",QString::SkipEmptyParts).at(0) + ".csv";
                    QString initialDirectory;

                    for(uint dirCycle = 0;dirCycle < directoryList.size()-1;dirCycle++)
                    {
                        initialDirectory += directoryList.at(dirCycle) + "/";
                    }

                    QString savedir = QFileDialog::getExistingDirectory(
                                this, tr("Open Directory"),
                                initialDirectory,
                                QFileDialog::ShowDirsOnly
                                | QFileDialog::DontResolveSymlinks);

                    dbInformation.database = defaultName;
                    }
                    break;
                case 2:
                {
                    QStringList directoryList = dbInformation.database.split("/",QString::SkipEmptyParts);
                    QString defaultName = dbInformation.database.split(".",QString::SkipEmptyParts).at(0) + ".json";
                    QString initialDirectory;

                    for(uint dirCycle = 0;dirCycle < directoryList.size()-1;dirCycle++)
                    {
                        initialDirectory += directoryList.at(dirCycle) + "/";
                    }

                    QString savedir = QFileDialog::getExistingDirectory(
                                this, tr("Open Directory"),
                                initialDirectory,
                                QFileDialog::ShowDirsOnly
                                | QFileDialog::DontResolveSymlinks);

                    dbInformation.database = defaultName;
                }
                    break;
                }
            }
            else if(dbInformation.database.split(".",QString::SkipEmptyParts).at(1) == "csv")
            {
                switch (ui->convertComboBox->currentIndex())
                {
                case 0:
                    {
                    QStringList directoryList = dbInformation.database.split("/",QString::SkipEmptyParts);
                    QString defaultName = dbInformation.database.split(".",QString::SkipEmptyParts).at(0) + ".txt";
                    QString initialDirectory;

                    for(uint dirCycle = 0;dirCycle < directoryList.size()-1;dirCycle++)
                    {
                        initialDirectory += directoryList.at(dirCycle) + "/";
                    }

                    QString savedir = QFileDialog::getExistingDirectory(
                                this, tr("Open Directory"),
                                initialDirectory,
                                QFileDialog::ShowDirsOnly
                                | QFileDialog::DontResolveSymlinks);

                    QStringList nameBreakdown = defaultName.split("/",QString::SkipEmptyParts);

                    defaultName = savedir + "/" + nameBreakdown.at(nameBreakdown.size()-1);

                    dbInformation.database = defaultName;

                    ofstream updatedFile;

                    updatedFile.open(defaultName.toStdString());

                    //write the topology here
                    updatedFile << "topology: ";
                    for(uint topologyCycle = 0;topologyCycle < savedTopology.size();topologyCycle++)
                    {
                        updatedFile << QString::number(savedTopology.at(topologyCycle)).toStdString() + " ";
                    }
                    updatedFile << endl;

                    for(uint columnFill =0;columnFill<deviceInputs.size();columnFill++)
                    {

                        uint inputRowFill = 0;

                        updatedFile << "in: ";
                        for(inputRowFill = 0;inputRowFill<deviceInputs.at(columnFill).size();inputRowFill++)
                        {
                            QTableWidgetItem *csvInputTableItems = new QTableWidgetItem;

                            updatedFile << QString::number(deviceInputs.at(columnFill).at(inputRowFill)).toStdString() + " ";
                        }
                        uint outputRowFill;
                        updatedFile << endl;

                        updatedFile << "out: ";
                        for(outputRowFill = inputRowFill;outputRowFill<deviceOutputs.at(columnFill).size()+inputRowFill;outputRowFill++)
                        {
                            QTableWidgetItem *csvOutputTableItems = new QTableWidgetItem;

                            updatedFile << QString::number(deviceOutputs.at(columnFill).at(outputRowFill-inputRowFill)).toStdString() + " ";
                        }
                        updatedFile << endl;
                    }

                    updatedFile.close();

                    }
                    break;
                case 1:
                    {
                    QStringList directoryList = dbInformation.database.split("/",QString::SkipEmptyParts);
                    QString defaultName = dbInformation.database.split(".",QString::SkipEmptyParts).at(0) + ".csv";
                    QString initialDirectory;

                    for(uint dirCycle = 0;dirCycle < directoryList.size()-1;dirCycle++)
                    {
                        initialDirectory += directoryList.at(dirCycle) + "/";
                    }

                    QString savedir = QFileDialog::getExistingDirectory(
                                this, tr("Open Directory"),
                                initialDirectory,
                                QFileDialog::ShowDirsOnly
                                | QFileDialog::DontResolveSymlinks);

                    dbInformation.database = defaultName;
                    }
                    break;
                case 2:
                {
                    QStringList directoryList = dbInformation.database.split("/",QString::SkipEmptyParts);
                    QString defaultName = dbInformation.database.split(".",QString::SkipEmptyParts).at(0) + ".json";
                    QString initialDirectory;

                    for(uint dirCycle = 0;dirCycle < directoryList.size()-1;dirCycle++)
                    {
                        initialDirectory += directoryList.at(dirCycle) + "/";
                    }

                    QString savedir = QFileDialog::getExistingDirectory(
                                this, tr("Open Directory"),
                                initialDirectory,
                                QFileDialog::ShowDirsOnly
                                | QFileDialog::DontResolveSymlinks);

                    dbInformation.database = defaultName;
                }
                    break;
                }
            }
            else if(dbInformation.database.split(".",QString::SkipEmptyParts).at(1) == "json")
            {

            }
        }

    }
}

void runtime_Window::on_dynamicObjectSetupBtn_clicked()
{
    customMethodConstructorWindow *newMethodSetup = new customMethodConstructorWindow(this);
    connect(newMethodSetup,SIGNAL(updatedMethodObject(methodSetup)),
            this,SLOT(on_updatedMethodObject(methodSetup)));
    newMethodSetup->show();

}


void runtime_Window::on_linkNNBtn_clicked()
{
    NNtoTagsLinkageWindow *newLinkWindow = new NNtoTagsLinkageWindow(this);
    newLinkWindow->show();
}

void loopMethods(methodSetup currentSetup)
{
    //this and the this_thread::sleep_for is used to reduce CPU % usage
    //at 200 there is no recognizable load on the CPU, likely possible to reduce this greatly
    using namespace std::literals::chrono_literals;

    controlMethodManagement controlObj;
    while(run)
    {
        controlObj.executeCustomMethods(currentSetup);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void runtime_Window::on_engageDynamicMethodsChk_toggled(bool checked)
{
    //this allows us to call the loop function which itself calls the control methods in a while loop
    //we use the atomic boolean to start and stop the function in the thread and then the join() to allow the thread
    //to run to completion
    if(checked && fillableSetup)
    {
        run = true;
        controlThreads[0] = std::thread(loopMethods,*fillableSetup);
    }
    else if(!checked)
    {
        run = false;
        controlThreads[0].join();
    }
}

void runtime_Window::on_updatedMethodObject(methodSetup newSetup)
{
    *fillableSetup = newSetup;
}

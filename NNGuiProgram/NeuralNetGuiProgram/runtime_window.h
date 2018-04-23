/*
 * Copyright © 2001-2011 Reginald Marr <reginald.t.marr@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 *
 */
#ifndef RUNTIME_WINDOW_H
#define RUNTIME_WINDOW_H

#include <QMainWindow>
#include "executeneuralnet.h"
//#include <QSqlTableModel>
#include "QListWidgetItem"
#include "datalogdialogwindow.h"

namespace Ui {
class runtime_Window;
}

struct Tag {
    vector<string> tagName;
    vector<string> tagIPAddress;
    vector<int> tagMessageType;
    vector<int> IOState; //initialized to 0 output is -1 input is 1
    vector<int> tagDataAddress;
    vector<int> tagSlaveAddress;
};

struct methodSetup
{
    methodSetup() {}

    std::vector<std::vector<double>> userSelection;
    std::vector<std::vector<int>> selectionType; //this may not be needed as it could be implied from layout type
    std::vector<int> layoutType;
};

class runtime_Window : public QMainWindow
{
    Q_OBJECT

public:
    //explicit runtime_Window(QWidget *parent = 0,Browser * parentModel);
    explicit runtime_Window(QWidget *parent = 0);
    ~runtime_Window();

    void insertRow();
    void deleteRow();
    void updateActions();

    sourceInformation dbInformation;
public slots:
    void dbSourceConfigured(sourceInformation receivedSourceConfig);

private slots:
    void on_controlSelectComboBox_currentIndexChanged(int index);

    void on_IPEdit0_textChanged(const QString &arg1);

    void on_IPEdit1_textChanged(const QString &arg1);

    void on_IPEdit2_textChanged(const QString &arg1);

    void on_IPEdit3_textChanged(const QString &arg1);

    void on_portEdit_textChanged(const QString &arg1);

    void on_connectBtn_clicked();

    void on_sendDataBtn_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_connectionSelectionBox_currentIndexChanged(int index);

    void on_interactionSelect_currentIndexChanged(int index);

    void on_dataLogChkBox_stateChanged(int arg1);

    void on_sendQueryButton_clicked();

    void on_messageBox_currentIndexChanged(int index);

    void on_startingAddressSelectBox_valueChanged(int arg1);

    void on_IDValBox_valueChanged(int arg1);

    void on_addTagButton_clicked();

    void on_hexDisplayChk_stateChanged(int arg1);

    void on_tagsTabWidget_currentChanged(int index);

    void on_tagTableWidget_cellClicked(int row, int column);

    void on_confirmTagsBtn_clicked();

    void on_mainMenuBtn_clicked();

    void on_manualUpdateButton_clicked();

    void on_historicalDataTableWidget_cellChanged(int row, int column);

    void on_autoUpdateConfigButton_clicked();

    void on_convertDbButton_clicked();

    void on_saveToFileChkBox_stateChanged(int arg1);

    void on_dynamicObjectSetupBtn_clicked();
    
    void on_linkNNBtn_clicked();

    void on_engageDynamicMethodsChk_toggled(bool checked);

    void on_updatedMethodObject(methodSetup newSetup);

private:
    //Browser * m_model;
    void releaseTcpModbus();
    void updateRegisterView( void );
    void updateCombobox(int listIndex);
    void updateDatabase(bool tableOnly, int dbType);

    modbus_t * m_tcpModbus = NULL;
    Ui::runtime_Window *ui;
    QString IPAddress[4];
    int portNum;
    int connectionType;
    int manualMessageType;
    bool processTypeOpened,dataLoggingSetup,dbLoaded;
    int dataAddress;
    int slaveAddress;
    Tag TagObj;
    QStringList MessageNames[3];
    bool firstTagMade,tagSet;
    QListWidget *tagNames = new QListWidget;
    QListWidget *inputCheckBoxes = new QListWidget;
    QListWidget *outputCheckBoxes = new QListWidget;
    dataLogDialogWindow *datasetup;//  = new dataLogDialogWindow(this,dbType);
    vector<vector<double>> deviceInputs;
    vector<vector<double>> deviceOutputs;
    vector<unsigned> savedTopology;
    int dbType;
    bool dbConvertOccurred;
    methodSetup *fillableSetup = new methodSetup;
    methodSetup otherTestSetup;
};
#endif // RUNTIME_WINDOW_H

#ifndef RUNTIME_WINDOW_H
#define RUNTIME_WINDOW_H

#include <QMainWindow>
#include "executeneuralnet.h"
//#include <QSqlTableModel>

#include "dataloggerwindow.h"

namespace Ui {
class runtime_Window;
}

struct Tag {
    vector<string> tagName;
    vector<int> IOState; //initialized to 0 output is -1 input is 1
    vector<int> tagDataAddress;
    vector<int> tagSlaveAddress;
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

    void closeExternalWindow(dataLoggerWindow * targetClose);

    void on_IOSetupBtn_clicked();

    void on_messageBox_currentIndexChanged(int index);

    void on_startingAddressSelectBox_valueChanged(int arg1);

    void on_IDValBox_valueChanged(int arg1);

    void on_addTagButton_clicked();

private:
    //Browser * m_model;
    void releaseTcpModbus();
    void updateRegisterView( void );
    modbus_t * m_tcpModbus = NULL;
    dataLoggerWindow * setupWindow;
    Ui::runtime_Window *ui;
    QString IPAddress[4];
    int portNum;
    int connectionType;
    int manualMessageType;
    bool processTypeOpened;
    bool dataLoggingSetup;
    int dataAddress;
    int slaveAddress;
    Tag TagObj;
    QStringList MessageNames[2];
};
#endif // RUNTIME_WINDOW_H

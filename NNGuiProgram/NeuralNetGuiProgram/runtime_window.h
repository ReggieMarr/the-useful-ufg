#ifndef RUNTIME_WINDOW_H
#define RUNTIME_WINDOW_H

#include <QMainWindow>
#include "executeneuralnet.h"

namespace Ui {
class runtime_Window;
}

class runtime_Window : public QMainWindow
{
    Q_OBJECT

public:
    explicit runtime_Window(QWidget *parent = 0);
    ~runtime_Window();

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

private:
    void releaseTcpModbus();
    void updateRegisterView( void );
    modbus_t * m_tcpModbus = NULL;

    Ui::runtime_Window *ui;
    QString IPAddress[4];
    int portNum;
    int connectionType;
    int manualMessageType;
    bool processTypeOpened;
};

#endif // RUNTIME_WINDOW_H

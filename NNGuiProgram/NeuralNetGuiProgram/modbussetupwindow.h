#ifndef MODBUSSETUPWINDOW_H
#define MODBUSSETUPWINDOW_H

#include <QMainWindow>

typedef struct modbusConfig
{
    char *serialPortOrIPAddress;
    int baudOrTCPPort;
    int dataBits;
    float stopBits;
    char *parity;
} modbusConfig;

namespace Ui {
class modbusSetupWindow;
}

class modbusSetupWindow : public QMainWindow
{
    Q_OBJECT

public:

    modbusConfig userConfig = {"/dev/ttyS0",9600,8,1.5,"Odd"};



    explicit modbusSetupWindow(QWidget *parent = 0);
    explicit modbusSetupWindow(QWidget *parent,modbusConfig &modbusConnConfig);
    ~modbusSetupWindow();

private slots:
    void on_setupConnectionBtn_clicked();

private:
    Ui::modbusSetupWindow *ui;
    int portIndex;
};

#endif // MODBUSSETUPWINDOW_H

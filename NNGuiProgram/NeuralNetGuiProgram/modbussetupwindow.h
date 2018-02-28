/*
 * Copyright © 2001-2011 Reginald Marr <reginald.t.marr@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 *
 * This library implements the Modbus protocol.
 * http://libmodbus.org/
 */
#ifndef MODBUSSETUPWINDOW_H
#define MODBUSSETUPWINDOW_H

#include <QMainWindow>

typedef struct modbusConfig
{
    //char *serialPortOrIPAddress;
    QString serialPortOrIPAddress;
    int baudOrTCPPort;
    int dataBits;
    float stopBits;
    //char *parity;
    QString parity;
} modbusConfig;

namespace Ui {
class modbusSetupWindow;
}

class modbusSetupWindow : public QMainWindow
{
    Q_OBJECT

public:

    modbusConfig *userConfig;// = new(modbusConfig);// = {"/dev/ttyS0",9600,8,1.5,"Odd"};

    explicit modbusSetupWindow(QWidget *parent = 0);
    explicit modbusSetupWindow(QWidget *parent,modbusConfig *modbusConnConfig,int connType);
    ~modbusSetupWindow();

    //these come from the libmodbus gui
    void settext(const QString &t);
    QString text() const;

signals:
    void textChanged(const QString& newText);

private slots:
    void on_setupConnectionBtn_clicked();

    void on_editIPClassifier_textChanged(const QString &arg1);

    void on_editIPAdd2_textChanged(const QString &arg1);

    void on_editIPAdd3_textChanged(const QString &arg1);

    void on_editIPAdd4_textChanged(const QString &arg1);

    void on_editPort_textChanged(const QString &arg1);

private:
    Ui::modbusSetupWindow *ui;
    int portIndex;
    int modbusEthernetMode;
    QString inputIP;
    QString inputPort;
};

#endif // MODBUSSETUPWINDOW_H

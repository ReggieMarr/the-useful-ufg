/*
 * Copyright © 2001-2011 Reginald Marr <reginald.t.marr@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 *
 */
#ifndef MYSQLSETUPWINDOW_H
#define MYSQLSETUPWINDOW_H

#include <QMainWindow>
#include "mysqlapi.h"

namespace Ui {
class mySQlSetupWindow;
}

struct setupConfiguration
{
char *serverIP; //192.168.2.156
char *user; //root
char *password; //6AS3Project
char *database; //stillas3_project
};

class mySQlSetupWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit mySQlSetupWindow(QWidget *parent = 0);
    explicit mySQlSetupWindow(QWidget *parent = 0, setupConfiguration validatedSetup = {});
    ~mySQlSetupWindow();

private slots:
    void on_pushButton_clicked();

    void on_firstIPVal_valueChanged(const QString &arg1);

    void on_secondIPVal_valueChanged(const QString &arg1);

    void on_threeIPVal_valueChanged(const QString &arg1);

    void on_fourthIPVal_valueChanged(const QString &arg1);

private:
    char *testServerIP;
    char *testUser;
    char *testPassword;
    char *testDatabase;
    QString inputServerIP[4];
    Ui::mySQlSetupWindow *ui;
    mySQLAPI sqlAPIObj;
    char ***SQLout;
    bool sqlCheck;
    setupConfiguration validatedSetup;
};

#endif // MYSQLSETUPWINDOW_H

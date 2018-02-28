/*
 * Copyright © 2001-2011 Reginald Marr <reginald.t.marr@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 *
 */
#ifndef MYSQLCUSTOMQUERYWINDOW_H
#define MYSQLCUSTOMQUERYWINDOW_H

#include <QMainWindow>
#include "mysqlapi.h"

namespace Ui {
class mySQLCustomQueryWindow;
}

class mySQLCustomQueryWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit mySQLCustomQueryWindow(QWidget *parent = 0);

    mySQLCustomQueryWindow(bool addInputMode);
    bool queryIsValid = false;

    bool returnValidateCheck()
    {
        return queryIsValid;
    }

    ~mySQLCustomQueryWindow();

private slots:
    void on_pushButton_clicked();

private:
    bool inputMode;
    Ui::mySQLCustomQueryWindow *ui;
    mySQLAPI sqlAPIObj;
};

#endif // MYSQLCUSTOMQUERYWINDOW_H

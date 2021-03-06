/*
 * Copyright © 2001-2011 Reginald Marr <reginald.t.marr@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 *
 */
#include "mysqlcustomquerywindow.h"
#include "ui_mysqlcustomquerywindow.h"

mySQLCustomQueryWindow::mySQLCustomQueryWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mySQLCustomQueryWindow)
{
    ui->setupUi(this);
}

mySQLCustomQueryWindow::mySQLCustomQueryWindow(bool addInputMode)
{
    inputMode = addInputMode;
}

mySQLCustomQueryWindow::~mySQLCustomQueryWindow()
{
    delete ui;
}

void mySQLCustomQueryWindow::on_pushButton_clicked()
{
    //sqlAPIObj.sql_main("Query",ui->databaseTextEntry->);
}

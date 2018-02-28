/*
 * Copyright © 2001-2011 Reginald Marr <reginald.t.marr@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 *
 */
#include "mysqlsetupwindow.h"
#include "ui_mysqlsetupwindow.h"
#include "QMessageBox"
#include "nnconfigwindow.h"

mySQlSetupWindow::mySQlSetupWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mySQlSetupWindow)
{
    ui->setupUi(this);
}

mySQlSetupWindow::mySQlSetupWindow(QWidget *parent,setupConfiguration validatedSetup) :
    QMainWindow(parent),
    ui(new Ui::mySQlSetupWindow)
{
    ui->setupUi(this);
}

mySQlSetupWindow::~mySQlSetupWindow()
{
    NNConfigWindow configWindowObj;

    if(sqlCheck)
    {
        configWindowObj.mySQLIsValid = true;

        configWindowObj.mySQLConfig = validatedSetup;
    }
    else
    {
        configWindowObj.mySQLIsValid = false;
    }

    delete ui;
}

void mySQlSetupWindow::on_pushButton_clicked()
{
    QByteArray tempPassword = (ui->passwordTextEntry->toPlainText()).toLocal8Bit();
    testPassword = tempPassword.data();

    QByteArray tempUsername = (ui->usernameTextEntry->toPlainText()).toLocal8Bit();
    testUser = tempUsername.data();

    QByteArray tempDatabase = (ui->databaseTextEntry->toPlainText()).toLocal8Bit();
    testDatabase = tempDatabase.data();


    QByteArray tempServerIP  = (inputServerIP[0] + "." + inputServerIP[1] + "." + inputServerIP[2] + "." + inputServerIP[3]).toLocal8Bit();
    testServerIP = tempServerIP.data();


    char sqlcmd[350] ={'\0'};
    //Sizing the SQLout based on the number of rows in Stations_and_Devices
    SQLout = (char ***)malloc(sizeof(char **));
    SQLout[0] = (char **)malloc(sizeof(char *));
    SQLout[0][0] = (char *)malloc(sizeof(char ));
    memset(&sqlcmd[0], 0, sizeof(sqlcmd));
    strcpy(sqlcmd,"SELECT COUNT(*) FROM Devices_References");
    //sql_main(sqlcmd,SQLout,"Stations_and_Devices",true,sqlcolsize);

    int sqlrowsize = atoi(SQLout[0][0]);

    memset(&sqlcmd[0], 0, sizeof(sqlcmd));
    strcpy(sqlcmd,"SELECT Count(*) FROM INFORMATION_SCHEMA.COLUMNS WHERE table_name = 'Devices_References'");
    //sql_main(sqlcmd,SQLout,"Stations_and_Devices",true,sqlcolsize);

    int sqlcolsize = atoi(SQLout[0][0]);

    //Setting the size of string in each SQLoutput followed by the number of rows and then columns
//    SQLout = (char ***)calloc(250, sizeof(char **));
//    for (int i=0;i <sqlrowsize; i++)
//    {
//        SQLout[i] = (char **)calloc(sqlrowsize, sizeof(char*));
//        for (int j =0;j<sqlcolsize;j++)
//        {
//            SQLout[i][j] = (char *)calloc(250,sizeof(char));
//        }
//    }

    //bool mySQLAPI::sql_main(char *sqlcmd,char ***SQLout, char *database,bool returnmode,
     //                       int columnsize,char *server,char *user,char *password)

    sqlCheck = sqlAPIObj.sql_main("Select * FROM stilldata",SQLout,testDatabase,false,8,testServerIP,testUser,testPassword);


    free(SQLout[0][0]);
    free(SQLout[0]);
    free(SQLout);


    if(sqlCheck)
    {
        //ui->pushButton->setText("mySQLSetup OK");
        validatedSetup.database = testDatabase;
        validatedSetup.password = testPassword;
        validatedSetup.user = testUser;
        validatedSetup.serverIP = testServerIP;

        QMessageBox::information(
            this,
            tr("Industrial Neural Network"),
            tr("mySQL Setup OK!") );
    }
    else
    {
        QMessageBox::information(
            this,
            tr("Industrial Neural Network"),
            tr("mySQL is BAD :(!") );
    }
}

void mySQlSetupWindow::on_firstIPVal_valueChanged(const QString &arg1)
{
    inputServerIP[0] = arg1;
}

void mySQlSetupWindow::on_secondIPVal_valueChanged(const QString &arg1)
{
    inputServerIP[1] = arg1;
}

void mySQlSetupWindow::on_threeIPVal_valueChanged(const QString &arg1)
{
    inputServerIP[2] = arg1;
}

void mySQlSetupWindow::on_fourthIPVal_valueChanged(const QString &arg1)
{
    inputServerIP[3] = arg1;
}

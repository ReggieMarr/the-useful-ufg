/*
 * Copyright © 2001-2011 Reginald Marr <reginald.t.marr@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 *
 */
#include "startupwindow.h"
#include "ui_startupwindow.h"
#include "custommethodconstructorwindow.h"

StartupWindow::StartupWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StartupWindow)
{
    ui->setupUi(this);
    trainingMode = true;
}

StartupWindow::~StartupWindow()
{
    delete ui;
}

void StartupWindow::on_trainingMode_clicked(bool checked)
{
    trainingMode = checked;
}

void StartupWindow::on_runMode_clicked(bool checked)
{
    trainingMode = !checked;
}

void StartupWindow::on_selectButton_clicked()
{

    this->close();
    if(trainingMode)
    {
//        customMethodConstructorWindow *newMethodSetup = new customMethodConstructorWindow(this);
//        newMethodSetup->show();
        NeuralNetWindow = new NNConfigWindow(this,trainingMode);
        NeuralNetWindow->show();
    }
    else
    {
        onlineWindow = new runtime_Window(this);
        onlineWindow->show();
    }


}

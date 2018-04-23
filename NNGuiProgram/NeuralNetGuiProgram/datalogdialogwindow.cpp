/*
 * Copyright © 2001-2011 Reginald Marr <reginald.t.marr@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 *
 */
#include "datalogdialogwindow.h"
#include "ui_datalogdialogwindow.h"
#include "QFileDialog"

dataLogDialogWindow::dataLogDialogWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::dataLogDialogWindow)
{
    ui->setupUi(this);

    ui->sourceTypeCombo->addItem("Existing Local File");
    ui->sourceTypeCombo->addItem("New Local File");
    ui->sourceTypeCombo->addItem("MySQL Database");
}

dataLogDialogWindow::dataLogDialogWindow(QWidget *parent, int MessageType) :

    QMainWindow(parent),
    ui(new Ui::dataLogDialogWindow)
{
    ui->setupUi(this);

    ui->sourceTypeCombo->addItem("Existing Local File");
    ui->sourceTypeCombo->addItem("New Local File");
    ui->sourceTypeCombo->addItem("MySQL Database");
}

dataLogDialogWindow::~dataLogDialogWindow()
{
    delete ui;
}

void dataLogDialogWindow::on_historicalSetupBtn_clicked()
{
    switch (ui->sourceTypeCombo->currentIndex()) {
    case 0:
    {
        filename = QFileDialog::getOpenFileName(
                    this,
                    tr("Select Data File"),
                    "/home",
                    "Text Files (*.txt);CSV File (*.csv);JSON File (*.json);All Files (*.*)"
                    );

        addedSource.database = filename;
        addedSource.sourceType = ui->sourceTypeCombo->currentIndex();

        emit sendConfigInfo(addedSource);

        this->close();

    }
        break;
    case 1:
    {
        savedir = QFileDialog::getExistingDirectory(
                    this, tr("Open Directory"),
                    "/home",
                    QFileDialog::ShowDirsOnly
                    | QFileDialog::DontResolveSymlinks);
        addedSource.sourceType =  ui->sourceTypeCombo->currentIndex();

        addedSource.database = savedir + addedSource.database;

        emit sendConfigInfo(addedSource);

        this->close();
    }
        break;
    case 2:
    {

    }
        break;
    }
}

void dataLogDialogWindow::on_sourceTypeCombo_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
        ui->editDatabase->setVisible(false);
        ui->editHostname->setVisible(false);
        ui->editPassword->setVisible(false);
        ui->portSpinBox->setVisible(false);
        ui->textLabel2->setVisible(false);
        ui->textLabel3->setVisible(false);
        ui->textLabel4->setVisible(false);
        ui->textLabel4_2->setVisible(false);
        ui->textLabel5->setVisible(false);
        ui->textLabel5_2->setVisible(false);
        ui->editUsername->setVisible(false);
        break;
    case 1:
        ui->editHostname->setVisible(false);
        ui->editPassword->setVisible(false);
        ui->portSpinBox->setVisible(false);
        ui->textLabel2->setVisible(false);
        ui->textLabel4->setVisible(false);
        ui->textLabel4_2->setVisible(false);
        ui->textLabel5->setVisible(false);
        ui->textLabel5_2->setVisible(false);
        ui->editUsername->setVisible(false);
        ui->textLabel3->setVisible(true);
        ui->textLabel3->setText("Filename");
        ui->editDatabase->setVisible(true);
        break;
    case 2:
        ui->editDatabase->setVisible(true);
        ui->editHostname->setVisible(true);
        ui->editPassword->setVisible(true);
        ui->portSpinBox->setVisible(true);
        ui->textLabel2->setVisible(true);
        ui->textLabel3->setVisible(true);
        ui->textLabel4->setVisible(true);
        ui->textLabel4_2->setVisible(true);
        ui->textLabel5->setVisible(true);
        ui->textLabel5_2->setVisible(true);
        ui->editUsername->setVisible(true);
        break;
    }
}

void dataLogDialogWindow::on_editDatabase_editingFinished()
{
    addedSource.database = (ui->editDatabase->text().toLocal8Bit());//tempDatabase.data();
}

void dataLogDialogWindow::on_editUsername_editingFinished()
{
    addedSource.user = (ui->editUsername->text().toLocal8Bit());//tempUsername.data();
}

void dataLogDialogWindow::on_editPassword_editingFinished()
{
    addedSource.password = (ui->editPassword->text().toLocal8Bit());//tempPassword.data();
}

void dataLogDialogWindow::on_editHostname_editingFinished()
{
    addedSource.serverIP = (ui->editHostname->text().toLocal8Bit());//tempHostname.data();
}

void dataLogDialogWindow::on_portSpinBox_valueChanged(int arg1)
{
    addedSource.port = arg1;
}

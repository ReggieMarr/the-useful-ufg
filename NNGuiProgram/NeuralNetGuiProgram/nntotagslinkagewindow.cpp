/*
 * Copyright © 2001-2011 Reginald Marr <reginald.t.marr@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */
#include "nntotagslinkagewindow.h"
#include "ui_nntotagslinkagewindow.h"

NNtoTagsLinkageWindow::NNtoTagsLinkageWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NNtoTagsLinkageWindow)
{
    ui->setupUi(this);
}

NNtoTagsLinkageWindow::~NNtoTagsLinkageWindow()
{
    delete ui;
}

void NNtoTagsLinkageWindow::on_confirmLinkageBtn_clicked()
{
    
}

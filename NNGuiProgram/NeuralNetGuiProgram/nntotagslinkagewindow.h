/*
 * Copyright © 2001-2011 Reginald Marr <reginald.t.marr@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */
#ifndef NNTOTAGSLINKAGEWINDOW_H
#define NNTOTAGSLINKAGEWINDOW_H

#include <QDialog>

namespace Ui {
class NNtoTagsLinkageWindow;
}

class NNtoTagsLinkageWindow : public QDialog
{
    Q_OBJECT

public:
    explicit NNtoTagsLinkageWindow(QWidget *parent = 0);
    ~NNtoTagsLinkageWindow();

private slots:
    void on_confirmLinkageBtn_clicked();
    
private:
    Ui::NNtoTagsLinkageWindow *ui;
};

#endif // NNTOTAGSLINKAGEWINDOW_H

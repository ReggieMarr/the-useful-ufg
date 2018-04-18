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

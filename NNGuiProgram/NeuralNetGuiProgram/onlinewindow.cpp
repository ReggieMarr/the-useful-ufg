#include "onlinewindow.h"
#include "ui_onlinewindow.h"

OnlineWindow::OnlineWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::OnlineWindow)
{
    ui->setupUi(this);
}

OnlineWindow::~OnlineWindow()
{
    delete ui;
}

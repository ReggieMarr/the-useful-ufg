#include "startupwindow.h"
#include "ui_startupwindow.h"

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
        NeuralNetWindow = new NNConfigWindow(this,trainingMode);
        NeuralNetWindow->show();
    }
    else
    {
        onlineWindow = new runtime_Window(this);
        onlineWindow->show();
    }


}

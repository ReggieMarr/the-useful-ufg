#ifndef STARTUPWINDOW_H
#define STARTUPWINDOW_H

#include <QMainWindow>
#include "nnconfigwindow.h"

namespace Ui {
class StartupWindow;
}

class StartupWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit StartupWindow(QWidget *parent = 0);
    ~StartupWindow();

private slots:
    void on_trainingMode_clicked(bool checked);

    void on_runMode_clicked(bool checked);

    void on_selectButton_clicked();

private:
    Ui::StartupWindow *ui;
    bool trainingMode;
    NNConfigWindow* NeuralNetWindow;
};

#endif // STARTUPWINDOW_H

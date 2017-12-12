#ifndef MODBUSSETUPWINDOW_H
#define MODBUSSETUPWINDOW_H

#include <QMainWindow>

namespace Ui {
class modbusSetupWindow;
}

class modbusSetupWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit modbusSetupWindow(QWidget *parent = 0);
    ~modbusSetupWindow();

private slots:
    void on_setupConnectionBtn_clicked();

private:
    Ui::modbusSetupWindow *ui;
    int portIndex;
};

#endif // MODBUSSETUPWINDOW_H

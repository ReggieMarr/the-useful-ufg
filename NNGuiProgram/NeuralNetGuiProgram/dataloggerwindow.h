#ifndef DATALOGGERWINDOW_H
#define DATALOGGERWINDOW_H

#include <QWidget>

namespace Ui {
class dataLoggerWindow;
}

class dataLoggerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit dataLoggerWindow(QWidget *parent = 0);
    ~dataLoggerWindow();

private slots:
    void on_historicalSetupBtn_clicked();

    void on_sourceTypeCombo_currentIndexChanged(int index);


private:
    Ui::dataLoggerWindow *ui;
};

#endif // DATALOGGERWINDOW_H

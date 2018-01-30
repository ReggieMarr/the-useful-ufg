#ifndef DATALOGDIALOGWINDOW_H
#define DATALOGDIALOGWINDOW_H

#include <QMainWindow>

namespace Ui {
class dataLogDialogWindow;
}

struct sourceInformation
{
//char *
QString serverIP; //192.168.2.156
QString user; //root
QString password; //6AS3Project
QString database; //stillas3_project
int port;
int sourceType;
};

class dataLogDialogWindow : public QMainWindow
{
    Q_OBJECT

public:


    explicit dataLogDialogWindow(QWidget *parent = 0);
    explicit dataLogDialogWindow(QWidget *parent = 0, int MessageType = -1);
    ~dataLogDialogWindow();

    sourceInformation addedSource;

signals:
    void sendConfigInfo(sourceInformation InfotoSend);

private slots:
    void on_historicalSetupBtn_clicked();

    void on_sourceTypeCombo_currentIndexChanged(int index);

    void on_editDatabase_editingFinished();

    void on_editUsername_editingFinished();

    void on_editPassword_editingFinished();

    void on_editHostname_editingFinished();

    void on_portSpinBox_valueChanged(int arg1);

private:
    Ui::dataLogDialogWindow *ui;
    QString savedir;
    QString filename;
};

#endif // DATALOGDIALOGWINDOW_H

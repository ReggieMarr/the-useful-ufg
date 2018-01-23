#ifndef RUNTIME_WINDOW_H
#define RUNTIME_WINDOW_H

#include <QMainWindow>
#include "executeneuralnet.h"
#include <QSqlTableModel>
namespace Ui {
class runtime_Window;
}


class runtime_Window : public QMainWindow
{
    Q_OBJECT

public:
    //explicit runtime_Window(QWidget *parent = 0,Browser * parentModel);
    explicit runtime_Window(QWidget *parent = 0);
    ~runtime_Window();
    QSqlError addConnection(const QString &driver, const QString &dbName, const QString &host,
                  const QString &user, const QString &passwd, int port = -1);

    void insertRow();
    void deleteRow();
    void updateActions();

private slots:
    void on_controlSelectComboBox_currentIndexChanged(int index);

    void on_IPEdit0_textChanged(const QString &arg1);

    void on_IPEdit1_textChanged(const QString &arg1);

    void on_IPEdit2_textChanged(const QString &arg1);

    void on_IPEdit3_textChanged(const QString &arg1);

    void on_portEdit_textChanged(const QString &arg1);

    void on_connectBtn_clicked();

    void on_sendDataBtn_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_connectionSelectionBox_currentIndexChanged(int index);

    void on_interactionSelect_currentIndexChanged(int index);

    void on_dataLogChkBox_stateChanged(int arg1);

    void on_sendQueryButton_clicked();

    void exec();
    void showTable(const QString &table);
    void showMetaData(const QString &table);
    void addConnection();
    void currentChanged() { updateActions(); }
    void about();

    void on_insertRowAction_triggered()
    { insertRow(); }
    void on_deleteRowAction_triggered()
    { deleteRow(); }
    void on_fieldStrategyAction_triggered();
    void on_rowStrategyAction_triggered();
    void on_manualStrategyAction_triggered();
    void on_submitAction_triggered();
    void on_revertAction_triggered();
    void on_selectAction_triggered();
    void on_connectionWidget_tableActivated(const QString &table)
    { showTable(table); }
    void on_connectionWidget_metaDataRequested(const QString &table)
    { showMetaData(table); }
signals:
    void statusMessage(const QString &message);
private:
    //Browser * m_model;
    void releaseTcpModbus();
    void updateRegisterView( void );
    modbus_t * m_tcpModbus = NULL;

    Ui::runtime_Window *ui;
    QString IPAddress[4];
    int portNum;
    int connectionType;
    int manualMessageType;
    bool processTypeOpened;
};

class CustomModel: public QSqlTableModel
{
    Q_OBJECT
public:
    explicit CustomModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase()):QSqlTableModel(parent, db) {}
    QVariant data(const QModelIndex &idx, int role) const override
    {
        if (role == Qt::BackgroundRole && isDirty(idx))
            return QBrush(QColor(Qt::yellow));
        return QSqlTableModel::data(idx, role);
    }
};

#endif // RUNTIME_WINDOW_H

#ifndef EXECUTELOGICSETUPWINDOW_H
#define EXECUTELOGICSETUPWINDOW_H

#include <QMainWindow>
#include "controlobjectitemdelegate.h"

namespace Ui {
class executeLogicSetupWindow;
}

struct customObject
{
    customObject() {}

    std::vector<int> inputType,outputType,comparisonType;
    std::vector<double> valueA,valueB;
    //std::vector<bool> comboElseIfMode,comboWhileMode,comboAndMode;

};

class executeLogicSetupWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit executeLogicSetupWindow(QWidget *parent = 0);
    ~executeLogicSetupWindow();
    void translateLogic(int inputType, int comparisonType, double valueA, double valueB);
    customObject customObjectAttributes;

private slots:
    void on_newRowButton_clicked();

    void on_newColumnButton_clicked();

    void on_confirmLogicButton_clicked();

    void OnComboIndexChanged(const QString& text);

    void OnTreeItemCBChanged(QStandardItem* testItem);

private:
    bool initialSetup;
    unsigned methodIndex;
    void addComboRow(int rowIndex);
    Ui::executeLogicSetupWindow *ui;
    QStandardItemModel *treeModel;
    controlObjectItemDelegate *treeItemDelegate;

};

#endif // EXECUTELOGICSETUPWINDOW_H
#ifndef EXECUTELOGICSETUPWINDOW_H
#define EXECUTELOGICSETUPWINDOW_H

#include <QMainWindow>

namespace Ui {
class executeLogicSetupWindow;
}

struct customLogicFlow
{
    customLogicFlow() {}

    std::vector<int> inputType;
    std::vector<int> comparisonType;
    std::vector<double> valueA;
    std::vector<double> valueB;
    std::vector<int> outputType;
};

class executeLogicSetupWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit executeLogicSetupWindow(QWidget *parent = 0);
    ~executeLogicSetupWindow();
    void translateLogic(int inputType, int comparisonType, double valueA, double valueB);
    customLogicFlow setupLogic;

private slots:
    void on_newRowButton_clicked();

    void on_newColumnButton_clicked();

    void on_confirmLogicButton_clicked();

    void OnComboIndexChanged(const QString& text);

private:
    Ui::executeLogicSetupWindow *ui;
};

#endif // EXECUTELOGICSETUPWINDOW_H

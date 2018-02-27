#ifndef CUSTOMMETHODCONSTRUCTORWINDOW_H
#define CUSTOMMETHODCONSTRUCTORWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QTreeWidget>
#include "controlobjectitemdelegate.h"

namespace Ui {
class customMethodConstructorWindow;
}

struct testcustomObject
{
    testcustomObject() {}

    std::vector<int> inputType,outputType,comparisonType;
    std::vector<double> valueA,valueB;
    //std::vector<bool> comboElseIfMode,comboWhileMode,comboAndMode;

};

class customMethodConstructorWindow : public QMainWindow
{
    Q_OBJECT

    void AddRoot(QStringList rootNames);
    void AddChild(QTreeWidgetItem *parent, QStringList rootNames);

public:
    QPoint testPoint;
    testcustomObject customObjectAttributes;
    explicit customMethodConstructorWindow(QWidget *parent = 0);
    ~customMethodConstructorWindow();

private slots:
    void OnComboIndexChanged(const QString& text);
    void OnTimeBoxChanged(const QString& text);

    void on_addMethodBtn_clicked();

private:
    controlObjectItemDelegate *treeItemDelegate;
    int childColumnOffset;
    bool methodHasIfChild;
    Ui::customMethodConstructorWindow *ui;
    std::vector<std::vector<QStringList>> methodBlocks;
    void addChildRow(QTreeWidget *widgetParent, QTreeWidgetItem *itemParent, int cycleStart, int layoutType);
    void addChildRow(QTreeWidget *widgetParent, QTreeWidgetItem *itemParent, int rowType);
};

#endif // CUSTOMMETHODCONSTRUCTORWINDOW_H

/*
 * Copyright © 2001-2011 Reginald Marr <reginald.t.marr@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 *
 */
#ifndef CUSTOMMETHODCONSTRUCTORWINDOW_H
#define CUSTOMMETHODCONSTRUCTORWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QTreeWidget>
//#include "controlobjectitemdelegate.h"
#include "runtime_window.h"

namespace Ui {
class customMethodConstructorWindow;
}

class customMethodConstructorWindow : public QMainWindow
{
    Q_OBJECT

    void AddRoot(QString rootName, int ifIndex);
    void AddChild(QTreeWidgetItem *parent, QStringList rootNames);

public:
    //std::vector<methodSetup> customObjectMethods;
    methodSetup customObjectMethods;
    //testcustomObject customObjectAttributes;
    explicit customMethodConstructorWindow(QWidget *parent = 0);
    ~customMethodConstructorWindow();

private slots:
    void OnLogicSetupComboIndexChanged(const QString& text);
    void OnExecuteLogicComboIndexChanged(const QString& text);
    void on_addMethodBtn_clicked();

    void on_confirmBtn_clicked();

signals:
    void updatedMethodObject(methodSetup newSetup);

private:
    std::vector<QString> treeRootNames;
    //controlObjectItemDelegate *treeItemDelegate;
    int childColumnOffset;
    bool methodHasIfChild;
    int methodIndex,loggedIfIndex;
    Ui::customMethodConstructorWindow *ui;
    std::vector<std::vector<QStringList>> methodBlocks;
    void addChildRow(QTreeWidget *widgetParent, QTreeWidgetItem *itemParent, bool fillSelections, int layoutType, int rootIndex, int ifIndex);
    void addChildRow(QTreeWidget *widgetParent, QTreeWidgetItem *itemParent);
    void updateLayout(unsigned currentLayout, unsigned selectedColumn, QString selectedText,
                      QTreeWidget *widgetParent, QTreeWidgetItem *parentItem, bool fillInputs, unsigned rootIndex, int methodCycle, int ifIndex);
    int updateMethodModel(int cycleCount, QTreeWidgetItem *parentItem);

};

#endif // CUSTOMMETHODCONSTRUCTORWINDOW_H

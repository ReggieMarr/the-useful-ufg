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
#include "controlobjectitemdelegate.h"

namespace Ui {
class customMethodConstructorWindow;
}

struct methodSetup
{
    methodSetup() {}

    std::vector<std::vector<double>> userSelection;
    std::vector<std::vector<int>> selectionType;


};

class customMethodConstructorWindow : public QMainWindow
{
    Q_OBJECT

    void AddRoot(QString rootName);
    void AddChild(QTreeWidgetItem *parent, QStringList rootNames);

public:
    //std::vector<methodSetup> customObjectMethods;
    methodSetup customObjectMethods;
    //testcustomObject customObjectAttributes;
    explicit customMethodConstructorWindow(QWidget *parent = 0);
    ~customMethodConstructorWindow();

private slots:
    void OnComboIndexChanged(const QString& text);
    void OnTimeBoxChanged(const QString& text);

    void on_addMethodBtn_clicked();

private:
    std::vector<QString> treeRootNames;
    controlObjectItemDelegate *treeItemDelegate;
    int childColumnOffset;
    bool methodHasIfChild;
    Ui::customMethodConstructorWindow *ui;
    std::vector<std::vector<QStringList>> methodBlocks;
    void addChildRow(QTreeWidget *widgetParent, QTreeWidgetItem *itemParent, bool fillSelections, int layoutType, int rootIndex);
    void addChildRow(QTreeWidget *widgetParent, QTreeWidgetItem *itemParent, int rowType);
};

#endif // CUSTOMMETHODCONSTRUCTORWINDOW_H

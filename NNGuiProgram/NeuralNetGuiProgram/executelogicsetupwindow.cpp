/*
 * Copyright © 2001-2011 Reginald Marr <reginald.t.marr@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 *
 */
#include "executelogicsetupwindow.h"
#include "ui_executelogicsetupwindow.h"
#include "QComboBox"
#include "QTreeView"
#include "QDebug"

#include <QFile>
//#include "treemodel.h"

void executeLogicSetupWindow::addComboRow(int rowIndex)
{
    int colIndex = 0;
    //int rowIndex = ui->logicSetupTableWidget->rowCount() -1;
    //its not actually neccessary to name each of these seperately

    //we want to ensure that each attribute has been setup

//    if(rowIndex > 0)
//    {
//        customObjectAttributes.inputType.at(rowIndex - 1);
//        customObjectAttributes.comparisonType.at(rowIndex - 1);
//    }

    QComboBox *inputComboBox = new QComboBox;
    inputComboBox->setProperty("row", rowIndex);
    inputComboBox->setProperty("column", colIndex);
    inputComboBox->addItems(QString("If;While").split(";"));
    connect(inputComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
    ui->logicSetupTableWidget->setCellWidget(rowIndex,colIndex,inputComboBox);
    colIndex++;

    QComboBox *ValAComboBox = new QComboBox;
    ValAComboBox->setProperty("row", rowIndex);
    ValAComboBox->setProperty("column", colIndex);
    ValAComboBox->addItems(QString("Timer time;Device Tag;Custom Static Val").split(";"));
    //ValAComboBox->setSizePolicy(QSizePolicy::Maximum);
    connect(ValAComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
    ui->logicSetupTableWidget->setCellWidget(rowIndex,colIndex,ValAComboBox);
    colIndex++;

    QComboBox *comparisonComboBox = new QComboBox;
    comparisonComboBox->setProperty("row",rowIndex);
    comparisonComboBox->setProperty("column", colIndex);
    comparisonComboBox->addItems(QString("Greater Than;Equal To;Less Than").split(";"));
    connect(comparisonComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
    ui->logicSetupTableWidget->setCellWidget(rowIndex,colIndex,comparisonComboBox);
    colIndex++;

    QComboBox *ValBComboBox = new QComboBox;
    ValBComboBox->setProperty("row",rowIndex);
    ValBComboBox->setProperty("column", colIndex);
    ValBComboBox->addItems(QString("Timer time;Device Tag;Custom Static Val").split(";"));
    //ValBComboBox->setSizePolicy(QSizePolicy::Maximum);
    connect(ValBComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
    ui->logicSetupTableWidget->setCellWidget(rowIndex,colIndex,ValBComboBox);
    colIndex++;

    QComboBox *reactionComboBox = new QComboBox;
    reactionComboBox->setProperty("row",rowIndex);
    reactionComboBox->setProperty("column", colIndex);
    reactionComboBox->addItems(QString("Set;Use;Call Function").split(";"));
    connect(reactionComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
    ui->logicSetupTableWidget->setCellWidget(rowIndex,colIndex,reactionComboBox);
    ui->logicSetupTableWidget->resizeColumnsToContents();
    colIndex++;

    QComboBox *reactionObjectComboBox = new QComboBox;
    reactionObjectComboBox->setProperty("row",rowIndex);
    reactionObjectComboBox->setProperty("column", colIndex);
    reactionObjectComboBox->addItems(QString("Object;Action").split(";"));
    reactionObjectComboBox->setCurrentIndex(3);
    connect(reactionObjectComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
    ui->logicSetupTableWidget->setCellWidget(rowIndex,colIndex,reactionObjectComboBox);
    ui->logicSetupTableWidget->resizeColumnsToContents();
    colIndex++;

    QComboBox *reactionMethodComboBox = new QComboBox;
    reactionMethodComboBox->setProperty("row",rowIndex);
    reactionMethodComboBox->setProperty("column", colIndex);
    reactionMethodComboBox->addItems(QString("True;False;Value").split(";"));
    reactionMethodComboBox->setCurrentIndex(3);
    connect(reactionMethodComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
    ui->logicSetupTableWidget->setCellWidget(rowIndex,colIndex,reactionMethodComboBox);
    ui->logicSetupTableWidget->resizeColumnsToContents();
    colIndex++;

    QComboBox *outputComboBox = new QComboBox;
    outputComboBox->setProperty("row",rowIndex);
    outputComboBox->setProperty("column", colIndex);
    outputComboBox->addItems(QString("Else;And;Or;End").split(";"));
    outputComboBox->setCurrentIndex(3);
    connect(outputComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
    ui->logicSetupTableWidget->setCellWidget(rowIndex,colIndex,outputComboBox);
    ui->logicSetupTableWidget->resizeColumnsToContents();
}

executeLogicSetupWindow::executeLogicSetupWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::executeLogicSetupWindow)
{
    ui->setupUi(this);
    ui->logicSetupTableWidget->setVisible(false);

    int maxRow = 2,maxCol = 4;

    treeModel = new QStandardItemModel(maxRow,maxCol,this);
    treeItemDelegate = new controlObjectItemDelegate(this);

    //tableView.setItemDelegateForColumn(1, delegate); // Column 0 can take any value, column 1 can only take values up to 8.
    ui->dynamicObjectTreeView->setModel(treeModel);
    ui->dynamicObjectTreeView->setItemDelegate(treeItemDelegate);


    QModelIndex rootIndex = treeModel->index(0,1,QModelIndex());
    ui->dynamicObjectTreeView->setRootIndex(rootIndex);
    QStandardItem *rootItem;
    treeModel->setItem(0,0,rootItem);


    for(int row = 1;row< maxRow;row++)
    {
        for(int col = 0;col< maxCol;col++)
        {
            QModelIndex index = treeModel->index(row,col,QModelIndex());

            treeModel->setData(index,"Click to Edit",Qt::EditRole);
        }
    }

    connect(treeModel,SIGNAL(itemChanged(QStandardItem*)),this,SLOT(OnTreeItemCBChanged(QStandardItem *)));

    //connect(treeModel,SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),this,SLOT(OnTreeDataChanged(QModelIndex,QModelIndex,QVector<int>)));
    ui->logicSetupTableWidget->setColumnCount(8);
    ui->logicSetupTableWidget->setRowCount(1);
    ui->logicSetupTableWidget->setHorizontalHeaderLabels(QString("Input Condition;Val A;Comparison;Val B;Reaction;Output Action").split(";"));

    methodIndex = 0;

    addComboRow(ui->logicSetupTableWidget->rowCount()-1);
}

executeLogicSetupWindow::~executeLogicSetupWindow()
{
    delete ui;
}


void executeLogicSetupWindow::insertChild()
{
    QModelIndex index = ui->dynamicObjectTreeView->selectionModel()->currentIndex();
    QAbstractItemModel *model = ui->dynamicObjectTreeView->model();

    if (model->columnCount(index) == 0) {
        if (!model->insertColumn(0, index))
            return;
    }

    if (!model->insertRow(0, index))
        return;

    for (int column = 0; column < model->columnCount(index); ++column) {
        QModelIndex child = model->index(0, column, index);
        model->setData(child, QVariant("[No data]"), Qt::EditRole);
        if (!model->headerData(column, Qt::Horizontal).isValid())
            model->setHeaderData(column, Qt::Horizontal, QVariant("[No header]"), Qt::EditRole);
    }

    ui->dynamicObjectTreeView->selectionModel()->setCurrentIndex(model->index(0, 0, index),
                                            QItemSelectionModel::ClearAndSelect);
    updateTreeView();
}

bool executeLogicSetupWindow::insertColumn()
{
    QAbstractItemModel *model = ui->dynamicObjectTreeView->model();
    int column = ui->dynamicObjectTreeView->selectionModel()->currentIndex().column();

    // Insert a column in the parent item.
    bool changed = model->insertColumn(column + 1);
    if (changed)
        model->setHeaderData(column + 1, Qt::Horizontal, QVariant("[No header]"), Qt::EditRole);

    updateTreeView();

    return changed;
}

void executeLogicSetupWindow::insertRow()
{
    QModelIndex index = ui->dynamicObjectTreeView->selectionModel()->currentIndex();
    QAbstractItemModel *model = ui->dynamicObjectTreeView->model();

    if (!model->insertRow(index.row()+1, index.parent()))
        return;

    updateTreeView();

    for (int column = 0; column < model->columnCount(index.parent()); ++column) {
        QModelIndex child = model->index(index.row()+1, column, index.parent());
        model->setData(child, QVariant("[No data]"), Qt::EditRole);
    }
}

bool executeLogicSetupWindow::removeColumn()
{
    QAbstractItemModel *model = ui->dynamicObjectTreeView->model();
    int column = ui->dynamicObjectTreeView->selectionModel()->currentIndex().column();

    // Insert columns in each child of the parent item.
    bool changed = model->removeColumn(column);

    if (changed)
        updateTreeView();

    return changed;
}

void executeLogicSetupWindow::removeRow()
{
    QModelIndex index = ui->dynamicObjectTreeView->selectionModel()->currentIndex();
    QAbstractItemModel *model = ui->dynamicObjectTreeView->model();
    if (model->removeRow(index.row(), index.parent()))
        updateTreeView();
}

void executeLogicSetupWindow::updateTreeView()
{
    bool hasSelection = !ui->dynamicObjectTreeView->selectionModel()->selection().isEmpty();
//    removeRowAction->setEnabled(hasSelection);
//    removeColumnAction->setEnabled(hasSelection);

    bool hasCurrent = ui->dynamicObjectTreeView->selectionModel()->currentIndex().isValid();
//    insertRowAction->setEnabled(hasCurrent);
//    insertColumnAction->setEnabled(hasCurrent);

    if (hasCurrent) {
        ui->dynamicObjectTreeView->closePersistentEditor(ui->dynamicObjectTreeView->selectionModel()->currentIndex());

        int row = ui->dynamicObjectTreeView->selectionModel()->currentIndex().row();
        int column = ui->dynamicObjectTreeView->selectionModel()->currentIndex().column();
        if (ui->dynamicObjectTreeView->selectionModel()->currentIndex().parent().isValid())
            statusBar()->showMessage(tr("Position: (%1,%2)").arg(row).arg(column));
        else
            statusBar()->showMessage(tr("Position: (%1,%2) in top level").arg(row).arg(column));
    }
}

void executeLogicSetupWindow::OnTreeDataChanged(QModelIndex indexone,QModelIndex indextwo,QVector<int> somevector)
{
//what I did
//    QComboBox* combo = qobject_cast<QComboBox*>(sender());
//    QModelIndex testIndex = testItem->index();
    qDebug() << "testItem->index();";

}



void executeLogicSetupWindow::OnTreeItemCBChanged(QStandardItem *testItem)
{
//what I did
//    QComboBox* combo = qobject_cast<QComboBox*>(sender());
//    QModelIndex testIndex = testItem->index();
//    qDebug() << testItem->index();
    int row = testItem->index().row();
    int col = testItem->index().column();

    //what stackoverflow suggested
    QVariant variant = testItem->data(Qt::UserRole+1);
    if(variant.isValid()){
        qDebug()<<variant.toInt();

        int comboIndex = -1;

        QStandardItem* child = new QStandardItem( tr("Child test") );

        insertChild();
        //testItem->appendRow(child);
    }
}

void executeLogicSetupWindow::OnComboIndexChanged(const QString& text)
{
    QComboBox* combo = qobject_cast<QComboBox*>(sender());
    if (combo)
    {

        int nRow = combo->property("row").toInt();
//        int nCol = sender()->property("column").toInt();

        switch (combo->property("column").toInt())
        {
        case 0:
            if(nRow < customObjectAttributes.inputType.size())
            {
                customObjectAttributes.inputType.push_back(combo->currentIndex());
            }
            else
            {
                customObjectAttributes.inputType.at(nRow) =combo->currentIndex();
            }
            break;
        case 1:
            if(nRow < customObjectAttributes.valueA.size())
            {
                customObjectAttributes.valueA.push_back(combo->currentIndex());
            }
            else
            {
                customObjectAttributes.valueA.at(nRow) = combo->currentIndex();
            }
            break;
        case 2:
            customObjectAttributes.comparisonType.push_back(combo->currentIndex());
            break;
        case 3:
            customObjectAttributes.valueB.push_back(combo->currentIndex());
            break;
        case 4:

            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            ui->logicSetupTableWidget->setRowCount(ui->logicSetupTableWidget->rowCount()+1);
            if(combo->currentIndex() != 3)
            {
                addComboRow(ui->logicSetupTableWidget->rowCount()-1);
            }
            customObjectAttributes.outputType.push_back(combo->currentIndex());
            break;
        }
    }
}

void executeLogicSetupWindow::on_newRowButton_clicked()
{
    ui->logicSetupTableWidget->setRowCount(ui->logicSetupTableWidget->rowCount()+1);
}

void executeLogicSetupWindow::on_newColumnButton_clicked()
{
    ui->logicSetupTableWidget->setColumnCount(ui->logicSetupTableWidget->columnCount()+1);
}

void executeLogicSetupWindow::on_confirmLogicButton_clicked()
{

}

//TODO rename class and this function to incorporate the idea of dynamic method creation
void executeLogicSetupWindow::translateLogic(int inputType,int comparisonType,double valueA,double valueB)
{

    switch (inputType) {
    case 0:
        switch (comparisonType) {
        case 0:
            if(valueA > valueB)
            {

            }
            break;
        case 1:
            if(valueA == valueB)
            {

            }
            break;
        case 2:
            if(valueA < valueB)
            {

            }
            break;
        }
        break;
    case 1:
        switch (comparisonType) {
        case 0:
            while(valueA > valueB)
            {

            }
            break;
        case 1:
            while(valueA == valueB)
            {

            }
            break;
        case 2:
            while(valueA < valueB)
            {

            }
            break;
        }
        break;
    }

}

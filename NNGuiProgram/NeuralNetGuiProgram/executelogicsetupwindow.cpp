#include "executelogicsetupwindow.h"
#include "ui_executelogicsetupwindow.h"
#include "QComboBox"
#include "QDebug"

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

    int maxRow = 3,maxCol = 2;
    treeModel = new QStandardItemModel(maxRow,maxCol,this);
    treeItemDelegate = new controlObjectItemDelegate(this);

    //tableView.setItemDelegateForColumn(1, delegate); // Column 0 can take any value, column 1 can only take values up to 8.
    ui->dynamicObjectTreeView->setModel(treeModel);
    ui->dynamicObjectTreeView->setItemDelegate(treeItemDelegate);



    for(int row = 0;row< maxRow;row++)
    {
        for(int col = 0;col< maxCol;col++)
        {
            QModelIndex index = treeModel->index(row,col,QModelIndex());
            //int value = (row+1) * (col+1);
            //treeModel->setData(index,QVariant(value),Qt::EditRole);
            treeModel->setData(index,"Click to Edit",Qt::EditRole);
        }
    }


    connect(treeModel,SIGNAL(itemChanged(QStandardItem*)),this,SLOT(OnTreeItemCBChanged(QStandardItem *)));


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


void executeLogicSetupWindow::OnTreeItemCBChanged(QStandardItem *testItem)
{

    QComboBox* combo = qobject_cast<QComboBox*>(sender());


    QModelIndex testIndex = testItem->index();
    qDebug() << testItem->index();
    int row = testItem->index().row();
    int col = testItem->index().column();
    QVariant value = testItem->data();

    //qDebug(testItem);
    if (combo)
    {
        qDebug("It worked");
    }
    else
    {
        qDebug("Guess its fucked");
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

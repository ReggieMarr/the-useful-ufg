#include "executelogicsetupwindow.h"
#include "ui_executelogicsetupwindow.h"
#include "QComboBox"
#include "QDebug"

executeLogicSetupWindow::executeLogicSetupWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::executeLogicSetupWindow)
{
    ui->setupUi(this);


    ui->logicSetupTableWidget->setColumnCount(6);
    ui->logicSetupTableWidget->setRowCount(1);
    ui->logicSetupTableWidget->setHorizontalHeaderLabels(QString("Input Condition;Val A;Comparison;Val B;Reaction;Output Action").split(";"));

    //its not actually neccessary to name each of these seperately
    QComboBox *inputComboBox = new QComboBox;
    inputComboBox->setProperty("row", ui->logicSetupTableWidget->rowCount() -1);
    inputComboBox->setProperty("col", 0);
    inputComboBox->addItems(QString("If;While").split(";"));
    connect(inputComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
    ui->logicSetupTableWidget->setCellWidget(0,0,inputComboBox);

    QComboBox *ValAComboBox = new QComboBox;
    ValAComboBox->setProperty("row", ui->logicSetupTableWidget->rowCount() -1);
    ValAComboBox->setProperty("col", 1);
    ValAComboBox->addItems(QString("Tag1;Tag2;Tag3;Tag4").split(";"));
    connect(ValAComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
    ui->logicSetupTableWidget->setCellWidget(0,1,ValAComboBox);

    QComboBox *comparisonComboBox = new QComboBox;
    comparisonComboBox->setProperty("row", ui->logicSetupTableWidget->rowCount() -1);
    comparisonComboBox->setProperty("col", 2);
    comparisonComboBox->addItems(QString("Greater Than;Equal To;Less Than").split(";"));
    connect(comparisonComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
    ui->logicSetupTableWidget->setCellWidget(0,2,comparisonComboBox);


    QComboBox *ValBComboBox = new QComboBox;
    ValBComboBox->setProperty("row", ui->logicSetupTableWidget->rowCount() -1);
    ValBComboBox->setProperty("col", 3);
    ValBComboBox->addItems(QString("Tag1;Tag2;Tag3;Tag4").split(";"));
    connect(ValBComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
    ui->logicSetupTableWidget->setCellWidget(0,3,ValBComboBox);

    QComboBox *reactionComboBox = new QComboBox;
    reactionComboBox->setProperty("row", ui->logicSetupTableWidget->rowCount() -1);
    reactionComboBox->setProperty("col", 4);
    reactionComboBox->addItems(QString("Set;Use").split(";"));
    connect(reactionComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
    ui->logicSetupTableWidget->setCellWidget(0,4,reactionComboBox);
    ui->logicSetupTableWidget->resizeColumnsToContents();

    QComboBox *outputComboBox = new QComboBox;
    outputComboBox->setProperty("row", ui->logicSetupTableWidget->rowCount() -1);
    outputComboBox->setProperty("col", 5);
    outputComboBox->addItems(QString("Else;And;Or;End").split(";"));
    connect(outputComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
    ui->logicSetupTableWidget->setCellWidget(0,5,outputComboBox);
    ui->logicSetupTableWidget->resizeColumnsToContents();

}

executeLogicSetupWindow::~executeLogicSetupWindow()
{
    delete ui;
}

void executeLogicSetupWindow::OnComboIndexChanged(const QString& text)
{
    QComboBox* combo = qobject_cast<QComboBox*>(sender());
    if (combo)
    {

        switch (combo->property("column").toInt())
        {
        case 0:
            setupLogic.inputType.push_back(combo->currentIndex());
            break;
        case 1:
            //this could later refer to the index num of the tag struct in runtime_window
            setupLogic.valueA.push_back(combo->currentIndex());
            break;
        case 2:
            setupLogic.comparisonType.push_back(combo->currentIndex());
            break;
        case 3:
            setupLogic.valueB.push_back(combo->currentIndex());
            break;
        case 4:
            setupLogic.outputType.push_back(combo->currentIndex());
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

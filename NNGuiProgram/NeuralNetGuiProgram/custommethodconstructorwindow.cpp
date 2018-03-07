/*
 * Copyright © 2001-2011 Reginald Marr <reginald.t.marr@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */
#include "custommethodconstructorwindow.h"
#include "ui_custommethodconstructorwindow.h"
#include "QStackedWidget"
#include "QComboBox"
#include "QTextEdit"
#include "QLineEdit"
#include "QPushButton"
#include "iostream"

Q_DECLARE_METATYPE(QTreeWidgetItem*)
Q_DECLARE_METATYPE(QStackedWidget*)
Q_DECLARE_METATYPE(QFrame*)

customMethodConstructorWindow::customMethodConstructorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::customMethodConstructorWindow)
{
    ui->setupUi(this);
    treeItemDelegate = new controlObjectItemDelegate(this);
    ui->methodSetupTreeWidget->setItemDelegate(treeItemDelegate);
    ui->methodSetupTreeWidget->setColumnCount(5);


    //this is all done to then later populate the comboboxes
    //tags could be dynamically passed into the setupblock
    std::vector<QStringList> setup,reaction;

    setup.push_back(QString("If;While").split(";"));
    setup.push_back(QString("Time Passed;Static Var;Tag Var").split(";"));
    setup.push_back(QString("<;=;!=;>").split(";"));
    setup.push_back(QString("Time Passed;Static Var;Tag Var").split(";"));
    setup.push_back(QString("Continue;And;Or").split(";"));

    reaction.push_back(QString("Action;Item;Static Var").split(";"));
    reaction.push_back(QString("End;Else").split(";"));
    reaction.push_back(QString("End;Break").split(";"));

    methodBlocks.push_back(setup);
    methodBlocks.push_back(reaction);

    AddRoot("Method One");
    for(int i = 0; i < ui->methodSetupTreeWidget->columnCount();i++)
    {
        ui->methodSetupTreeWidget->resizeColumnToContents(i);
    }

    childColumnOffset = 0;
    methodHasIfChild = false;
    ui->newMethodBtn->setEnabled(false);
    ui->addMethodBtn->setEnabled(false);
}

customMethodConstructorWindow::~customMethodConstructorWindow()
{
    delete ui;
}

void customMethodConstructorWindow::addChildRow(QTreeWidget *widgetParent, QTreeWidgetItem *itemParent, bool fillSelections,
                                                int layoutType,int rootIndex)
{
    //default is to fill child row with time comparison
    //TODO: fix this in the future
    //widgetParent->setColumnCount(methodBlocks.at(rowType).size());

    QTreeWidgetItem *childItem = new QTreeWidgetItem(itemParent);

    QVariant childItemVariant,widgetParentVarient;
    widgetParentVarient.setValue(widgetParent);
    childItemVariant.setValue(childItem);
    int widgetType,methodBlockCount = 0,rowType = 0;
    int cycleSetup = 0,cycleSize;
    int timeLineEditLocation = 2,lineEditLocation = 4 + timeLineEditLocation;
    int childColumnOffset;

    switch (layoutType) {
    case 0:
    {
        cycleSize = methodBlockCount+methodBlocks.at(rowType).size() + 2;
        widgetParent->setColumnCount(cycleSize);
        timeLineEditLocation = 2,lineEditLocation = 5;
        while (cycleSetup < cycleSize)
        {
            QComboBox *itemComboBox = new QComboBox;
            itemComboBox->setProperty("rowType", rowType);//First row in a method
            itemComboBox->setProperty("row", childColumnOffset);
            itemComboBox->setProperty("column",cycleSetup);
            itemComboBox->setProperty("childItem",childItemVariant);itemComboBox->clear();itemComboBox->clear();
            if(cycleSetup == lineEditLocation-1)
            {
                for(uint i = 1;i< methodBlocks.at(rowType).at(methodBlockCount).size();i++)
                {
                    itemComboBox->addItem(methodBlocks.at(rowType).at(methodBlockCount).at(i));
                }
            }
            else if(cycleSetup == timeLineEditLocation-1)
            {
                itemComboBox->addItem(methodBlocks.at(rowType).at(methodBlockCount).at(0));
                for(uint i = 2;i< methodBlocks.at(rowType).at(methodBlockCount).size();i++)
                {
                    itemComboBox->addItem(methodBlocks.at(rowType).at(methodBlockCount).at(i));
                }
            }
            else
            {
                itemComboBox->addItems(methodBlocks.at(rowType).at(methodBlockCount));
            }

            QVariant itemWidgetVar;
            itemWidgetVar.setValue(itemComboBox);
            childItem->setData(cycleSetup,Qt::UserRole,itemWidgetVar);
            widgetType = 0;//this means its a method blocks type combobox
            childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
            if(fillSelections && customObjectMethods.selectionType.at(childColumnOffset).at(cycleSetup) == 0)
            {
                itemComboBox->setCurrentIndex((int)customObjectMethods.userSelection.at(childColumnOffset).at(cycleSetup));
            }
            QObject::connect(itemComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
            widgetParent->setItemWidget(childItem,cycleSetup,itemComboBox);
            itemParent->addChild(childItem);
            cycleSetup++;
            methodBlockCount++;

            if(cycleSetup == timeLineEditLocation)
            {
                QVariant timeCBWidgetVar;
                QComboBox *timeItemCombobox = new QComboBox;
                timeItemCombobox->setProperty("rowType", rowType);//First row in a method
                timeItemCombobox->setProperty("row", childColumnOffset);
                timeItemCombobox->setProperty("column",cycleSetup);
                timeItemCombobox->setProperty("childItem",childItemVariant);
                timeItemCombobox->addItems(QString("Seconds;MilliSeconds").split(";"));

                timeCBWidgetVar.setValue(timeItemCombobox);
                childItem->setData(cycleSetup,Qt::UserRole,timeCBWidgetVar);
                widgetType = 2;//this means its a time passed type
                childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
                if(fillSelections&& customObjectMethods.selectionType.at(childColumnOffset).at(cycleSetup) == 2)
                {
                    timeItemCombobox->setCurrentIndex((int)customObjectMethods.userSelection.at(childColumnOffset).at(cycleSetup));
                }
                QObject::connect(timeItemCombobox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnTimeBoxChanged(const QString&)));
                widgetParent->setItemWidget(childItem,cycleSetup,timeItemCombobox);
                itemParent->addChild(childItem);
                cycleSetup++;
            }
            else if(cycleSetup == lineEditLocation)
            {
                QVariant timeLEWidgetVar;
                QLineEdit *itemLineEdit = new QLineEdit;
                itemLineEdit->setProperty("rowType", rowType);
                itemLineEdit->setProperty("row", 0);
                itemLineEdit->setProperty("column",cycleSetup);
                itemLineEdit->setProperty("widgetParent",widgetParentVarient);
                itemLineEdit->setProperty("childItem",childItemVariant);
                //QObject::connect(itemComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnTimeBoxChanged(const QString&)));
                timeLEWidgetVar.setValue(itemLineEdit);
                childItem->setData(cycleSetup,Qt::UserRole,timeLEWidgetVar);
                widgetType = 1;//this means its a time passed type combobox
                childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
                if(fillSelections && customObjectMethods.selectionType.at(childColumnOffset).at(cycleSetup) == 0)
                {
                    itemLineEdit->setText(QString::number(customObjectMethods.userSelection.at(childColumnOffset).at(cycleSetup)));
                }
                widgetParent->setItemWidget(childItem,cycleSetup,itemLineEdit);
                itemParent->addChild(childItem);
                cycleSetup++;
            }

        }

        fillSelections = false;

    }
        break;
    case 1:
    {

        cycleSize = methodBlockCount+methodBlocks.at(rowType).size() + 2;
        widgetParent->setColumnCount(cycleSize);
        timeLineEditLocation = 5,lineEditLocation = 2;
        while (cycleSetup < cycleSize)
        {
            QComboBox *itemComboBox = new QComboBox;
            itemComboBox->setProperty("rowType", rowType);//First row in a method
            itemComboBox->setProperty("row", childColumnOffset);
            itemComboBox->setProperty("column",cycleSetup);
            itemComboBox->setProperty("childItem",childItemVariant);itemComboBox->clear();itemComboBox->clear();
            if(cycleSetup == lineEditLocation-1)
            {
                for(uint i = 1;i< methodBlocks.at(rowType).at(methodBlockCount).size();i++)
                {
                    itemComboBox->addItem(methodBlocks.at(rowType).at(methodBlockCount).at(i));
                }
            }
            else if(cycleSetup == timeLineEditLocation-1)
            {
                itemComboBox->addItem(methodBlocks.at(rowType).at(methodBlockCount).at(0));
                for(uint i = 2;i< methodBlocks.at(rowType).at(methodBlockCount).size();i++)
                {
                    itemComboBox->addItem(methodBlocks.at(rowType).at(methodBlockCount).at(i));
                }
            }
            else
            {
                itemComboBox->addItems(methodBlocks.at(rowType).at(methodBlockCount));
            }


            QObject::connect(itemComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
            QVariant itemWidgetVar;
            itemWidgetVar.setValue(itemComboBox);
            childItem->setData(cycleSetup,Qt::UserRole,itemWidgetVar);
            widgetType = 0;//this means its a method blocks type combobox
            childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
            if(fillSelections && customObjectMethods.selectionType.at(childColumnOffset).at(cycleSetup) == 0)
            {
                itemComboBox->setCurrentIndex((int)customObjectMethods.userSelection.at(childColumnOffset).at(cycleSetup));
            }
            widgetParent->setItemWidget(childItem,cycleSetup,itemComboBox);
            itemParent->addChild(childItem);
            cycleSetup++;
            methodBlockCount++;

            if(cycleSetup == timeLineEditLocation)
            {
                QVariant timeCBWidgetVar;
                QComboBox *timeItemCombobox = new QComboBox;
                timeItemCombobox->setProperty("rowType", rowType);//First row in a method
                timeItemCombobox->setProperty("row", childColumnOffset);
                timeItemCombobox->setProperty("column",cycleSetup);
                timeItemCombobox->setProperty("childItem",childItemVariant);
                timeItemCombobox->addItems(QString("Seconds;MilliSeconds;Reset").split(";"));
                QObject::connect(timeItemCombobox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnTimeBoxChanged(const QString&)));
                timeCBWidgetVar.setValue(timeItemCombobox);
                childItem->setData(cycleSetup,Qt::UserRole,timeCBWidgetVar);
                widgetType = 2;//this means its a time passed type
                childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
                if(fillSelections&& customObjectMethods.selectionType.at(childColumnOffset).at(cycleSetup) == 2)
                {
                    timeItemCombobox->setCurrentIndex((int)customObjectMethods.userSelection.at(childColumnOffset).at(cycleSetup));
                }
                widgetParent->setItemWidget(childItem,cycleSetup,timeItemCombobox);
                itemParent->addChild(childItem);
                cycleSetup++;
            }
            else if(cycleSetup == lineEditLocation)
            {
                QVariant timeLEWidgetVar;
                QLineEdit *itemLineEdit = new QLineEdit;
                itemLineEdit->setProperty("rowType", rowType);
                itemLineEdit->setProperty("row", 0);
                itemLineEdit->setProperty("column",cycleSetup);
                itemLineEdit->setProperty("widgetParent",widgetParentVarient);
                itemLineEdit->setProperty("childItem",childItemVariant);
                //QObject::connect(itemComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnTimeBoxChanged(const QString&)));
                timeLEWidgetVar.setValue(itemLineEdit);
                childItem->setData(cycleSetup,Qt::UserRole,timeLEWidgetVar);
                widgetType = 1;//this means its a time passed type combobox
                childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
                if(fillSelections && customObjectMethods.selectionType.at(childColumnOffset).at(cycleSetup) == 0)
                {
                    itemLineEdit->setText(QString::number(customObjectMethods.userSelection.at(childColumnOffset).at(cycleSetup)));
                }
                widgetParent->setItemWidget(childItem,cycleSetup,itemLineEdit);
                itemParent->addChild(childItem);
                cycleSetup++;
            }

        }


    }
        break;
    case 2:
    {
        cycleSize =  methodBlockCount+methodBlocks.at(rowType).size() + 1;
        widgetParent->setColumnCount(cycleSize);
        timeLineEditLocation = 2;
        while (cycleSetup < cycleSize)
        {
            QComboBox *itemComboBox = new QComboBox;
            itemComboBox->setProperty("rowType", rowType);//First row in a method
            itemComboBox->setProperty("row", childColumnOffset);
            itemComboBox->setProperty("column",cycleSetup);
            itemComboBox->setProperty("childItem",childItemVariant);itemComboBox->clear();
            if(cycleSetup == 4)
            {
                for(uint i = 1;i< methodBlocks.at(rowType).at(methodBlockCount).size();i++)
                {
                    itemComboBox->addItem(methodBlocks.at(rowType).at(methodBlockCount).at(i));
                }
                itemComboBox->setCurrentIndex(1);
            }
            else
            {
                itemComboBox->addItems(methodBlocks.at(rowType).at(methodBlockCount));
            }
            QObject::connect(itemComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
            QVariant itemWidgetVar;
            itemWidgetVar.setValue(itemComboBox);
            childItem->setData(cycleSetup,Qt::UserRole,itemWidgetVar);
            widgetType = 0;//this means its a method blocks type combobox
            childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
            if(fillSelections && customObjectMethods.selectionType.at(childColumnOffset).at(cycleSetup) == 0)
            {
                itemComboBox->setCurrentIndex((int)customObjectMethods.userSelection.at(childColumnOffset).at(cycleSetup));
            }
            widgetParent->setItemWidget(childItem,cycleSetup,itemComboBox);
            itemParent->addChild(childItem);
            cycleSetup++;
            methodBlockCount++;

            if(cycleSetup == timeLineEditLocation)
            {
                QVariant timeCBWidgetVar;
                QComboBox *timeItemCombobox = new QComboBox;
                timeItemCombobox->setProperty("rowType", rowType);//First row in a method
                timeItemCombobox->setProperty("row", childColumnOffset);
                timeItemCombobox->setProperty("column",cycleSetup);
                timeItemCombobox->setProperty("childItem",childItemVariant);
                timeItemCombobox->addItems(QString("Seconds;MilliSeconds").split(";"));
                QObject::connect(timeItemCombobox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnTimeBoxChanged(const QString&)));
                timeCBWidgetVar.setValue(timeItemCombobox);
                childItem->setData(cycleSetup,Qt::UserRole,timeCBWidgetVar);
                widgetType = 2;//this means its a time passed type
                childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
                if(fillSelections&& customObjectMethods.selectionType.at(childColumnOffset).at(cycleSetup) == 2)
                {
                    timeItemCombobox->setCurrentIndex((int)customObjectMethods.userSelection.at(childColumnOffset).at(cycleSetup));
                }
                widgetParent->setItemWidget(childItem,cycleSetup,timeItemCombobox);
                itemParent->addChild(childItem);
                cycleSetup++;
            }


        }
    }
        break;
    case 3:
    {
        cycleSize = methodBlockCount+methodBlocks.at(rowType).size() + 1;
        widgetParent->setColumnCount(cycleSize);
        timeLineEditLocation = 4;
        while (cycleSetup < cycleSize)
        {
            QComboBox *itemComboBox = new QComboBox;
            itemComboBox->setProperty("rowType", rowType);//First row in a method
            itemComboBox->setProperty("row", childColumnOffset);
            itemComboBox->setProperty("column",cycleSetup);
            itemComboBox->setProperty("childItem",childItemVariant);itemComboBox->clear();itemComboBox->clear();

            if(cycleSetup == 1)
            {
                for(uint i = 1;i< methodBlocks.at(rowType).at(methodBlockCount).size();i++)
                {
                    itemComboBox->addItem(methodBlocks.at(rowType).at(methodBlockCount).at(i));
                }
                itemComboBox->setCurrentIndex(1);
            }
            else
            {
                itemComboBox->addItems(methodBlocks.at(rowType).at(methodBlockCount));
            }

            QObject::connect(itemComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
            QVariant itemWidgetVar;
            itemWidgetVar.setValue(itemComboBox);
            childItem->setData(cycleSetup,Qt::UserRole,itemWidgetVar);
            widgetType = 0;//this means its a method blocks type combobox
            childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
            if(fillSelections && customObjectMethods.selectionType.at(childColumnOffset).at(cycleSetup) == 0)
            {
                itemComboBox->setCurrentIndex((int)customObjectMethods.userSelection.at(childColumnOffset).at(cycleSetup));
            }
            widgetParent->setItemWidget(childItem,cycleSetup,itemComboBox);
            itemParent->addChild(childItem);
            cycleSetup++;
            methodBlockCount++;

            if(cycleSetup == timeLineEditLocation)
            {
                QVariant timeCBWidgetVar;
                QComboBox *timeItemCombobox = new QComboBox;
                timeItemCombobox->setProperty("rowType", rowType);//First row in a method
                timeItemCombobox->setProperty("row", childColumnOffset);
                timeItemCombobox->setProperty("column",cycleSetup);
                timeItemCombobox->setProperty("childItem",childItemVariant);
                timeItemCombobox->addItems(QString("Seconds;MilliSeconds;Reset").split(";"));
                QObject::connect(timeItemCombobox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnTimeBoxChanged(const QString&)));
                timeCBWidgetVar.setValue(timeItemCombobox);
                childItem->setData(cycleSetup,Qt::UserRole,timeCBWidgetVar);
                widgetType = 2;//this means its a time passed type
                childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
                if(fillSelections&& customObjectMethods.selectionType.at(childColumnOffset).at(cycleSetup) == 2)
                {
                    timeItemCombobox->setCurrentIndex((int)customObjectMethods.userSelection.at(childColumnOffset).at(cycleSetup));
                }
                widgetParent->setItemWidget(childItem,cycleSetup,timeItemCombobox);
                itemParent->addChild(childItem);
                cycleSetup++;
            }

        }
    }
        break;
    case 4:
    {
        cycleSize = methodBlockCount+methodBlocks.at(rowType).size() + 1;
        widgetParent->setColumnCount(cycleSize);
        lineEditLocation = 2;
        while (cycleSetup < cycleSize)
        {
            QComboBox *itemComboBox = new QComboBox;
            itemComboBox->setProperty("rowType", rowType);//First row in a method
            itemComboBox->setProperty("row", childColumnOffset);
            itemComboBox->setProperty("column",cycleSetup);
            itemComboBox->setProperty("childItem",childItemVariant);itemComboBox->clear();itemComboBox->clear();
            if(cycleSetup == lineEditLocation-1)
            {
                itemComboBox->addItems(methodBlocks.at(rowType).at(methodBlockCount));
                itemComboBox->setCurrentIndex(1);
            }
            else if(cycleSetup == 4)
            {
                itemComboBox->addItem(methodBlocks.at(rowType).at(methodBlockCount).at(0));
                for(uint i = 2;i< methodBlocks.at(rowType).at(methodBlockCount).size();i++)
                {
                    itemComboBox->addItem(methodBlocks.at(rowType).at(methodBlockCount).at(i));
                }
                itemComboBox->setCurrentIndex(1);

            }
            else
            {
                itemComboBox->addItems(methodBlocks.at(rowType).at(methodBlockCount));
            }

            QObject::connect(itemComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
            QVariant itemWidgetVar;
            itemWidgetVar.setValue(itemComboBox);
            childItem->setData(cycleSetup,Qt::UserRole,itemWidgetVar);
            widgetType = 0;//this means its a method blocks type combobox
            childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
            if(fillSelections && customObjectMethods.selectionType.at(childColumnOffset).at(cycleSetup) == 0)
            {
                itemComboBox->setCurrentIndex((int)customObjectMethods.userSelection.at(childColumnOffset).at(cycleSetup));
            }
            widgetParent->setItemWidget(childItem,cycleSetup,itemComboBox);
            itemParent->addChild(childItem);
            cycleSetup++;
            methodBlockCount++;

            if(cycleSetup == lineEditLocation)
            {
                QVariant timeLEWidgetVar;
                QLineEdit *itemLineEdit = new QLineEdit;
                itemLineEdit->setProperty("rowType", rowType);
                itemLineEdit->setProperty("row", 0);
                itemLineEdit->setProperty("column",cycleSetup);
                itemLineEdit->setProperty("widgetParent",widgetParentVarient);
                itemLineEdit->setProperty("childItem",childItemVariant);
                //QObject::connect(itemComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnTimeBoxChanged(const QString&)));
                timeLEWidgetVar.setValue(itemLineEdit);
                childItem->setData(cycleSetup,Qt::UserRole,timeLEWidgetVar);
                widgetType = 1;//this means its a time passed type combobox
                childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
                if(fillSelections && customObjectMethods.selectionType.at(childColumnOffset).at(cycleSetup) == 0)
                {
                    itemLineEdit->setText(QString::number(customObjectMethods.userSelection.at(childColumnOffset).at(cycleSetup)));
                }
                widgetParent->setItemWidget(childItem,cycleSetup,itemLineEdit);
                itemParent->addChild(childItem);
                cycleSetup++;
            }

        }


    }
        break;
    case 5:
    {
        cycleSize = methodBlockCount+methodBlocks.at(rowType).size() + 1;
        widgetParent->setColumnCount(cycleSize);
        lineEditLocation = 4;
        while (cycleSetup < cycleSize)
        {
            QComboBox *itemComboBox = new QComboBox;
            itemComboBox->setProperty("rowType", rowType);//First row in a method
            itemComboBox->setProperty("row", childColumnOffset);
            itemComboBox->setProperty("column",cycleSetup);
            itemComboBox->setProperty("childItem",childItemVariant);itemComboBox->clear();

            if(cycleSetup == lineEditLocation-1)
            {
                itemComboBox->addItems(methodBlocks.at(rowType).at(methodBlockCount));
                itemComboBox->setCurrentIndex(1);
            }
            else if(cycleSetup == 1)
            {
                itemComboBox->addItem(methodBlocks.at(rowType).at(methodBlockCount).at(0));
                for(uint i = 2;i< methodBlocks.at(rowType).at(methodBlockCount).size();i++)
                {
                    itemComboBox->addItem(methodBlocks.at(rowType).at(methodBlockCount).at(i));
                }
                itemComboBox->setCurrentIndex(1);
            }
            else
            {
                itemComboBox->addItems(methodBlocks.at(rowType).at(methodBlockCount));
            }
            QObject::connect(itemComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
            QVariant itemWidgetVar;
            itemWidgetVar.setValue(itemComboBox);
            childItem->setData(cycleSetup,Qt::UserRole,itemWidgetVar);
            widgetType = 0;//this means its a method blocks type combobox
            childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
            if(fillSelections && customObjectMethods.selectionType.at(childColumnOffset).at(cycleSetup) == 0)
            {
                itemComboBox->setCurrentIndex((int)customObjectMethods.userSelection.at(childColumnOffset).at(cycleSetup));
            }
            widgetParent->setItemWidget(childItem,cycleSetup,itemComboBox);
            itemParent->addChild(childItem);
            cycleSetup++;
            methodBlockCount++;

            if(cycleSetup == lineEditLocation)
            {
                QVariant timeLEWidgetVar;
                QLineEdit *itemLineEdit = new QLineEdit;
                itemLineEdit->setProperty("rowType", rowType);
                itemLineEdit->setProperty("row", 0);
                itemLineEdit->setProperty("column",cycleSetup);
                itemLineEdit->setProperty("widgetParent",widgetParentVarient);
                itemLineEdit->setProperty("childItem",childItemVariant);
                //QObject::connect(itemComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnTimeBoxChanged(const QString&)));
                timeLEWidgetVar.setValue(itemLineEdit);
                childItem->setData(cycleSetup,Qt::UserRole,timeLEWidgetVar);
                widgetType = 1;//this means its a time passed type combobox
                childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
                if(fillSelections && customObjectMethods.selectionType.at(childColumnOffset).at(cycleSetup) == 0)
                {
                    itemLineEdit->setText(QString::number(customObjectMethods.userSelection.at(childColumnOffset).at(cycleSetup)));
                }
                widgetParent->setItemWidget(childItem,cycleSetup,itemLineEdit);
                itemParent->addChild(childItem);
                cycleSetup++;
            }

        }

    }
        break;
    case 6:
    {
        cycleSize = methodBlockCount+methodBlocks.at(rowType).size();
        widgetParent->setColumnCount(cycleSize);
        while (cycleSetup < cycleSize)
        {
            QComboBox *itemComboBox = new QComboBox;
            itemComboBox->setProperty("rowType", rowType);//First row in a method
            itemComboBox->setProperty("row", childColumnOffset);
            itemComboBox->setProperty("column",cycleSetup);
            itemComboBox->setProperty("childItem",childItemVariant);itemComboBox->clear();
            itemComboBox->addItems(methodBlocks.at(rowType).at(methodBlockCount));
            if(cycleSetup == 1)
            {
                itemComboBox->setCurrentIndex(2);
            }
            else if(cycleSetup == 3)
            {
                itemComboBox->setCurrentIndex(2);
            }
            QObject::connect(itemComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
            QVariant itemWidgetVar;
            itemWidgetVar.setValue(itemComboBox);
            childItem->setData(cycleSetup,Qt::UserRole,itemWidgetVar);
            widgetType = 0;//this means its a method blocks type combobox
            childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
            if(fillSelections && customObjectMethods.selectionType.at(childColumnOffset).at(cycleSetup) == 0)
            {
                itemComboBox->setCurrentIndex((int)customObjectMethods.userSelection.at(childColumnOffset).at(cycleSetup));
            }
            widgetParent->setItemWidget(childItem,cycleSetup,itemComboBox);
            itemParent->addChild(childItem);
            cycleSetup++;
            methodBlockCount++;
        }
    }
        break;
    case 7:
    {
        cycleSize = methodBlockCount+methodBlocks.at(rowType).size() + 1;
        widgetParent->setColumnCount(cycleSize);
        timeLineEditLocation = 4;
        while (cycleSetup < cycleSize)
        {
            QComboBox *itemComboBox = new QComboBox;
            itemComboBox->setProperty("rowType", rowType);//First row in a method
            itemComboBox->setProperty("row", childColumnOffset);
            itemComboBox->setProperty("column",cycleSetup);
            itemComboBox->setProperty("childItem",childItemVariant);itemComboBox->clear();itemComboBox->clear();

            if(cycleSetup == 1)
            {
                for(uint i = 1;i< methodBlocks.at(rowType).at(methodBlockCount).size();i++)
                {
                    itemComboBox->addItem(methodBlocks.at(rowType).at(methodBlockCount).at(i));
                }
                itemComboBox->setCurrentIndex(1);
            }
            else
            {
                itemComboBox->addItems(methodBlocks.at(rowType).at(methodBlockCount));
            }

            QObject::connect(itemComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
            QVariant itemWidgetVar;
            itemWidgetVar.setValue(itemComboBox);
            childItem->setData(cycleSetup,Qt::UserRole,itemWidgetVar);
            widgetType = 0;//this means its a method blocks type combobox
            childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
            if(fillSelections && customObjectMethods.selectionType.at(childColumnOffset).at(cycleSetup) == 0)
            {
                itemComboBox->setCurrentIndex((int)customObjectMethods.userSelection.at(childColumnOffset).at(cycleSetup));
            }
            widgetParent->setItemWidget(childItem,cycleSetup,itemComboBox);
            itemParent->addChild(childItem);
            cycleSetup++;
            methodBlockCount++;

            if(cycleSetup == timeLineEditLocation)
            {
                QVariant timeCBWidgetVar,timeLEWidgetVar;
                QComboBox *timeItemCombobox = new QComboBox;
                timeItemCombobox->setProperty("rowType", rowType);//First row in a method
                timeItemCombobox->setProperty("row", childColumnOffset);
                timeItemCombobox->setProperty("column",cycleSetup);
                timeItemCombobox->setProperty("childItem",childItemVariant);
                timeItemCombobox->addItems(QString("Seconds;MilliSeconds").split(";"));
                QObject::connect(timeItemCombobox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnTimeBoxChanged(const QString&)));
                timeCBWidgetVar.setValue(timeItemCombobox);
                childItem->setData(cycleSetup,Qt::UserRole,timeLEWidgetVar);
                widgetType = 2;//this means its a time passed type
                childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
                if(fillSelections&& customObjectMethods.selectionType.at(childColumnOffset).at(cycleSetup) == 2)
                {
                    timeItemCombobox->setCurrentIndex((int)customObjectMethods.userSelection.at(childColumnOffset).at(cycleSetup));
                }
                widgetParent->setItemWidget(childItem,cycleSetup,timeItemCombobox);
                itemParent->addChild(childItem);
                cycleSetup++;
            }

        }

    }
        break;
    }

    childItem->setData(0,Qt::UserRole+2,QVariant::fromValue(cycleSize)); //gives the columnCount
    childItem->setData(0,Qt::UserRole+3,layoutType);//indicates layout type
    childItem->setData(0,Qt::UserRole+4,QVariant::fromValue(rootIndex));
    widgetParent->setColumnCount(cycleSize);
    for(int i = 0; i < widgetParent->columnCount();i++)
    {
        widgetParent->resizeColumnToContents(i);
    }

}

void customMethodConstructorWindow::addChildRow(QTreeWidget *widgetParent, QTreeWidgetItem *itemParent, int rowType)
{
    //default is to fill child row with time comparison
    //TODO: fix this in the future
    //widgetParent->setColumnCount(methodBlocks.at(rowType).size());

    QTreeWidgetItem *childItem = new QTreeWidgetItem;//new QTreeWidgetItem(itemParent);
    itemParent->addChild(childItem);

    QComboBox *item0ComboBox = new QComboBox;
    item0ComboBox->setProperty("rowType", rowType);
    item0ComboBox->setProperty("row", 1);
    item0ComboBox->setProperty("column", 0);
    item0ComboBox->addItems(methodBlocks.at(1).at(0));
    //QObject::connect(item0ComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
    widgetParent->setItemWidget(childItem,0,item0ComboBox);
    itemParent->addChild(childItem);

    QComboBox *item1ComboBox = new QComboBox;
    item1ComboBox->setProperty("rowType", rowType);
    item1ComboBox->setProperty("row", 1);
    item1ComboBox->setProperty("column", 1);
    item1ComboBox->addItems(methodBlocks.at(1).at(rowType));
    //QObject::connect(item1ComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
    widgetParent->setItemWidget(childItem,1,item1ComboBox);
    itemParent->addChild(childItem);

}

void customMethodConstructorWindow::AddRoot(QString rootName)
{
    QTreeWidgetItem *rootItem = new QTreeWidgetItem(ui->methodSetupTreeWidget);

    rootItem->setText(0,rootName);

    treeRootNames.push_back(rootName);

    ui->methodSetupTreeWidget->addTopLevelItem(rootItem);

    addChildRow(ui->methodSetupTreeWidget,rootItem,0,0,treeRootNames.size()-1);

    addChildRow(ui->methodSetupTreeWidget,rootItem->child(0),0);
    //AddChild(rootItem,QString("one;two;three").split(";"));

}

void customMethodConstructorWindow::AddChild(QTreeWidgetItem *parent,QStringList rootNames)
{
    QTreeWidgetItem *childItem = new QTreeWidgetItem(parent);
    for(int i = 0;i< rootNames.size();i++)
    {
        childItem->setText(i,rootNames.at(i));
    }

    parent->addChild(childItem);
}

void customMethodConstructorWindow::OnTimeBoxChanged(const QString& text)
{

}

void customMethodConstructorWindow::OnComboIndexChanged(const QString& text)
{
    ui->addMethodBtn->setEnabled(true);
    QComboBox* combo = qobject_cast<QComboBox*>(sender());
    if (combo)
    {
        //int nRow = combo->property("row").toInt();
        int nCol = combo->property("column").toInt();
        QTreeWidgetItem *childItem = combo->property("childItem").value<QTreeWidgetItem*>();
        unsigned rootIndex = childItem->data(0,Qt::UserRole+4).toInt();
        QTreeWidgetItem *parentItem = childItem->parent()->treeWidget()->topLevelItem(rootIndex);//childItem->parent();

        unsigned childIndex = parentItem->indexOfChild(childItem);
        unsigned currentColumnCount = childItem->data(0,Qt::UserRole+2).toInt();

        QTreeWidget* widgetParent = parentItem->treeWidget();

        unsigned currentLayoutType = childItem->data(0,Qt::UserRole+3).toInt();
        //note that the number of columns is offset by one with reference to the nCol

        //this was originally for the the last column, where we were to create the if and, or ect
        if(nCol == currentColumnCount-1)
        {
            if(combo->currentIndex() != 0)
            {
                childColumnOffset++;
                widgetParent->setColumnCount(childColumnOffset + currentColumnCount);

                //std::vector<std::vector<int>> methodSetup;
                customObjectMethods.selectionType.clear();
                customObjectMethods.userSelection.clear();
                
                int ifCount = 0;
                int vecsize = 0;

                while(parentItem->childCount() != 0)
                {
                    vecsize = customObjectMethods.selectionType.size();
                    customObjectMethods.selectionType.resize(ifCount+1);
                    customObjectMethods.userSelection.resize(ifCount+1);
                    vecsize = customObjectMethods.selectionType.size();

                    QTreeWidgetItem *saveChild = parentItem->child(0);
                    int columnCount = saveChild->data(0,Qt::UserRole+2).toInt();
                    for(int i = 0;i<columnCount;i++)
                    {
                        int widgetType = saveChild->data(i,Qt::UserRole+1).toInt();
                        switch (widgetType) {
                        case 0:
                        {
                            QComboBox *methodBlockCombo = saveChild->data(i,Qt::UserRole).value<QComboBox*>();
                            if(methodBlockCombo)
                            {
                                int index = methodBlockCombo->currentIndex();
                                customObjectMethods.userSelection.at(ifCount).push_back((double)index);
                                customObjectMethods.selectionType.at(ifCount).push_back(0);
                                std::cout << "Caught method block at col: " << i << std::endl;
                            }
                            else
                            {
                                std::cout << "Missed method block at col: " << i << std::endl;
                            }

                        }
                            break;
                        case 1:
                        {
                            QLineEdit *staticVarLE = saveChild->data(i,Qt::UserRole).value<QLineEdit*>();

                            if(staticVarLE)
                            {
                                customObjectMethods.userSelection.at(ifCount).push_back((staticVarLE->text().toDouble()));
                                customObjectMethods.selectionType.at(ifCount).push_back(1);
                                std::cout << "Caught static var at col: " << i << std::endl;
                            }
                            else
                            {
                                std::cout << "Missed static var at col: " << i << std::endl;
                            }

                        }
                            break;
                        case 2:
                        {
                            QComboBox *timeCombo = saveChild->data(i,Qt::UserRole).value<QComboBox*>();

                            if(timeCombo)
                            {
                                customObjectMethods.userSelection.at(ifCount).push_back((double)timeCombo->currentIndex());
                                customObjectMethods.selectionType.at(ifCount).push_back(2);
                                std::cout << "Caught time combo at col: " << i << std::endl;
                            }
                            else
                            {
                                std::cout << "Missed time combo at col: " << i << std::endl;
                            }
                        }
                            break;
                        }
                    }

                    ifCount++;
                    parentItem->removeChild(saveChild);
                }

                addChildRow(widgetParent,parentItem,true,currentLayoutType,rootIndex);
                addChildRow(widgetParent,parentItem->child(childIndex),false,0,rootIndex);
                addChildRow(widgetParent,parentItem->child(parentItem->childCount()-1)->child(0),1);
                return;
            }
            else
            {
                childColumnOffset--;
                childItem->removeChild(childItem->child(0));
                addChildRow(widgetParent,parentItem,1);
            }

        }

        if(childColumnOffset > 0)
        {
            customObjectMethods.selectionType.clear();
            customObjectMethods.userSelection.clear();

            int ifCount = 0;
            int vecsize = 0;

            while(parentItem->childCount() != 0)
            {
                vecsize = customObjectMethods.selectionType.size();
                customObjectMethods.selectionType.resize(ifCount+1);
                customObjectMethods.userSelection.resize(ifCount+1);
                vecsize = customObjectMethods.selectionType.size();

                QTreeWidgetItem *saveChild = parentItem->child(0);
                int columnCount = saveChild->data(0,Qt::UserRole+2).toInt();
                for(int i = 0;i<columnCount;i++)
                {
                    int widgetType = saveChild->data(i,Qt::UserRole+1).toInt();
                    switch (widgetType) {
                    case 0:
                    {
                        QComboBox *methodBlockCombo = saveChild->data(i,Qt::UserRole).value<QComboBox*>();
                        if(methodBlockCombo)
                        {
                            int index = methodBlockCombo->currentIndex();
                            customObjectMethods.userSelection.at(ifCount).push_back((double)index);
                            customObjectMethods.selectionType.at(ifCount).push_back(0);
                            std::cout << "Caught method block at col: " << i << std::endl;
                        }
                        else
                        {
                            std::cout << "Missed method block at col: " << i << std::endl;
                        }

                    }
                        break;
                    case 1:
                    {
                        QLineEdit *staticVarLE = saveChild->data(i,Qt::UserRole).value<QLineEdit*>();

                        if(staticVarLE)
                        {
                            customObjectMethods.userSelection.at(ifCount).push_back((staticVarLE->text().toDouble()));
                            customObjectMethods.selectionType.at(ifCount).push_back(1);
                            std::cout << "Caught static var at col: " << i << std::endl;
                        }
                        else
                        {
                            std::cout << "Missed static var at col: " << i << std::endl;
                        }

                    }
                        break;
                    case 2:
                    {
                        QComboBox *timeCombo = saveChild->data(i,Qt::UserRole).value<QComboBox*>();

                        if(timeCombo)
                        {
                            customObjectMethods.userSelection.at(ifCount).push_back((double)timeCombo->currentIndex());
                            customObjectMethods.selectionType.at(ifCount).push_back(2);
                            std::cout << "Caught time combo at col: " << i << std::endl;
                        }
                        else
                        {
                            std::cout << "Missed time combo at col: " << i << std::endl;
                        }
                    }
                        break;
                    }
                }

                ifCount++;
                parentItem->removeChild(saveChild);
            }

            for(unsigned int i = 0;i<childColumnOffset;i++)
            {

            }

        }
        else
        {
            while(parentItem->childCount() != 0)//childColumnOffset)
            {
                parentItem->removeChild(parentItem->child(parentItem->childCount()-1));
            }

            switch (currentLayoutType) {
            case 0:
            {
                if(nCol == 1)
                {
                    addChildRow(widgetParent,parentItem,false,5,rootIndex);
                }
                else
                {
                    addChildRow(widgetParent,parentItem,false,2,rootIndex);
                }

            }
                break;
            case 1:
            {
                if(nCol == 1)
                {
                    addChildRow(widgetParent,parentItem,false,6,rootIndex);
                }
                else
                {
                    addChildRow(widgetParent,parentItem,false,4,rootIndex);
                }
            }
                break;
            case 2:
            {
                if(text == "Static Var" && nCol == 4)
                {
                    addChildRow(widgetParent,parentItem,false,0,rootIndex);
                }
                else if(nCol == 1 && text == "Static Var")
                {
                    addChildRow(widgetParent,parentItem,false,4,rootIndex);
                }
                else
                {
                    addChildRow(widgetParent,parentItem,false,6,rootIndex);
                }
            }
                break;
            case 3:
            {
                if(nCol == 3 && text == "Static Var")
                {
                    addChildRow(widgetParent,parentItem,false,1,rootIndex);
                }
                else if(nCol == 1 && text == "Static Var")
                {
                    addChildRow(widgetParent,parentItem,false,5,rootIndex);
                }
                else
                {
                    addChildRow(widgetParent,parentItem,false,6,rootIndex);
                }
            }
                break;
            case 4:
            {
                if(nCol == 4 && text == "Time Passed")
                {
                    addChildRow(widgetParent,parentItem,false,1,rootIndex);
                }
                else if(nCol == 1 && text == "Time Passed")
                {
                    addChildRow(widgetParent,parentItem,false,2,rootIndex);
                }
                else
                {
                    addChildRow(widgetParent,parentItem,false,6,rootIndex);
                }
            }
                break;
            case 5:
            {
                if(text == "Static Var")
                {
                    addChildRow(widgetParent,parentItem,false,0,rootIndex);
                }
                else if(text == "Time Passed" && nCol == 1)
                {
                    addChildRow(widgetParent,parentItem,false,0,rootIndex);
                }
                else if(text == "Time Passed" && nCol == 3)
                {
                    addChildRow(widgetParent,parentItem,false,7,rootIndex);
                }
                else
                {
                    addChildRow(widgetParent,parentItem,false,6,rootIndex);
                }
            }
                break;
            case 6:
            {
                if(nCol == 1 && text == "Time Passed")
                {
                    addChildRow(widgetParent,parentItem,false,2,rootIndex);
                }
                else if(nCol == 1 && text == "Static Var")
                {
                    addChildRow(widgetParent,parentItem,false,4,rootIndex);
                }
                else if(nCol == 3 && text == "Static Var")
                {
                    addChildRow(widgetParent,parentItem,false,5,rootIndex);
                }
                else
                {
                    addChildRow(widgetParent,parentItem,false,3,rootIndex);
                }
            }
                break;
            case 7:
            {
                if(nCol == 3)
                {
                    addChildRow(widgetParent,parentItem,false,6,rootIndex);
                }
                else if(text == "Static Var")
                {
                    addChildRow(widgetParent,parentItem,false,1,rootIndex);
                }
            }
                break;
            }

        }



//        int kidCount = 0;
//        if(childColumnOffset != 0)
//        {
//            //kidCount = parentItem->childCount();
//            addChildRow(widgetParent,parentItem,0);
//        }
//        else
//        {
//            //kidCount = parentItem->childCount();
//            addChildRow(widgetParent,childItem->child(childColumnOffset),0);
//        }
        if(childColumnOffset < 1)
        {
            childColumnOffset = 0;
        }

        //addChildRow(widgetParent,parentItem->child(childColumnOffset),0);
        //addChildRow(widgetParent,parentItem->child(childColumnOffset),0);
        widgetParent->setColumnCount(currentColumnCount+1 + childColumnOffset);
        for(int i = 0; i < widgetParent->columnCount();i++)
        {
            widgetParent->resizeColumnToContents(i);
        }

    }

}

void customMethodConstructorWindow::on_addMethodBtn_clicked()
{
    ui->newMethodBtn->setEnabled(true);

    ui->addMethodBtn->setEnabled(true);
}

#include "custommethodconstructorwindow.h"
#include "ui_custommethodconstructorwindow.h"
#include "QStackedWidget"
#include "QComboBox"
#include "QTextEdit"
#include "QLineEdit"
#include "QPushButton"


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

    QStringList rootName;
    //this is all done to then later populate the comboboxes
    //tags could be dynamically passed into the setupblock
    std::vector<QStringList> setup,reaction;

    setup.push_back(QString("If;While").split(";"));
    setup.push_back(QString("Time Passed;Static Var;Tag Var").split(";"));
    setup.push_back(QString("<;=;!=;>").split(";"));
    setup.push_back(QString("Time Passed;Static Var;Tag Var").split(";"));
    //setup.push_back(QString("Continue;And;Or").split(";"));

    reaction.push_back(QString("Action;Item;Static Var").split(";"));
    reaction.push_back(QString("End;Else").split(";"));
    reaction.push_back(QString("End;Break").split(";"));

    methodBlocks.push_back(setup);
    methodBlocks.push_back(reaction);

    rootName << "Method One";

    AddRoot(rootName);
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

void customMethodConstructorWindow::addChildRow(QTreeWidget *widgetParent, QTreeWidgetItem *itemParent, int cycleStart, int layoutType)
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


            QObject::connect(itemComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
            QVariant itemWidgetVar;
            itemWidgetVar.fromValue(itemComboBox);
            childItem->setData(cycleSetup,Qt::UserRole,itemWidgetVar);
            widgetType = 0;//this means its a method blocks type combobox
            childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
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
                widgetParent->setItemWidget(childItem,cycleSetup,itemLineEdit);
                itemParent->addChild(childItem);
                cycleSetup++;
            }

        }

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
            itemWidgetVar.fromValue(itemComboBox);
            childItem->setData(cycleSetup,Qt::UserRole,itemWidgetVar);
            widgetType = 0;//this means its a method blocks type combobox
            childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
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
            itemWidgetVar.fromValue(itemComboBox);
            childItem->setData(cycleSetup,Qt::UserRole,itemWidgetVar);
            widgetType = 0;//this means its a method blocks type combobox
            childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
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
        timeLineEditLocation = 2;
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
            itemWidgetVar.fromValue(itemComboBox);
            childItem->setData(cycleSetup,Qt::UserRole,itemWidgetVar);
            widgetType = 0;//this means its a method blocks type combobox
            childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
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
            itemWidgetVar.fromValue(itemComboBox);
            childItem->setData(cycleSetup,Qt::UserRole,itemWidgetVar);
            widgetType = 0;//this means its a method blocks type combobox
            childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
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
            itemWidgetVar.fromValue(itemComboBox);
            childItem->setData(cycleSetup,Qt::UserRole,itemWidgetVar);
            widgetType = 0;//this means its a method blocks type combobox
            childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
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
            itemWidgetVar.fromValue(itemComboBox);
            childItem->setData(cycleSetup,Qt::UserRole,itemWidgetVar);
            widgetType = 0;//this means its a method blocks type combobox
            childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
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
            itemWidgetVar.fromValue(itemComboBox);
            childItem->setData(cycleSetup,Qt::UserRole,itemWidgetVar);
            widgetType = 0;//this means its a method blocks type combobox
            childItem->setData(cycleSetup,Qt::UserRole+1,QVariant::fromValue(widgetType));
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
                widgetParent->setItemWidget(childItem,cycleSetup,timeItemCombobox);
                itemParent->addChild(childItem);
                cycleSetup++;
            }

        }

    }
        break;
    }

    childItem->setData(0,Qt::UserRole+2,QVariant::fromValue(cycleSize));
    childItem->setData(0,Qt::UserRole+3,layoutType);//indicates layout type

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
    QObject::connect(item0ComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
    widgetParent->setItemWidget(childItem,0,item0ComboBox);
    itemParent->addChild(childItem);

    QComboBox *item1ComboBox = new QComboBox;
    item1ComboBox->setProperty("rowType", rowType);
    item1ComboBox->setProperty("row", 1);
    item1ComboBox->setProperty("column", 1);
    item1ComboBox->addItems(methodBlocks.at(1).at(rowType));
    QObject::connect(item1ComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
    widgetParent->setItemWidget(childItem,1,item1ComboBox);
    itemParent->addChild(childItem);

}

void customMethodConstructorWindow::AddRoot(QStringList rootNames)
{
    QTreeWidgetItem *rootItem = new QTreeWidgetItem(ui->methodSetupTreeWidget);

    for(int i = 0;i< rootNames.size();i++)
    {
        rootItem->setText(i,rootNames.at(i));
    }
    ui->methodSetupTreeWidget->addTopLevelItem(rootItem);

    addChildRow(ui->methodSetupTreeWidget,rootItem,0,0);

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
        QTreeWidgetItem *parentItem = childItem->parent();
        QTreeWidget* widgetParent = parentItem->treeWidget();

        int currentColumnCount = childItem->data(0,Qt::UserRole+2).toInt();

        int currentLayoutType = childItem->data(0,Qt::UserRole+3).toInt();
        //note that the number of columns is offset by one with reference to the nCol

        if(nCol == currentColumnCount-1)
        {
            if(combo->currentIndex() != 0)
            {
                childColumnOffset++;
                //childItem->removeChild(childItem->child(0));
                widgetParent->setColumnCount(childColumnOffset + currentColumnCount);
                addChildRow(widgetParent,parentItem,2,0);
                //addChildRow(widgetParent,childItem->child(0),1);
                return;
            }
            else
            {
                childColumnOffset--;
                childItem->removeChild(childItem->child(0));
                addChildRow(widgetParent,parentItem,1);
            }

        }
//        while(childItem->childCount() != 0)
//        {
//            childItem->removeChild(childItem->child(0));
//        }

        while(parentItem->childCount() != childColumnOffset)//childColumnOffset)
        {
            parentItem->removeChild(parentItem->child(childColumnOffset));
        }


        //this
        switch (currentLayoutType) {
        case 0:
        {
            if(nCol == 1)
            {
                addChildRow(widgetParent,parentItem,2,5);
            }
            else
            {
                addChildRow(widgetParent,parentItem,5,2);
            }

        }
            break;
        case 1:
        {
            if(nCol == 1)
            {
                addChildRow(widgetParent,parentItem,5,6);
            }
            else
            {
                addChildRow(widgetParent,parentItem,5,4);
            }
        }
            break;
        case 2:
        {
            if(text == "Static Var" && nCol == 4)
            {
                addChildRow(widgetParent,parentItem,5,0);
            }
            else if(nCol == 1 && text == "Static Var")
            {
                addChildRow(widgetParent,parentItem,5,4);
            }
            else
            {
                addChildRow(widgetParent,parentItem,5,6);
            }
        }
            break;
        case 3:
        {
            if(nCol == 3 && text == "Static Var")
            {
                addChildRow(widgetParent,parentItem,5,1);
            }
            else if(nCol == 1 && text == "Static Var")
            {
                addChildRow(widgetParent,parentItem,5,5);
            }
            else
            {
                addChildRow(widgetParent,parentItem,5,6);
            }
        }
            break;
        case 4:
        {
            if(nCol == 4 && text == "Time Passed")
            {
                addChildRow(widgetParent,parentItem,5,1);
            }
            else
            {
                addChildRow(widgetParent,parentItem,5,2);
            }
        }
            break;
        case 5:
        {
            if(text == "Static Var")
            {
                addChildRow(widgetParent,parentItem,5,0);
            }
            else if(text == "Time Passed" && nCol == 1)
            {
                addChildRow(widgetParent,parentItem,5,0);
            }
            else if(text == "Time Passed" && nCol == 3)
            {
                addChildRow(widgetParent,parentItem,5,7);
            }
            else
            {
                addChildRow(widgetParent,parentItem,5,6);
            }
        }
            break;
        case 6:
        {
            if(nCol == 1 && text == "Time Passed")
            {
                addChildRow(widgetParent,parentItem,5,2);
            }
            else if(nCol == 1 && text == "Static Var")
            {
                addChildRow(widgetParent,parentItem,5,4);
            }
            else if(nCol == 3 && text == "Static Var")
            {
                addChildRow(widgetParent,parentItem,5,5);
            }
            else
            {
                addChildRow(widgetParent,parentItem,5,3);
            }
        }
            break;
        case 7:
        {
            if(nCol == 3)
            {
                addChildRow(widgetParent,parentItem,5,6);
            }
            else if(text == "Static Var")
            {
                addChildRow(widgetParent,parentItem,5,1);
            }
        }
            break;
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
        addChildRow(widgetParent,parentItem->child(childColumnOffset),0);
        widgetParent->setColumnCount(childColumnOffset + currentColumnCount);
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

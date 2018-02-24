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
    ui->methodSetupTreeWidget->setColumnCount(8);

    QStringList rootName;
    //this is all done to then later populate the comboboxes
    //tags could be dynamically passed into the setupblock
    std::vector<QStringList> setup,reaction;

    setup.push_back(QString("If;While").split(";"));
    setup.push_back(QString("Tag;Time Passed;Static Var").split(";"));
    setup.push_back(QString("<;=;!=;>").split(";"));
    setup.push_back(QString("Tag;Time Passed;Static Var").split(";"));
    setup.push_back(QString("Continue;And;Or").split(";"));

    reaction.push_back(QString("Action;Item;Static Var").split(";"));
    reaction.push_back(QString("End;Else").split(";"));
    reaction.push_back(QString("End;Break").split(";"));

    methodBlocks.push_back(setup);
    methodBlocks.push_back(reaction);

    rootName << "Method One";

    AddRoot(rootName);
}

customMethodConstructorWindow::~customMethodConstructorWindow()
{
    delete ui;
}

void customMethodConstructorWindow::addChildRow(QTreeWidget *widgetParent,QTreeWidgetItem *itemParent,int rowType)
{
    //widgetParent->setColumnCount(methodBlocks.at(rowType).size());
    if(rowType == 0)
    {
        QTreeWidgetItem *childItem = new QTreeWidgetItem(itemParent);



        QVariant childItemVariant,widgetParentVarient;
        widgetParentVarient.setValue(widgetParent);
        childItemVariant.setValue(childItem);
        uint cycleSetup;
        for(cycleSetup = 0;cycleSetup < methodBlocks.at(rowType).size()+2;cycleSetup++)
        {
            if(cycleSetup < methodBlocks.at(rowType).size())
            {
                QComboBox *itemComboBox = new QComboBox;
                itemComboBox->setProperty("rowType", rowType);
                itemComboBox->setProperty("row", 0);
                itemComboBox->setProperty("column",cycleSetup);
                itemComboBox->setProperty("widgetParent",widgetParentVarient);
                itemComboBox->setProperty("childItem",childItemVariant);
                itemComboBox->addItems(methodBlocks.at(0).at(cycleSetup));
                QObject::connect(itemComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
                QLineEdit *itemLineEdit = new QLineEdit;
                QPushButton *itemButton = new QPushButton;
                itemButton->setText("Reset");
                QComboBox *timeComboBox = new QComboBox;
                timeComboBox->setProperty("rowType", rowType);
                timeComboBox->setProperty("row", 0);
                timeComboBox->setProperty("column",cycleSetup);
                timeComboBox->setProperty("widgetParent",widgetParentVarient);
                timeComboBox->setProperty("childItem",childItemVariant);
                timeComboBox->addItems(QString("Seconds;MilliSeconds;Reset").split(";"));
                QFrame *stackFrame = new QFrame;
                QStackedWidget *masterItemWidget = new QStackedWidget(stackFrame);
                masterItemWidget->addWidget(itemLineEdit);
                masterItemWidget->addWidget(itemComboBox);
                masterItemWidget->addWidget(itemButton);
                masterItemWidget->addWidget(timeComboBox);
                masterItemWidget->setCurrentIndex(1);
                QVariant stackParent;
                stackParent.setValue(masterItemWidget);
                itemComboBox->setProperty("stackParent",stackParent);
                itemComboBox->setProperty("cycleSetupIT",cycleSetup);
                QVariant frameVariant;
                frameVariant.setValue(stackFrame);
                childItem->setData(cycleSetup,Qt::UserRole,stackParent);
                //stackWidgetList.push_back(stackParent);
                widgetParent->setItemWidget(childItem,cycleSetup,masterItemWidget);
                itemParent->addChild(childItem);
            }
            else
            {
//                QComboBox *itemComboBox = new QComboBox;
//                itemComboBox->setProperty("rowType", rowType);
//                itemComboBox->setProperty("row", 0);
//                itemComboBox->setProperty("column",cycleSetup);
//                itemComboBox->setProperty("widgetParent",widgetParentVarient);
//                //itemComboBox->setProperty("itemParent",itemParentVariant);
//                itemComboBox->addItems(methodBlocks.at(0).at(methodBlocks.at(rowType).size() - 1));
//                QObject::connect(itemComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
//                QLineEdit *itemLineEdit = new QLineEdit;
//                QPushButton *itemButton = new QPushButton;
//                itemButton->setText("Reset");
//                QStackedWidget *masterItemWidget = new QStackedWidget;
//                masterItemWidget->addWidget(itemLineEdit);
//                masterItemWidget->addWidget(itemComboBox);
//                masterItemWidget->addWidget(itemButton);
//                masterItemWidget->setCurrentIndex(1);
//                masterItemWidget->setVisible(false);
//                QVariant stackParent;
//                stackParent.setValue(masterItemWidget);
//                itemComboBox->setProperty("stackParent",stackParent);
//                widgetParent->setItemWidget(childItem,cycleSetup,masterItemWidget);
//                widgetParent->itemWidget(childItem,cycleSetup)->setVisible(false);
//                itemParent->addChild(childItem);
            }

//            QVariant checkVar;
//            for(int i = 0; i < methodBlocks.at(rowType).size();i++)
//            {
//                checkVar= childItem->data(i,Qt::UserRole);
//            }

            //childItem->setData(0,Qt::UserRole,stackWidgetList);
        }



    }
    else
    {
        QTreeWidgetItem *childItem = new QTreeWidgetItem(itemParent);

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


}

void customMethodConstructorWindow::AddRoot(QStringList rootNames)
{
    QTreeWidgetItem *rootItem = new QTreeWidgetItem(ui->methodSetupTreeWidget);

    for(int i = 0;i< rootNames.size();i++)
    {
        rootItem->setText(i,rootNames.at(i));
    }
    ui->methodSetupTreeWidget->addTopLevelItem(rootItem);


    addChildRow(ui->methodSetupTreeWidget,rootItem,0);

    //addChildRow(ui->methodSetupTreeWidget,ui->methodSetupTreeWidget->itemAt(QPoint(1,0)),1);
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

void customMethodConstructorWindow::OnComboIndexChanged(const QString& text)
{
    QComboBox* combo = qobject_cast<QComboBox*>(sender());
    if (combo)
    {

        int nRow = combo->property("row").toInt();
        int nCol = combo->property("column").toInt();
        switch (nRow) {
        case 0:
        {
            switch (nCol)
            {
            case 0:
            {
                //combo->setVisible(false);
                //testPoint = combo->pos();

//                if(nRow < customObjectAttributes.inputType.size())
//                {
//                    customObjectAttributes.inputType.push_back(combo->currentIndex());
//                }
//                else
//                {
//                    customObjectAttributes.inputType.at(nRow) =combo->currentIndex();
//                }
            }
                break;
            case 1:
            {
                switch (combo->currentIndex()) {
                case 0:
                {

                }
                    break;
                case 1:
                {

                }
                    break;
                case 2:
                {
                    //combo->setEditable(true);
                    QStackedWidget *itemMaster = combo->property("stackParent").value<QStackedWidget*>();
                    itemMaster->setCurrentIndex(0);
                    QTreeWidget *widgetParent = combo->property("widgetParent").value<QTreeWidget*>();

                    //QTreeWidgetItem *parentItem = combo->property("itemParent").value<QTreeWidgetItem*>();
                    QTreeWidgetItem *childItem = combo->property("childItem").value<QTreeWidgetItem*>();//new QTreeWidgetItem(parentItem);
                    QTreeWidgetItem *parentItem = childItem->parent();

                    //QTreeWidgetItem *current = childItem;
                    //QTreeWidgetItem *parent = current->parent();
                    QTreeWidgetItem *nextSibling;
                    if(parentItem){
                      nextSibling =parentItem->child(parentItem->indexOfChild(childItem)+1);
                    }
                    else {
                      QTreeWidget *treeWidget = childItem->treeWidget();
                      nextSibling = treeWidget->topLevelItem(treeWidget->indexOfTopLevelItem(childItem)+1);
                    }
                    int childInd = parentItem->indexOfChild(childItem);
                    int sibInd = parentItem->indexOfChild(nextSibling);
                    int kidCount = childItem->childCount();

//                    QVariant checkVar;
//                    for(int i = 0; i < methodBlocks.at(0).size();i++)
//                    {
//                        checkVar= childItem->data(i,Qt::UserRole);
//                    }
                    //widgetParent->setpr
                    //QFrame *frameObject = childItem->data(2,Qt::UserRole).value<QFrame*>();
                    //QList<QVariant> stackList = childItem->data(0,Qt::UserRole).value<QList<QVariant>>();
                    int cycleIT = combo->property("cycleSetupIT").toInt();
                    QStackedWidget *itemFrameMaster = childItem->data(cycleIT+1,Qt::UserRole).value<QStackedWidget*>();//frameObject->findChild<QStackedWidget*>();
                    if(itemFrameMaster)
                    {
                        itemFrameMaster->setCurrentIndex(3);
                        qDebug() << "itemFrame Exists";

                    }
                    else
                    {
                        qDebug() << "itemFrame is NULL";
                    }
                    //QObjectList objList = widgetParent->children();
                    //QObject itemObject = qobject_cast<QObject*>();
                    //widgetParent->children().at(0)->children().at(2)->
//                    QComboBox *itemComboBox = qobject_cast<QComboBox*>(widgetParent->itemWidget(childItem,childInd));
//                    itemComboBox->setProperty("rowType", widgetParent->itemWidget(childItem,combo->property("column").toInt())->property("rowType").toInt());
//                    itemComboBox->setProperty("row", 0);
//                    itemComboBox->setProperty("column",cycleSetup);
//                    itemComboBox->setProperty("widgetParent",widgetParentVarient);
//                    //itemComboBox->setProperty("itemParent",itemParentVariant);
//                    itemComboBox->addItems(methodBlocks.at(0).at(cycleSetup));

//                    QStackedWidget *itemTimerMaster = itemComboBox->property("stackParent").value<QStackedWidget*>();

//                    QComboBox *timerItemCB = qobject_cast<QComboBox*>(itemTimerMaster->widget(1));
//                    timerItemCB->clear();
//                    timerItemCB->addItems(QString("Seconds;MilliSeconds;Reset").split(";"));
                    //FUCK I NEED TO MAKE A WHOLE FUCKING SERIES OF SLOTS HERE

//                    widgetParent->setItemWidget(childItem,combo->property("column").toInt()+1,timerItemCB);
//                    //parentItem->insertChild(combo->property("column").toInt(),childItem);
//                    QLineEdit *itemStaticVarLineEdit = new QLineEdit;
//                    widgetParent->setItemWidget(childItem,combo->property("column").toInt()+3,itemStaticVarLineEdit);
//                    //parentItem->insertChild(combo->property("column").toInt()+3,childItem);
//                    QPushButton *itemPushButon = new QPushButton;
//                    itemPushButon->setText("Reset");
//                    widgetParent->setItemWidget(childItem,combo->property("column").toInt()+4,itemPushButon);
//                    //parentItem->insertChild(combo->property("column").toInt()+4,childItem);

//
                }
                    break;
                }

//                if(combo->currentIndex() != 0)
//                {
//                    int index = parentItem->indexOfChild(childItem);
//                    QString testthing = parentItem->whatsThis(index);
//                    QTextEdit *textItemEdit = new QTextEdit;
//                    QObject::connect(item1ComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnComboIndexChanged(const QString&)));
//                    ui->methodSetupTreeWidget->setItemWidget(childItem,2,textItemEdit);
//                }
//                if(nRow < customObjectAttributes.valueA.size())
//                {
//                    customObjectAttributes.valueA.push_back(combo->currentIndex());
//                }
//                else
//                {
//                    customObjectAttributes.valueA.at(nRow) = combo->currentIndex();
//                }
            }
                break;
            case 2:
                //customObjectAttributes.comparisonType.push_back(combo->currentIndex());
                break;
            case 3:
            {
                //customObjectAttributes.valueB.push_back(combo->currentIndex());
            }
                break;
            case 4:

                break;
            case 5:
                break;
            case 6:
                break;
            case 7:
                //ui->logicSetupTableWidget->setRowCount(ui->logicSetupTableWidget->rowCount()+1);
                if(combo->currentIndex() != 3)
                {
                    //addComboRow(ui->logicSetupTableWidget->rowCount()-1);
                }
                //customObjectAttributes.outputType.push_back(combo->currentIndex());
                break;
            }
        }
            break;
        case 1:
        {
            switch (combo->property("column").toInt())
            {
            case 0:
//                if(nRow < customObjectAttributes.inputType.size())
//                {
//                    customObjectAttributes.inputType.push_back(combo->currentIndex());
//                }
//                else
//                {
//                    customObjectAttributes.inputType.at(nRow) =combo->currentIndex();
//                }
                break;
            case 1:
            {
                addChildRow(ui->methodSetupTreeWidget,ui->methodSetupTreeWidget->itemAt(QPoint(2,1)),0);
                addChildRow(ui->methodSetupTreeWidget,ui->methodSetupTreeWidget->itemAt(QPoint(3,1)),1);
//                if(nRow < customObjectAttributes.valueA.size())
//                {
//                    customObjectAttributes.valueA.push_back(combo->currentIndex());
//                }
//                else
//                {
//                    customObjectAttributes.valueA.at(nRow) = combo->currentIndex();
//                }
            }
                break;
            case 2:
            {
                //customObjectAttributes.comparisonType.push_back(combo->currentIndex());
            }
                break;
            case 3:
            {
                //customObjectAttributes.valueB.push_back(combo->currentIndex());
            }
                break;
            case 4:
                break;
            case 5:
                break;
            case 6:
                break;
            case 7:
            {
                //ui->logicSetupTableWidget->setRowCount(ui->logicSetupTableWidget->rowCount()+1);
                if(combo->currentIndex() != 3)
                {
                    //addComboRow(ui->logicSetupTableWidget->rowCount()-1);
                }
                //customObjectAttributes.outputType.push_back(combo->currentIndex());
            }
                break;
            }
        }
            break;
        }


    }

}


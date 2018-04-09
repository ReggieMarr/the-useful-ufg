/*
 * Copyright © 2001-2011 Reginald Marr <reginald.t.marr@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 *
 */
#include "controlobjectitemdelegate.h"
#include "QAbstractItemDelegate"
#include "QAbstractItemModel"
//#include "treemodel.h"

controlObjectItemDelegate::controlObjectItemDelegate(QObject *parent) :
    QItemDelegate(parent)
{

}

QWidget* controlObjectItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //TreeModel.index(index.row(),index.column(),parent).
    //TreeModel(const QStringList &headers, const QString &data,
    //          QObject *parent = 0);
//    QStringList headers;
//    headers << tr("Title") << tr("Description");

//    TreeModel *editConstructorItem = new TreeModel(headers);
//    int rowCount = editConstructorItem->rowCount(index);


    if(!index.parent().isValid())
    {

        switch (index.column()) {
        case 0:
        {
            QComboBox *cellComboBox = new QComboBox(parent);
            cellComboBox->addItems(QString("If;While").split(";"));
            connect(cellComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onComboBoxChanged()));
            return cellComboBox;
        }
            break;
        case 1:
        {
            QComboBox *cellComboBox = new QComboBox(parent);
            cellComboBox->addItems(QString("Tag;Time;Static Var").split(";"));
            connect(cellComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onComboBoxChanged()));
            return cellComboBox;
        }
            break;
        case 2:
        {
            QComboBox *cellComboBox = new QComboBox(parent);
            cellComboBox->addItems(QString("<;=;>").split(";"));
            connect(cellComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onComboBoxChanged()));
            return cellComboBox;
        }
            break;
        case 3:
        {
            QComboBox *cellComboBox = new QComboBox(parent);
            cellComboBox->addItems(QString("Tag;Time;Static Var").split(";"));
            connect(cellComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onComboBoxChanged()));
            return cellComboBox;
        }
            break;

        }

    }
//    else if(rowCount == 2)
//    {
//        switch (index.column()) {
//        case 0:
//        {
//            QComboBox *cellComboBox = new QComboBox(parent);
//            cellComboBox->addItems(QString("Use;Execute").split(";"));
//            return cellComboBox;
//        }
//            break;
//        case 1:
//        {
//            QComboBox *cellComboBox = new QComboBox(parent);
//            cellComboBox->addItems(QString("Function;Control Type;Action").split(";"));
//            return cellComboBox;
//        }
//            break;
//        case 2:
//        {
//            QComboBox *cellComboBox = new QComboBox(parent);
//            cellComboBox->addItems(QString("End;Else").split(";"));
//            return cellComboBox;
//        }
//            break;

//        }
//    }


}

void controlObjectItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    //what i did
//    int value = index.model()->data(index,Qt::EditRole).toInt();
//    QComboBox *cellComboBox = static_cast<QComboBox*>(editor);
//    cellComboBox->setCurrentIndex(value);

    //what stackoverflow told me to do
    QString value = index.model()->data(index,Qt::EditRole).toString();
    QComboBox *cellComboBox = static_cast<QComboBox*>(editor);
    cellComboBox->setCurrentText(value);
}

void controlObjectItemDelegate::onComboBoxChanged()
{
    emit commitData(static_cast<QWidget*>(sender()));
}

void controlObjectItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{

    QComboBox *cellComboBox = static_cast<QComboBox*>(editor);
    model->setData(index, cellComboBox->currentText(), Qt::EditRole);
    // save the index of the QComboBox in the role Qt::UserRole + 1
    model->setData(index, cellComboBox->currentIndex(), Qt::UserRole+1);

}

void controlObjectItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}

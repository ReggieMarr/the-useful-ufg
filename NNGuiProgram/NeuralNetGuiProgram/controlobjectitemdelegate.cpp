#include "controlobjectitemdelegate.h"
#include "QAbstractItemDelegate"
#include "QAbstractItemModel"


controlObjectItemDelegate::controlObjectItemDelegate(QObject *parent) :
    QItemDelegate(parent)
{

}

QWidget* controlObjectItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QComboBox *cellComboBox = new QComboBox(parent);
    cellComboBox->addItems(QString("Test 1;Test 2;Test 3").split(";"));
    return cellComboBox;
}

void controlObjectItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{

    int value = index.model()->data(index,Qt::EditRole).toInt();
    QComboBox *cellComboBox = static_cast<QComboBox*>(editor);
    cellComboBox->setCurrentIndex(value);
}

void controlObjectItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *cellComboBox = static_cast<QComboBox*>(editor);
    QStringList value;
    for(int i=0;i<cellComboBox->count();++i)
    {
        value.append(cellComboBox->itemText(i));
    }


    model->setData(index, cellComboBox->currentText(), Qt::EditRole);
}

void controlObjectItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}

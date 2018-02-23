#ifndef CONTROLOBJECTITEMDELEGATE_H
#define CONTROLOBJECTITEMDELEGATE_H

#include <QObject>
#include <QItemDelegate>
#include <QModelIndex>
#include <QSize>
#include <QComboBox>
#include <QStandardItemModel>
#include <string>
#include <vector>

class QModelIndex;
class QWidget;
class QVariant;

class controlObjectItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit controlObjectItemDelegate(QObject *parent = 0);
    //the view calls this to create the delegate
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    //this allows the data to be edited in a cell
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    //this updates the model to be viewed
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    //how an item knows where it needs to be and how big it should be
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private slots:
    void onComboBoxChanged();

private:
    std::vector<std::string> Items;
};

#endif // CONTROLOBJECTITEMDELEGATE_H

#ifndef NNTOTAGSLINKAGEWINDOW_H
#define NNTOTAGSLINKAGEWINDOW_H

#include <QDialog>

namespace Ui {
class NNtoTagsLinkageWindow;
}

class NNtoTagsLinkageWindow : public QDialog
{
    Q_OBJECT

public:
    explicit NNtoTagsLinkageWindow(QWidget *parent = 0);
    ~NNtoTagsLinkageWindow();

private:
    Ui::NNtoTagsLinkageWindow *ui;
};

#endif // NNTOTAGSLINKAGEWINDOW_H

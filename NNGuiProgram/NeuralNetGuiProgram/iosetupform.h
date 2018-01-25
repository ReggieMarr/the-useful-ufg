#ifndef IOSETUPFORM_H
#define IOSETUPFORM_H

#include <QWidget>

namespace Ui {
class IOSetupForm;
}

class IOSetupForm : public QWidget
{
    Q_OBJECT

public:
    explicit IOSetupForm(QWidget *parent = 0);
    ~IOSetupForm();

private:
    Ui::IOSetupForm *ui;
};

#endif // IOSETUPFORM_H

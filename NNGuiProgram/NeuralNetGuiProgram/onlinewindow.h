#ifndef ONLINEWINDOW_H
#define ONLINEWINDOW_H

#include <QMainWindow>

namespace Ui {
class OnlineWindow;
}

class OnlineWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit OnlineWindow(QWidget *parent = 0);
    ~OnlineWindow();

private:
    Ui::OnlineWindow *ui;
};

#endif // ONLINEWINDOW_H

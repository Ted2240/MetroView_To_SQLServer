#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>

namespace Ui
{
    class LoginWindow;
}

class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

signals:
    void close_window(); //关闭登录界面信号

private slots:
    void on_LoginButton_clicked();

    void read_json(void);

private:
    Ui::LoginWindow *ui;
};

#endif // LOGINWINDOW_H

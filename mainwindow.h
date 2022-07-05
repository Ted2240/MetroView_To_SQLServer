#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "SQL.h"
#include <QJsonDocument>
#include <QFile>
#include <QDebug>
#include <QJsonObject>
#include <QByteArray>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QDebug>
#include <QComboBox>
#include <QWidget>
#include <QThread>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QByteArray>
#include <QTimer>
#include <QDateTime>
#include <QSystemTrayIcon>
#include <QObject>
#include <QLibrary>

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>

#include "loginwindow.h"
#include "adddialog.h"
#include "Logger.h"
#include "logset.h"
#include "about.h"
#include "loading.h"
#include "function.h"
#include <map>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class WorkThread : public QThread
{
    Q_OBJECT
public:
    WorkThread();

private:
protected:
    void run();
};

class HisThread : public QThread
{
    Q_OBJECT
public:
    HisThread();

private:
protected:
    void run();
};

class LoadThread : public QThread
{
    Q_OBJECT
public:
    LoadThread();

signals:
    void load_complete();
    void load_success();

private:
protected:
    void run();
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void changeEvent(QEvent *event);
    void closeEvent(QCloseEvent *e);

private slots:
    void on_actionLoad_triggered();

    void on_actionAdd_triggered();

    void on_actionSave_triggered();

    void on_actionExit_triggered();

    void fresh_Table();

    void on_actionmin_triggered();

    void on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);

    void on_actionopition_triggered();

    void on_actionabout_triggered();

    void on_actionorcle_triggered();

private:
    Ui::MainWindow *ui;
    LoginWindow *m_login;
    WorkThread *thread1;
    HisThread *thread2;
    LoadThread *thread3;
    QSystemTrayIcon *mSysTrayIcon;
    LogSet *m_log;
    About *m_about;
    Loading* m_load;
};

extern int level_log;
extern QString SQL_ip;
extern QString SQL_username;
extern QString SQL_passwd;
extern QString SQL_dbname;

#endif // MAINWINDOW_H

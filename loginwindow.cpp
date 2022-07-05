#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "mainwindow.h"
#include "SQL.h"
#include <QJsonDocument>
#include <QFile>
#include <QDebug>
#include <QJsonObject>
#include <QByteArray>

extern SQL SQLL;

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("登录")); //设置对话框的标题
    read_json();
    connect(this, SIGNAL(close_window()), this, SLOT(close()));
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_LoginButton_clicked()
{
    QString IP = ui->IPEdit->text();
    QString User = ui->UserEdit->text();
    QString Password = ui->PasswordEdit->text();
    QString Table = ui->TableEdit->text();
    if (SQLL.openSQLServer(IP, User, Password,Table))
    {
        QFile file(QApplication::applicationDirPath() + "/Login.json");

        if (!file.open(QIODevice::WriteOnly))
        {
            if (level_log > 0)
            {
                qCritical() << "Login.json文件打开失败";
            }
        }
        else
        {
            QJsonObject obj;
            obj["SAVE_FLAG"] = "1";
            obj["SAVE_IP"] = ui->IPEdit->text();
            obj["SAVE_USERNAME"] = ui->UserEdit->text();
            obj["SAVE_PASSWORD"] = ui->PasswordEdit->text();
            obj["SAVE_TABLE"] = ui->TableEdit->text();

            QJsonDocument jdoc(obj);
            file.write(jdoc.toJson());
            file.flush();

            // this->close();
            emit(close_window());
        }
    }
}

void LoginWindow::read_json(void)
{

    QFile file(QApplication::applicationDirPath() + "/Login.json");
    if (!file.open(QIODevice::ReadOnly))
    {
        if (level_log > 0)
        {
            qCritical() << "Login.json文件打开失败";
        }
    }
    else
    {
        QJsonDocument jdc(QJsonDocument::fromJson(file.readAll()));
        QJsonObject obj = jdc.object();
        if (obj.value("SAVE_FLAG").toString() == "1")
        {
            // qDebug() << "File open successfully!";
            QString save_name = obj.value("SAVE_IP").toString();
            QString save_username = obj.value("SAVE_USERNAME").toString();
            QString save_password = obj.value("SAVE_PASSWORD").toString();
            QString save_table = obj.value("SAVE_TABLE").toString();
            ui->IPEdit->setText(save_name);
            ui->PasswordEdit->setText(save_password);
            ui->UserEdit->setText(save_username);
            ui->TableEdit->setText(save_table);
            ui->checkBox->setChecked(true);
        }
    }
    file.close();
}

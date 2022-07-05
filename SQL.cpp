#include "SQL.h"
#include "QDateTime"

QString SQL_ip;
QString SQL_username;
QString SQL_passwd;
QString SQL_dbname;

QString getReqTime()
{
    QDateTime curDateTime = QDateTime::currentDateTime();
    return curDateTime.toString("yyyy-MM-dd hh:mm:ss");
}

QString getReqTime(int time_t)
{
    QDateTime curDateTime = QDateTime::fromSecsSinceEpoch(time_t);
    return curDateTime.toString("yyyy-MM-dd hh:mm:ss");
}

bool SQL::openSQLServer(const QString ip, const QString userName, const QString passwd,const QString table)
{
    if (ip.isEmpty() || userName.isEmpty() || passwd.isEmpty())
    {
        QMessageBox::critical(0, QObject::tr("Database error"), "参数不允许为空");
        return false;
    }

    {

        QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
        QString dsn = QString("DRIVER={SQL SERVER};SERVER=%1;DATABASE=%2").arg(ip).arg(table);
        db.setDatabaseName(dsn);
        db.setUserName(userName);
        db.setPassword(passwd);


        if (!db.open())
        {

            qDebug() << db.lastError().text();
            QMessageBox::critical(0, QObject::tr("Database error"), db.lastError().text());
            qInfo() << "登录SQLServer失败！";
            return false;
        }
        else
        {
            SQL_ip = ip;
            SQL_passwd = passwd;
            SQL_username = userName;
            SQL_dbname = table;
            // qDebug()<<"database open success!";
            QMessageBox::information(0, QObject::tr("Database ok"), "数据库打开成功");
            qInfo() << "登录SQLServer成功！";
        }
    }
    QSqlDatabase::removeDatabase("qt_sql_default_connection");

    return true;
}


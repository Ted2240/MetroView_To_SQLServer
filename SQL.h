#ifndef SQL_H
#define SQL_H

#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSql>
#include <QDialog>
#include <QDebug>
#include <QMessageBox>
#include <QDateTime>
#include <QMutex>

class SQL
{
private:
public:
    bool openSQLServer(const QString ip, const QString userName, const QString passwd, const QString table);

};

QString getReqTime();
QString getReqTime(int time_t);

extern int level_log;
extern QString SQL_ip;
extern QString SQL_username;
extern QString SQL_passwd;
extern QString SQL_dbname;

#endif // SQL_H

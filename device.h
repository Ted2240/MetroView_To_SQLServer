#ifndef DEVICE_H
#define DEVICE_H

#include <iostream>
#include <string>
#include <list>
#include <map>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSql>
#include <QMessageBox>
#include <QMutex>

#include "function.h"

typedef struct MetroView
{
    QString Tag;
    QString Device_num;
} DEVICE;

class device
{
public:
    device();
    ~device();
    bool connectDB(QString path, QString devicename, QString pointname);
    bool setSQLTable(QString tablename, QString listname, QString sqlnumber,QString sqlquery);

private:

public:
    map<QString, DEVICE> tag_list;
    QString device_name;
    QString point_name;
    QString sql_table;
    QString sql_list;
    QString my_path;
    QString sql_number;
    QString sql_query = "";

    // map<string, DEVICE>::iterator  itlist;
};

extern int level_log;
extern list<device> device_list;

#endif // DEVICE_H

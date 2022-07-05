#include "device.h"
#include "SQL.h"

extern SQL SQLL;

device::device()
{
}

device::~device()
{
}

bool device::connectDB(QString path, QString devicename, QString pointname)
{
    my_path = path;
    device_name = devicename;
    point_name = pointname;

    {

        QSqlDatabase mydb = QSqlDatabase::addDatabase("QMYSQL","device");
        mydb.setHostName(my_path);
        mydb.setUserName("sysadmin");
        mydb.setPassword("adminsys");
        mydb.setDatabaseName("idpora");

        if (mydb.open() == false)
        {
            QMessageBox::critical(0, "MYSQL连接错误" + devicename + "." + pointname, mydb.lastError().text());
            if (level_log > 0)
            {
                qCritical() << "[MYSQL连接错误]" << devicename + "." + pointname << "   " << mydb.lastError().text();
            }

            return false;
        }
        else
        {
            QSqlQuery query(mydb);
            QString sqls = "SELECT NAME,DEVICENO FROM cfg_view_point_protocol WHERE DeviceTypeName ='" + device_name + "' AND PointName = '" + point_name + "'";
            if (level_log > 2)
            {
                qDebug() << "[MYSQL查询语句]"
                         << "   " << sqls;
            }
            query.exec(sqls);
            tag_list.clear();
            while (true == query.next())
            {
                DEVICE temp;
                temp.Tag = query.value(0).toString();
                temp.Device_num = query.value(1).toString();
                tag_list.insert(pair<QString, DEVICE>(temp.Tag, temp));
                if (level_log > 3)
                {
                    qDebug() << "[获取到设备Tag]" << query.value(0).toString(); //取第一列
                                                                                //<< query.value(1).toInt(); //取第二列
                }
            }

            qInfo() << "[加载设备点位成功]"
                    << "    " << devicename + "." + pointname;
        }
    }
    //QSqlDatabase::removeDatabase("qt_sql_default_connection");
    QSqlDatabase::removeDatabase("device");

    return true;
}

bool device::setSQLTable(QString tablename, QString listname, QString sqlnumber, QString sqlquery)
{
    sql_table = tablename;
    sql_list = listname;
    sql_number = sqlnumber;
    sql_query = sqlquery;

//    if (sqlquery != "")
//    {
//        for (map<QString, DEVICE>::iterator itlist = tag_list.begin(); itlist != tag_list.end(); itlist++)
//        {
//            // string temp = "insert into " + tablename + " " + "(Tag, Value) values('" + (*itlist).Tag + "', 123);";
//            int inx = sql_number.toInt() + itlist->second.Device_num.toInt() - 1;
//            QString sqltemp = "SELECT * FROM " + tablename + " WHERE n_code = '" + QString::number(inx) + "' AND " + QString::fromStdString(cutPre(sql_query.toStdString(), "=")) + "=" + QString::fromStdString(cutNext(sql_query.toStdString(), "="));
//            if (SQLL.setQuery(sqltemp) == false)
//            {
//                QString temp = "insert into " + sql_table + " (" + "n_code," + QString::fromStdString(cutPre(sql_query.toStdString(), "=")) + ") values(" + QString::number(inx) + "," + QString::fromStdString(cutNext(sql_query.toStdString(), "=")) + ")";
//                SQLL.setSQL(temp);
//            }
//        }
//    }
//    else
//    {
//        for (map<QString, DEVICE>::iterator itlist = tag_list.begin(); itlist != tag_list.end(); itlist++)
//        {
//            // string temp = "insert into " + tablename + " " + "(Tag, Value) values('" + (*itlist).Tag + "', 123);";
//            int inx = sql_number.toInt() + itlist->second.Device_num.toInt() - 1;
//            QString sqltemp = "SELECT * FROM " + tablename + " WHERE n_code = '" + QString::number(inx) + "'";
//            if (SQLL.setQuery(sqltemp) == false)
//            {
//                QString temp = "insert into " + sql_table + " " + "(n_code) values(" + QString::number(inx) + "); ";
//                SQLL.setSQL(temp);
//            }
//        }
//    }
    //QSqlDatabase::removeDatabase("qt_sql_default_connection");
    return true;
}

#include "mainwindow.h"
#include "ui_mainwindow.h"

extern SQL SQLL;

struct cms
{
    QString sequeue1_CT;
    QString sequeue2_CT;
    QString sequeue3_CT;
    QString sequeue4_CT;
    QString sequeue5_CT;
    QString sequeue1_TI;
    QString sequeue2_TI;
    QString sequeue3_TI;
    QString sequeue4_TI;
    QString sequeue5_TI;
    bool send_flag = 0;
};

map<QString, cms> map_cms;
map<QString, QString> map_et;
map<QString, QString> map_ls;
map<QString, QString> map_csls;

void loadHisSubData(void);
void saveHisSubData(void);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("MetroView推送SQLServer")); //设置对话框的标题
    // setWindowIcon(QIcon(":/icons/qt-logo.png"));
    m_login = new LoginWindow;
    m_log = new LogSet;
    m_about = new About;
    m_login->show();
    thread1 = new WorkThread();
    thread2 = new HisThread();
    thread3 = new LoadThread();
    m_load = new Loading();

    connect(thread3, SIGNAL(load_success()), this, SLOT(fresh_Table()));
    connect(thread3, SIGNAL(load_complete()), m_load, SLOT(close()));
    connect(m_login, SIGNAL(close_window()), this, SLOT(show()));

    ui->qTableWidget->setColumnCount(5); //设置列数
    ui->qTableWidget->setRowCount(1000); //设置行数
    ui->qTableWidget->setWindowTitle("推送列表");
    QStringList m_Header;
    m_Header << QString("数据源") << QString("数据源设备类") << QString("数据源设备点") << QString("目标库表名") << QString("目标列名");
    ui->qTableWidget->setHorizontalHeaderLabels(m_Header);                 //添加横向表头
    ui->qTableWidget->verticalHeader()->setVisible(true);                  //纵向表头可视化
    ui->qTableWidget->horizontalHeader()->setVisible(true);                //横向表头可视化
    ui->qTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);  //设置编辑方式：禁止编辑表格
    ui->qTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); //设置表格选择方式：设置表格为整行选中
    ui->qTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->qTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setCentralWidget(ui->qTableWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionLoad_triggered()
{

    m_load->show();
    thread3->start();
}

void MainWindow::on_actionAdd_triggered()
{
    AddDialog *m_add = new AddDialog;
    m_add->show();
    connect(m_add, SIGNAL(fresh_Table()), this, SLOT(fresh_Table()));
}

void MainWindow::on_actionSave_triggered()
{

    QFile file(QApplication::applicationDirPath() + "/Data.json");
    if (!file.open(QIODevice::WriteOnly))
    {
        if (level_log > 0)
        {
            qCritical() << "Data.json文件打开失败";
        }
    }
    else
    {
        // qDebug() << "File open successfully!";
        QJsonObject json;
        int inx = 1;
        for (list<device>::iterator iter = device_list.begin(); iter != device_list.end(); iter++)
        {
            QJsonObject obj;
            char ss[20];
            sprintf(ss, "%d", inx);
            obj["path"] = (*iter).my_path;
            obj["device_name"] = (*iter).device_name;
            obj["point_name"] = (*iter).point_name;
            obj["sql_table"] = (*iter).sql_table;
            obj["sql_list"] = (*iter).sql_list;
            obj["sql_num_start"] = (*iter).sql_number;
            obj["sql_query"] = (*iter).sql_query;
            json.insert(ss, obj);
            inx++;
        }
        QJsonDocument jdoc(json);
        file.write(jdoc.toJson());
        file.flush();
        qInfo() << "保存配置文件成功！";
    }
}

void MainWindow::on_actionExit_triggered()
{
    qInfo() << "退出程序！";
    exit(1);
}

void MainWindow::fresh_Table()
{
    int cout = 0;
    for (list<device>::iterator iter = device_list.begin(); iter != device_list.end(); iter++)
    {
        ui->qTableWidget->setItem(cout, 0, new QTableWidgetItem((*iter).my_path));
        ui->qTableWidget->item(cout, 0)->setTextAlignment(Qt::AlignCenter); //居中显示
        ui->qTableWidget->setItem(cout, 1, new QTableWidgetItem((*iter).device_name));
        ui->qTableWidget->item(cout, 1)->setTextAlignment(Qt::AlignCenter); //居中显示
        ui->qTableWidget->setItem(cout, 2, new QTableWidgetItem((*iter).point_name));
        ui->qTableWidget->item(cout, 2)->setTextAlignment(Qt::AlignCenter); //居中显示
        ui->qTableWidget->setItem(cout, 3, new QTableWidgetItem((*iter).sql_table));
        ui->qTableWidget->item(cout, 3)->setTextAlignment(Qt::AlignCenter); //居中显示
        ui->qTableWidget->setItem(cout, 4, new QTableWidgetItem((*iter).sql_list));
        ui->qTableWidget->item(cout, 4)->setTextAlignment(Qt::AlignCenter); //居中显示
        cout++;
    }
    if (!thread1->isRunning())
    {
        thread1->start();
    }

    QEventLoop eventloop;
    QTimer::singleShot(1000 * 30, &eventloop, &QEventLoop::quit); //防止历史数据丢失，延迟半分钟开始历史数据线程
    eventloop.exec();

    if (!thread2->isRunning())
    {
        thread2->start();
    }
}

LoadThread::LoadThread()
{
}

void LoadThread::run()
{

    device_list.clear();
    QFile file(QApplication::applicationDirPath() + "/Data.json");
    if (!file.open(QIODevice::ReadOnly))
    {
        if (level_log > 0)
        {
            qCritical() << "Data.json文件打开失败";
        }
    }
    else
    {
        // qDebug() << "File open successfully!";

        QJsonParseError jsonError;
        QJsonDocument jdc = QJsonDocument::fromJson(file.readAll(), &jsonError);
        if (!jdc.isNull() && (jsonError.error == QJsonParseError::NoError))
        {
            QJsonObject json = jdc.object();
            for (QJsonObject::Iterator iteratorJson = json.begin(); iteratorJson != json.end(); iteratorJson++)
            {
                device device;

                QJsonObject obj = iteratorJson->toObject();
                device.my_path = obj.value("path").toString();
                device.device_name = obj.value("device_name").toString();
                device.point_name = obj.value("point_name").toString();
                device.sql_table = obj.value("sql_table").toString();
                device.sql_list = obj.value("sql_list").toString();
                device.sql_number = obj.value("sql_num_start").toString();
                device.sql_query = obj.value("sql_query").toString();

                if (device.connectDB(device.my_path, device.device_name, device.point_name))
                {
                    device.setSQLTable(device.sql_table, device.sql_list, device.sql_number, device.sql_query);
                    device_list.push_back(device);
                }
            }
            qInfo() << "加载配置文件成功！";
            emit load_success();
            // fresh_Table();
        }
        else
        {
            if (level_log > 0)
            {
                qCritical() << "Data.json文件解析失败" << jsonError.errorString();
            }
            if (jsonError.error != QJsonParseError::NoError)
            {
                QMessageBox::critical(0, QObject::tr("Json Error"), "文件Json格式错误！");
            }
        }
    }

    emit load_complete();
}

WorkThread::WorkThread()
{
}

void WorkThread::run()
{

    while (1)
    {
        QEventLoop eventloop;
        QTimer::singleShot(1000 * 60, &eventloop, &QEventLoop::quit);
        {
            QSqlDatabase db = QSqlDatabase::addDatabase("QODBC", "setSQL");
            QString dsn = QString("DRIVER={SQL SERVER};SERVER=%1;DATABASE=%2").arg(SQL_ip).arg(SQL_dbname);
            db.setDatabaseName(dsn);
            db.setUserName(SQL_username);
            db.setPassword(SQL_passwd);

            if (!db.open())
            {
                if (level_log > 1)
                {
                    qWarning() << "SQLServer连接失败！";
                }
            }
            else
            {

                if (!device_list.empty())
                {

                    for (list<device>::iterator iter = device_list.begin(); iter != device_list.end(); iter++)
                    {
                        QJsonArray tags;
                        QJsonObject obj;
                        for (map<QString, DEVICE>::iterator itt = (*iter).tag_list.begin(); itt != (*iter).tag_list.end(); itt++)
                        {
                            tags.append(itt->first);
                        }
                        obj.insert("tags", tags);

                        if (level_log > 3)
                        {
                            qDebug() << "[Http上送报文]"
                                     << "   " << QJsonDocument(obj).toJson();
                        }

                        QNetworkRequest request;
                        QNetworkAccessManager oNetAccessManager;
                        QNetworkReply *oNetReply = nullptr;
                        QString url = "http://" + (*iter).my_path + ":7670/tag/speed";
                        // qDebug() << url;
                        // request.setUrl(QUrl("http://192.168.88.88:7670/tag/speed"));
                        request.setUrl(QUrl(url));
                        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
                        oNetReply = oNetAccessManager.post(request, QJsonDocument(obj).toJson(QJsonDocument::Compact));

                        QEventLoop eventloopNet;
                        connect(oNetReply, SIGNAL(finished()), &eventloopNet, SLOT(quit()));
                        QTimer::singleShot(1000 * 5, &eventloopNet, &QEventLoop::quit);
                        eventloopNet.exec();

                        if (oNetReply->isFinished())
                        {
                            if (oNetReply->error() == QNetworkReply::NoError) //正常结束，读取响应数据
                            {
                                QByteArray strResult = oNetReply->readAll();

                                if (level_log > 3)
                                {
                                    qDebug() << "[Http接收报文]"
                                             << "   " << strResult;
                                }

                                QJsonParseError jsonError;
                                QJsonDocument doucment = QJsonDocument::fromJson(strResult, &jsonError); // 转化为 JSON 文档
                                if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
                                {
                                    QJsonObject object = doucment.object(); // 转化为对象
                                    for (QJsonObject::Iterator iteratorJson = object.begin(); iteratorJson != object.end(); iteratorJson++)
                                    {
                                        QJsonObject obj = iteratorJson->toObject();
                                        QJsonObject valobj = obj.value("value").toObject();
                                        QString value;

                                        int index = (*iter).tag_list[iteratorJson.key()].Device_num.toInt() + ((*iter).sql_number.toInt()) - 1;
                                        QString sqlstr = "";

                                        if (obj.value("updatetime").toObject().value("fieldvalue").toInt() != 0) //如果MetroView取到的更新时间正常
                                        {
                                            if (iter->device_name.contains("JJDH", Qt::CaseSensitive)) //紧急电话状态量定义不同
                                            {
                                                switch (valobj.value("fieldvalue").toInt())
                                                {
                                                case 4: //挂机
                                                    value = "2";
                                                    break;
                                                case 3: //摘机
                                                    value = "3";
                                                    break;
                                                case 8: //电话设备故障
                                                case 9: //通讯故障
                                                    value = "1";
                                                    break;
                                                default:
                                                    value = "0";
                                                    break;
                                                }
                                            }
                                            else if (iter->device_name.contains("LS", Qt::CaseSensitive)) //车指器状态量定义不同
                                            {
                                                switch (valobj.value("fieldvalue").toInt())
                                                {
                                                case 0: //断电
                                                    value = "3";
                                                    break;
                                                case 1: //正向通行
                                                    value = "0";
                                                    break;
                                                case 2: //反向通行
                                                    value = "1";
                                                    break;
                                                case 3: //禁止通行
                                                    value = "2";
                                                    break;
                                                default:
                                                    value = "0";
                                                    break;
                                                }
                                            }
                                            else
                                            {
                                                if (valobj.value("fieldvalue").type() == 0x2)
                                                {
                                                    value = QString::number(valobj.value("fieldvalue").toDouble());
                                                }
                                                else
                                                {
                                                    value = valobj.value("fieldvalue").toString();
                                                }
                                            }

                                            if ((*iter).sql_query != "")
                                            {
                                                sqlstr = "IF EXISTS (SELECT n_code FROM " + (*iter).sql_table + " WHERE n_code = '" + QString::number(index) + "' AND " + (*iter).sql_query + ") BEGIN UPDATE " + (*iter).sql_table + " SET t_rec_time = '" + getReqTime(obj.value("updatetime").toObject().value("fieldvalue").toInt()) + "',n_period = '1'," + (*iter).sql_list + " = '" + value + "',c_status = '0' WHERE n_code = '" + QString::number(index) + "' AND " + (*iter).sql_query + " END ELSE BEGIN INSERT INTO " + (*iter).sql_table + " (n_code,n_period,c_status,t_rec_time," + (*iter).sql_list + "," + QString::fromStdString(cutPre((*iter).sql_query.toStdString(), "=")) + ") VALUES ('" + QString::number(index) + "','1','0','" + getReqTime(obj.value("updatetime").toObject().value("fieldvalue").toInt()) + "','" + value + "'," + QString::fromStdString(cutNext((*iter).sql_query.toStdString(), "=")) + ") END";
                                            }
                                            else
                                            {
                                                sqlstr = "IF EXISTS (SELECT n_code FROM " + (*iter).sql_table + " WHERE n_code = '" + QString::number(index) + "') BEGIN UPDATE " + (*iter).sql_table + " SET t_rec_time = '" + getReqTime(obj.value("updatetime").toObject().value("fieldvalue").toInt()) + "',n_period = '1'," + (*iter).sql_list + " = '" + value + "',c_status = '0' WHERE n_code = '" + QString::number(index) + "' END ELSE BEGIN INSERT INTO " + (*iter).sql_table + " (n_code,n_period,c_status,t_rec_time," + (*iter).sql_list + ") VALUES ('" + QString::number(index) + "','1','0','" + getReqTime(obj.value("updatetime").toObject().value("fieldvalue").toInt()) + "','" + value + "') END";
                                            }

                                            db.exec(sqlstr);
                                            if (level_log > 2)
                                            {
                                                qDebug() << "[SQL Server查询语句]"
                                                         << "   " << sqlstr;
                                            }
                                        }
                                        else //设备断线
                                        {

                                            if ((*iter).sql_query != "")
                                            {
                                                sqlstr = "IF EXISTS (SELECT n_code FROM " + (*iter).sql_table + " WHERE n_code = '" + QString::number(index) + "' AND " + (*iter).sql_query + ") BEGIN UPDATE " + (*iter).sql_table + " SET t_rec_time = '" + getReqTime(obj.value("updatetime").toObject().value("fieldvalue").toInt()) + "',n_period = '0'," + (*iter).sql_list + " = '" + value + "',c_status = '1' WHERE n_code = '" + QString::number(index) + "' AND " + (*iter).sql_query + " END ELSE BEGIN INSERT INTO " + (*iter).sql_table + " (n_code,n_period,c_status,t_rec_time," + (*iter).sql_list + "," + QString::fromStdString(cutPre((*iter).sql_query.toStdString(), "=")) + ") VALUES ('" + QString::number(index) + "','0','1','" + getReqTime(obj.value("updatetime").toObject().value("fieldvalue").toInt()) + "','" + value + "'," + QString::fromStdString(cutNext((*iter).sql_query.toStdString(), "=")) + ") END";
                                            }
                                            else
                                            {
                                                sqlstr = "IF EXISTS (SELECT n_code FROM " + (*iter).sql_table + " WHERE n_code = '" + QString::number(index) + "') BEGIN UPDATE " + (*iter).sql_table + " SET t_rec_time = '" + getReqTime(obj.value("updatetime").toObject().value("fieldvalue").toInt()) + "',n_period = '0'," + (*iter).sql_list + " = '" + value + "',c_status = '1' WHERE n_code = '" + QString::number(index) + "' END ELSE BEGIN INSERT INTO " + (*iter).sql_table + " (n_code,n_period,c_status,t_rec_time," + (*iter).sql_list + ") VALUES ('" + QString::number(index) + "','0','1','" + getReqTime(obj.value("updatetime").toObject().value("fieldvalue").toInt()) + "','" + value + "') END";
                                            }

                                            db.exec(sqlstr);
                                            if (level_log > 2)
                                            {
                                                qDebug() << "[SQL Server查询语句]"
                                                         << "   " << sqlstr;
                                            }
                                        }
                                    }

                                } // 解析未发生错误

                                if (level_log > 1)
                                {
                                    qInfo() << "[实时库上传任务成功]"
                                            << "   " << (*iter).device_name + "." + (*iter).point_name;
                                }
                            }
                            else //异常结束，比如不存在的http服务器
                            {
                                if (level_log > 0)
                                {
                                    qCritical() << "MetroView接口连接异常" << oNetReply->errorString();
                                }
                            }
                        }
                        else //超时,错误信息：Operation canceled
                        {
                            disconnect(oNetReply, &QNetworkReply::finished, &eventloopNet, &QEventLoop::quit);
                            oNetReply->abort();
                            if (level_log > 0)
                            {
                                qCritical() << "MetroView接口连接超时";
                            }
                        }
                    }
                }
                else
                {
                    qInfo() << "推送任务列表为空";
                    QThread::sleep(1000 * 10);
                }
            }
        }
        QSqlDatabase::removeDatabase("setSQL");
        eventloop.exec();
    }
}

HisThread::HisThread()
{
}

void HisThread::run()
{
    uint8_t time_count = 5;

    loadHisSubData();

    while (1)
    {
        QEventLoop eventloop;
        QTimer::singleShot(1000 * 60, &eventloop, &QEventLoop::quit);
        {

            QSqlDatabase db = QSqlDatabase::addDatabase("QODBC", "HisUpdate");
            QString dsn = QString("DRIVER={SQL SERVER};SERVER=%1;DATABASE=%2").arg(SQL_ip).arg(SQL_dbname);
            db.setDatabaseName(dsn);
            db.setUserName(SQL_username);
            db.setPassword(SQL_passwd);
            if (!db.open())
            {
                if (level_log > 1)
                {
                    qWarning() << "SQLServer连接失败！";
                }
            }
            else
            {
                QString sqltemp;

                {
                    QSqlQuery query(db);
                    query.exec("SELECT * FROM CUR_AERO");
                    while (true == query.next())
                    {
                        QString n_code = query.value(0).toString();
                        QString n_time_stamp = query.value(11).toString();
                        QString d_temp = query.value(2).toString();
                        QString d_humidity = query.value(3).toString();
                        QString c_icing = query.value(4).toString();
                        QString n_visibility = query.value(5).toString();
                        QString c_wind_dir = query.value(6).toString();
                        QString d_wind_speed = query.value(7).toString();
                        QString d_rainfall = query.value(8).toString();
                        QString t_rec_time = getReqTime();

                        sqltemp = "INSERT INTO HIST_AERO (n_code,n_time_stamp,d_temp,d_humidity,c_icing,n_visibility,c_wind_dir,d_wind_speed,d_rainfall,t_rec_time,c_send_flag,c_stat_falg) VALUES ('" +
                                  n_code + "','" + n_time_stamp + "','" + d_temp + "','" + d_humidity + "','" + c_icing + "','" + n_visibility + "','" + c_wind_dir + "','" + d_wind_speed + "','" + d_rainfall + "','" + t_rec_time + "','" + "0" + "','" + "0" + "')";

                        if (level_log > 2)
                        {
                            qDebug() << "[SQL Server查询语句]"
                                     << "   " << sqltemp;
                        }

                        db.exec(sqltemp);
                    }
                    if (level_log > 1)
                    {
                        qInfo() << "[历史库上传任务成功]"
                                << "   "
                                << "HIST_AERO";
                    }
                }

                {
                    QSqlQuery query(db);
                    query.exec("SELECT * FROM CUR_CSLS");
                    while (true == query.next())
                    {
                        QString n_code = query.value(0).toString();
                        QString n_limit = query.value(2).toString();
                        QString c_status = query.value(3).toString();
                        QString t_rec_time = getReqTime();

                        map<QString, QString>::iterator iter = map_csls.find(n_code);
                        if (iter == map_csls.end())
                        {
                            map_csls.insert(pair<QString, QString>(n_code, NULL));
                            iter = map_csls.find(n_code);
                        }
                        if (iter->second.compare(n_limit))
                        {
                            iter->second = n_limit;
                            sqltemp = "INSERT INTO HIST_CSLS (n_code,n_limit,c_status,t_rec_time,c_send_flag) VALUES ('" +
                                      n_code + "','" + n_limit + "','" + c_status + "','" + t_rec_time + "','" + "0" + "')";

                            if (level_log > 2)
                            {
                                qDebug() << "[SQL Server查询语句]"
                                         << "   " << sqltemp;
                            }

                            db.exec(sqltemp);
                        }
                    }
                    if (level_log > 1)
                    {
                        qInfo() << "[历史库上传任务成功]"
                                << "   "
                                << "HIST_CSLS";
                    }
                }

                {

                    QSqlQuery query(db);
                    query.exec("SELECT * FROM CUR_CMS");
                    while (true == query.next())
                    {
                        QString n_code = query.value(0).toString();
                        map<QString, cms>::iterator iter = map_cms.find(n_code);

                        if (iter == map_cms.end())
                        {
                            cms empty_cms;
                            map_cms.insert(pair<QString, cms>(n_code, empty_cms));
                            iter = map_cms.find(n_code);
                        }

                        QString tempCT = query.value(3).toString();
                        QString tempTI = QString::number(query.value(4).toInt());

                        switch (query.value(2).toInt())
                        {
                        case 1:
                            if (iter->second.sequeue1_CT.compare(tempCT))
                            {
                                iter->second.sequeue1_CT = tempCT;
                                iter->second.send_flag = 1;
                            }
                            if (iter->second.sequeue1_TI.compare(tempTI))
                            {
                                iter->second.sequeue1_TI = tempTI;
                                iter->second.send_flag = 1;
                            }
                            break;
                        case 2:
                            if (iter->second.sequeue2_CT.compare(tempCT))
                            {
                                iter->second.sequeue2_CT = tempCT;
                                iter->second.send_flag = 1;
                            }
                            if (iter->second.sequeue2_TI.compare(tempTI))
                            {
                                iter->second.sequeue2_TI = tempTI;
                                iter->second.send_flag = 1;
                            }
                            break;
                        case 3:
                            if (iter->second.sequeue3_CT.compare(tempCT))
                            {
                                iter->second.sequeue3_CT = tempCT;
                                iter->second.send_flag = 1;
                            }
                            if (iter->second.sequeue3_TI.compare(tempTI))
                            {
                                iter->second.sequeue3_TI = tempTI;
                                iter->second.send_flag = 1;
                            }
                            break;
                        case 4:
                            if (iter->second.sequeue4_CT.compare(tempCT))
                            {
                                iter->second.sequeue4_CT = tempCT;
                                iter->second.send_flag = 1;
                            }
                            if (iter->second.sequeue4_TI.compare(tempTI))
                            {
                                iter->second.sequeue4_TI = tempTI;
                                iter->second.send_flag = 1;
                            }
                            break;
                        case 5:
                            if (iter->second.sequeue5_CT.compare(tempCT))
                            {
                                iter->second.sequeue5_CT = tempCT;
                                iter->second.send_flag = 1;
                            }
                            if (iter->second.sequeue5_TI.compare(tempTI))
                            {
                                iter->second.sequeue5_TI = tempTI;
                                iter->second.send_flag = 1;
                            }
                            break;
                        default:
                            break;
                        }
                    }

                    for (map<QString, cms>::iterator iter = map_cms.begin(); iter != map_cms.end(); iter++)
                    {
                        if (iter->second.send_flag)
                        {
                            QString n_code = iter->first;
                            QString t_rec_time = getReqTime();
                            QString content = iter->second.sequeue1_CT + "/" + iter->second.sequeue1_TI + "|" + iter->second.sequeue2_CT + "/" + iter->second.sequeue2_TI + "|" + iter->second.sequeue3_CT + "/" + iter->second.sequeue3_TI + "|" + iter->second.sequeue4_CT + "/" + iter->second.sequeue4_TI + "|" + iter->second.sequeue5_CT + "/" + iter->second.sequeue5_TI;
                            sqltemp = "INSERT INTO HIST_CMS (n_code,vc_orig_content,vc_content,c_status,t_rec_time,c_send_flag) VALUES ('" +
                                      n_code + "','" + content + "','" + content + "','" + "0" + "','" + t_rec_time + "','" + "0" + "')";
                            if (level_log > 2)
                            {
                                qDebug() << "[SQL Server查询语句]"
                                         << "   " << sqltemp;
                            }

                            db.exec(sqltemp);
                            iter->second.send_flag = 0;
                        }
                    }
                    if (level_log > 1)
                    {
                        qInfo() << "[历史库上传任务成功]"
                                << "   "
                                << "HIST_CMS";
                    }
                }

                {
                    QSqlQuery query(db);
                    query.exec("SELECT * FROM CUR_VI");
                    while (true == query.next())
                    {
                        QString n_code = query.value(0).toString();
                        QString n_time_stamp = query.value(5).toString();
                        QString n_visibility = query.value(2).toString();
                        QString t_rec_time = getReqTime();

                        sqltemp = "INSERT INTO HIST_VI (n_code,n_time_stamp,n_visibility,t_rec_time,c_send_flag,c_stat_falg) VALUES ('" +
                                  n_code + "','" + n_time_stamp + "','" + n_visibility + "','" + t_rec_time + "','" + "0" + "','" + "0" + "')";

                        if (level_log > 2)
                        {
                            qDebug() << "[SQL Server查询语句]"
                                     << "   " << sqltemp;
                        }

                        db.exec(sqltemp);
                    }
                    if (level_log > 1)
                    {
                        qInfo() << "[历史库上传任务成功]"
                                << "   "
                                << "HIST_VI";
                    }
                }

                {
                    QSqlQuery query(db);
                    query.exec("SELECT * FROM CUR_ET");
                    while (true == query.next())
                    {
                        QString n_code = query.value(0).toString();
                        QString c_et_status = query.value(2).toString();
                        QString t_rec_time = getReqTime();
                        QString status = "0";
                        if (!c_et_status.compare(QString::fromLocal8Bit("1")))
                        {
                            status = c_et_status;
                        }
                        map<QString, QString>::iterator iter = map_et.find(n_code);
                        if (iter == map_et.end())
                        {
                            map_et.insert(pair<QString, QString>(n_code, NULL));
                            iter = map_et.find(n_code);
                        }
                        if (iter->second.compare(c_et_status))
                        {
                            iter->second = c_et_status;

                            sqltemp = "INSERT INTO HIST_ET (n_code,c_et_status,c_status,t_rec_time,c_send_flag) VALUES ('" +
                                      n_code + "','" + c_et_status + "','" + status + "','" + t_rec_time + "','" + "0" + "')";

                            if (level_log > 2)
                            {
                                qDebug() << "[SQL Server查询语句]"
                                         << "   " << sqltemp;
                            }

                            db.exec(sqltemp);
                        }
                    }
                    if (level_log > 1)
                    {
                        qInfo() << "[历史库上传任务成功]"
                                << "   "
                                << "HIST_ET";
                    }
                }

                {
                    QSqlQuery query(db);
                    query.exec("SELECT * FROM CUR_LS");
                    while (true == query.next())
                    {
                        QString n_code = query.value(0).toString();
                        QString c_ls_status = query.value(2).toString();
                        QString t_rec_time = getReqTime();
                        QString status = "0";

                        map<QString, QString>::iterator iter = map_et.find(n_code);
                        if (iter == map_et.end())
                        {
                            map_et.insert(pair<QString, QString>(n_code, NULL));
                            iter = map_et.find(n_code);
                        }
                        if (iter->second.compare(c_ls_status))
                        {
                            iter->second = c_ls_status;

                            sqltemp = "INSERT INTO HIST_LS (n_code,c_ls_status,c_status,t_rec_time,c_send_flag) VALUES ('" +
                                      n_code + "','" + c_ls_status + "','" + status + "','" + t_rec_time + "','" + "0" + "')";

                            if (level_log > 2)
                            {
                                qDebug() << "[SQL Server查询语句]"
                                         << "   " << sqltemp;
                            }

                            db.exec(sqltemp);
                        }
                    }
                    if (level_log > 1)
                    {
                        qInfo() << "[历史库上传任务成功]"
                                << "   "
                                << "HIST_LS";
                    }
                }

                //车检器数据底层五分钟采集一次
                if (time_count == 5)
                {
                    {
                        QSqlQuery query(db);
                        query.exec("SELECT * FROM CUR_VD");
                        while (true == query.next())
                        {
                            QString n_code = query.value(0).toString();
                            QString d_up_lane1_occupy = query.value(2).toString();
                            QString d_up_lane1_speed = query.value(3).toString();
                            QString n_up_lane1_quantity = query.value(4).toString();
                            QString d_up_lane2_occupy = query.value(5).toString();
                            QString d_up_lane2_speed = query.value(6).toString();
                            QString n_up_lane2_quantity = query.value(7).toString();
                            QString d_up_lane3_occupy = query.value(8).toString();
                            QString d_up_lane3_speed = query.value(9).toString();
                            QString n_up_lane3_quantity = query.value(10).toString();
                            QString d_up_lane4_occupy = query.value(11).toString();
                            QString d_up_lane4_speed = query.value(12).toString();
                            QString n_up_lane4_quantity = query.value(13).toString();
                            QString d_up_lane5_occupy = query.value(14).toString();
                            QString d_up_lane5_speed = query.value(15).toString();
                            QString n_up_lane5_quantity = query.value(16).toString();
                            QString d_down_lane1_occupy = query.value(17).toString();
                            QString d_down_lane1_speed = query.value(18).toString();
                            QString n_down_lane1_quantity = query.value(19).toString();
                            QString d_down_lane2_occupy = query.value(20).toString();
                            QString d_down_lane2_speed = query.value(21).toString();
                            QString n_down_lane2_quantity = query.value(22).toString();
                            QString d_down_lane3_occupy = query.value(23).toString();
                            QString d_down_lane3_speed = query.value(24).toString();
                            QString n_down_lane3_quantity = query.value(25).toString();
                            QString d_down_lane4_occupy = query.value(26).toString();
                            QString d_down_lane4_speed = query.value(27).toString();
                            QString n_down_lane4_quantity = query.value(28).toString();
                            QString d_down_lane5_occupy = query.value(29).toString();
                            QString d_down_lane5_speed = query.value(30).toString();
                            QString n_down_lane5_quantity = query.value(31).toString();
                            QString t_rec_time = query.value(34).toString();

                            sqltemp = "INSERT INTO HIST_VD (n_code,n_time_stamp,d_up_lane1_occupy,d_up_lane1_speed,n_up_lane1_quantity,"
                                      "d_up_lane2_occupy,d_up_lane2_speed,n_up_lane2_quantity,"
                                      "d_up_lane3_occupy,d_up_lane3_speed,n_up_lane3_quantity,"
                                      "d_up_lane4_occupy,d_up_lane4_speed,n_up_lane4_quantity,"
                                      "d_up_lane5_occupy,d_up_lane5_speed,n_up_lane5_quantity,"
                                      "d_down_lane1_occupy,d_down_lane1_speed,n_down_lane1_quantity,"
                                      "d_down_lane2_occupy,d_down_lane2_speed,n_down_lane2_quantity,"
                                      "d_down_lane3_occupy,d_down_lane3_speed,n_down_lane3_quantity,"
                                      "d_down_lane4_occupy,d_down_lane4_speed,n_down_lane4_quantity,"
                                      "d_down_lane5_occupy,d_down_lane5_speed,n_down_lane5_quantity,t_rec_time,c_send_flag,c_stat_falg) VALUES ('" +
                                      n_code + "','" + t_rec_time + "','" + d_up_lane1_occupy + "','" + d_up_lane1_speed + "','" + n_up_lane1_quantity + "','" + d_up_lane2_occupy + "','" + d_up_lane2_speed + "','" + n_up_lane2_quantity + "','" + d_up_lane3_occupy + "','" + d_up_lane3_speed + "','" + n_up_lane3_quantity + "','" + d_up_lane4_occupy + "','" + d_up_lane4_speed + "','" + n_up_lane4_quantity + "','" + d_up_lane5_occupy + "','" + d_up_lane5_speed + "','" + n_up_lane5_quantity + "','" + d_down_lane1_occupy + "','" + d_down_lane1_speed + "','" + n_down_lane1_quantity + "','" + d_down_lane2_occupy + "','" + d_down_lane2_speed + "','" + n_down_lane2_quantity + "','" + d_down_lane3_occupy + "','" + d_down_lane3_speed + "','" + n_down_lane3_quantity + "','" + d_down_lane4_occupy + "','" + d_down_lane4_speed + "','" + n_down_lane4_quantity + "','" + d_down_lane5_occupy + "','" + d_down_lane5_speed + "','" + n_down_lane5_quantity + "','" + getReqTime() + "','" + "0" + "','" + "0" + "')";
                            if (level_log > 2)
                            {
                                qDebug() << "[SQL Server查询语句]"
                                         << "   " << sqltemp;
                            }
                            db.exec(sqltemp);
                        }
                    }

                    if (level_log > 1)
                    {
                        qInfo() << "[历史库上传任务成功]"
                                << "   "
                                << "HIST_VD";
                    }
                    time_count = 1;
                }
                else
                {
                    time_count++;
                }
            }
        }
        QSqlDatabase::removeDatabase("HisUpdate");
        eventloop.exec();
    }
}

void MainWindow::on_actionmin_triggered()
{
    this->hide();
    mSysTrayIcon = new QSystemTrayIcon(this);
    QIcon icon = QIcon("appico.png");
    mSysTrayIcon->setIcon(icon);
    mSysTrayIcon->setToolTip("MetroView推送SQLServer服务");
    connect(mSysTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason)));

    mSysTrayIcon->show();
}

void MainWindow::on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
        //单击托盘图标
        break;
    case QSystemTrayIcon::DoubleClick:
        //双击托盘图标
        //双击后显示主程序窗口
        this->show();
        this->setWindowState(Qt::WindowNoState);
        delete (mSysTrayIcon);
        break;
    default:
        break;
    }
}

void MainWindow::on_actionopition_triggered()
{
    m_log->show();
}

void MainWindow::changeEvent(QEvent *event)
{
    // if (event->type() != QEvent::WindowStateChange)
    //     return;
    // if (this->windowState() == Qt::WindowMinimized)
    // {
    //     on_actionmin_triggered();
    // }
}

void loadHisSubData()
{
    map_cms.clear();
    map_et.clear();
    map_ls.clear();
    map_csls.clear();

    QFile file(QApplication::applicationDirPath() + "/HisSubData.json");
    if (!file.open(QIODevice::ReadOnly))
    {
        if (level_log > 0)
        {
            qCritical() << "HisSubData.json文件打开失败";
        }
    }
    else
    {
        // qDebug() << "File open successfully!";

        QJsonParseError jsonError;
        QJsonDocument jdc = QJsonDocument::fromJson(file.readAll(), &jsonError);
        if (!jdc.isNull() && (jsonError.error == QJsonParseError::NoError))
        {
            QJsonObject json = jdc.object();
            QJsonObject objCMS = json.value("CMS").toObject();
            QJsonObject objET = json.value("ET").toObject();
            QJsonObject objLS = json.value("LS").toObject();
            QJsonObject objCSLS = json.value("CSLS").toObject();

            for (QJsonObject::Iterator iteratorJson = objCMS.begin(); iteratorJson != objCMS.end(); iteratorJson++)
            {
                cms tempcms;
                QJsonObject obj = iteratorJson->toObject();

                tempcms.send_flag =obj.value("send_flag").toBool();
                tempcms.sequeue1_CT =obj.value("sequeue1_CT").toString();
                tempcms.sequeue1_TI =obj.value("sequeue1_TI").toString();
                tempcms.sequeue2_CT =obj.value("sequeue2_CT").toString();
                tempcms.sequeue2_TI =obj.value("sequeue2_TI").toString();
                tempcms.sequeue3_CT =obj.value("sequeue3_CT").toString();
                tempcms.sequeue3_TI =obj.value("sequeue3_TI").toString();
                tempcms.sequeue4_CT =obj.value("sequeue4_CT").toString();
                tempcms.sequeue4_TI =obj.value("sequeue4_TI").toString();
                tempcms.sequeue5_CT =obj.value("sequeue5_CT").toString();
                tempcms.sequeue5_TI =obj.value("sequeue5_TI").toString();

                map_cms.insert(pair<QString,cms>(iteratorJson.key(),tempcms));

            }

            for (QJsonObject::Iterator iteratorJson = objET.begin(); iteratorJson != objET.end(); iteratorJson++)
            {
                map_et.insert(pair<QString,QString>(iteratorJson.key(),iteratorJson->toString()));
            }
            for (QJsonObject::Iterator iteratorJson = objLS.begin(); iteratorJson != objLS.end(); iteratorJson++)
            {
                map_ls.insert(pair<QString,QString>(iteratorJson.key(),iteratorJson->toString()));
            }
            for (QJsonObject::Iterator iteratorJson = objCSLS.begin(); iteratorJson != objCSLS.end(); iteratorJson++)
            {
                map_csls.insert(pair<QString,QString>(iteratorJson.key(),iteratorJson->toString()));
            }

            qInfo() << "加载历史数据缓存文件成功！";
        }
        else
        {
            if (level_log > 0)
            {
                qCritical() << "HisSubData.json文件解析失败" << jsonError.errorString();
            }
            if (jsonError.error != QJsonParseError::NoError)
            {
                QMessageBox::critical(0, QObject::tr("Json Error"), "文件Json格式错误！");
            }
        }
    }
}

void saveHisSubData()
{
    QFile file(QApplication::applicationDirPath() + "/HisSubData.json");
    if (!file.open(QIODevice::WriteOnly))
    {
        if (level_log > 0)
        {
            qCritical() << "HisSubData.json文件打开失败";
        }
    }
    else
    {
        QJsonObject json;
        QJsonObject objCMS;
        QJsonObject objET;
        QJsonObject objLS;
        QJsonObject objCSLS;

        for (map<QString, cms>::iterator iter = map_cms.begin(); iter != map_cms.end(); iter++)
        {
            QJsonObject obj;
            obj["sequeue1_CT"] = (*iter).second.sequeue1_CT;
            obj["sequeue1_TI"] = (*iter).second.sequeue1_TI;
            obj["sequeue2_CT"] = (*iter).second.sequeue2_CT;
            obj["sequeue2_TI"] = (*iter).second.sequeue2_TI;
            obj["sequeue3_CT"] = (*iter).second.sequeue3_CT;
            obj["sequeue3_TI"] = (*iter).second.sequeue3_TI;
            obj["sequeue4_CT"] = (*iter).second.sequeue4_CT;
            obj["sequeue4_TI"] = (*iter).second.sequeue4_TI;
            obj["sequeue5_CT"] = (*iter).second.sequeue5_CT;
            obj["sequeue5_TI"] = (*iter).second.sequeue5_TI;
            obj["send_flag"] = (*iter).second.send_flag;
            objCMS.insert((*iter).first, obj);
        }

        for (map<QString, QString>::iterator iter = map_et.begin(); iter != map_et.end(); iter++)
        {
            objET.insert((*iter).first, (*iter).second);
        }

        for (map<QString, QString>::iterator iter = map_ls.begin(); iter != map_ls.end(); iter++)
        {
            objET.insert((*iter).first, (*iter).second);
        }

        for (map<QString, QString>::iterator iter = map_csls.begin(); iter != map_csls.end(); iter++)
        {
            objET.insert((*iter).first, (*iter).second);
        }

        json.insert("CMS", objCMS);
        json.insert("ET", objET);
        json.insert("LS", objLS);
        json.insert("CSLS", objCSLS);

        QJsonDocument jdoc(json);
        file.write(jdoc.toJson());
        file.flush();
        qInfo() << "保存历史数据缓存文件成功！";
    }
}

void MainWindow::closeEvent(QCloseEvent *e)
{

    saveHisSubData();

    qInfo() << "退出程序！";
}

void MainWindow::on_actionabout_triggered()
{
    m_about->show();
}

void MainWindow::on_actionorcle_triggered()
{
    //    QLibrary *hello_lib = NULL;
    //        //写清楚库的路径，如果放在当前工程的目录下，路径为./Oracle.so
    //        //hello_lib = new QLibrary("D:\\Download\\WINDOWS.X64_193000_db_home\\bin\\oci.dll");
    //        hello_lib = new QLibrary("D:\\QT\\6.2.4\\mingw_64\\plugins\\sqldrivers\\qsqloci.dll");

    //        //加载动态库
    //        hello_lib->load();
    //        if (!hello_lib->isLoaded())
    //        {
    //            qDebug()<<"load Oracle failed!\n";
    //            return ;
    //        }

    // qDebug()<<QSqlDatabase::drivers()<<QApplication::libraryPaths();

//    QSqlDatabase dbOrcle = QSqlDatabase::addDatabase("QOCI", "Orcle");

//    dbOrcle.setDatabaseName("orcldatabase");
//    //    dbOrcle.setHostName(tr("172.20.31.223"));
//    dbOrcle.setPort(1521);
//    //    dbOrcle.setUserName(tr("hiway_test"));
//    //    dbOrcle.setPassword(tr("hiway_test"));
//    dbOrcle.setHostName(tr("localhost"));
//    dbOrcle.setUserName(tr("system"));
//    dbOrcle.setPassword(tr("hiway_test"));
//    if (!dbOrcle.open())
//    {
//        if (level_log > 1)
//        {
//            qWarning() << "Orcle连接失败！";
//            qWarning() << dbOrcle.lastError().text();
//        }
//    }
//    else
//    {
//        qDebug() << "oracle连接成功";
//    }
}

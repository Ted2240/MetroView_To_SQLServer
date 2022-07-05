#include "adddialog.h"
#include "ui_adddialog.h"

AddDialog::AddDialog(QWidget *parent) : QDialog(parent),
                                        ui(new Ui::AddDialog)
{
    ui->setupUi(this);
    ui->buttonBox->addButton("确认", QDialogButtonBox::AcceptRole);
    ui->buttonBox->addButton("取消", QDialogButtonBox::RejectRole);
    connect(this, SIGNAL(fresh_Table()), parent, SLOT(fresh_Table()));
}

AddDialog::~AddDialog()
{
    delete ui;
}

void AddDialog::on_buttonBox_accepted()
{
    // QMessageBox::critical(0, "错误","sds");

    if (ui->PointEdit->text().isEmpty() || ui->TableEdit->text().isEmpty() || ui->NumEdit->text().isEmpty() || ui->S_DeviceEdit->text().isEmpty() || ui->S_DevicePointEdit->text().isEmpty() || ui->S_IPEdit->text().isEmpty())
    {
        QMessageBox::critical(0, QObject::tr("Error"), "参数不允许为空");
    }
    else
    {
        QString s_query = ui->QueryEdit->text();
        QString point = ui->PointEdit->text();
        QString table = ui->TableEdit->text();
        QString number = ui->NumEdit->text();
        QString s_device = ui->S_DeviceEdit->text();
        QString s_devicepoint = ui->S_DevicePointEdit->text();
        QString s_ip = ui->S_IPEdit->text();

        device device;
        if (device.connectDB(s_ip, s_device, s_devicepoint))
        {
            device.setSQLTable(table, point, number,s_query);
            qInfo() << "新增上传任务成功"
                    << "   " << s_device + "." + s_devicepoint;
            device_list.push_back(device);
            emit(fresh_Table());
        }
        else
        {
        }
    }
}

void AddDialog::on_buttonBox_rejected(){

}

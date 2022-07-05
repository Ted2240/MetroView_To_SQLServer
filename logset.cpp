#include "logset.h"
#include "ui_logset.h"

LogSet::LogSet(QWidget *parent) : QDialog(parent),
                                  ui(new Ui::LogSet)
{
    ui->setupUi(this);
    ui->buttonBox->addButton("确认", QDialogButtonBox::AcceptRole);
    ui->buttonBox->addButton("取消", QDialogButtonBox::RejectRole);
}

LogSet::~LogSet()
{
    delete ui;
}

void LogSet::on_buttonBox_accepted()
{
    if (level_log != ui->comboBox->currentIndex())
    {
        level_log = ui->comboBox->currentIndex();
        qInfo() << "日志等级更改为" << level_log;
    }
    this->hide();
}

void LogSet::showEvent(QShowEvent *event)
{
    switch (level_log)
    {
    case 0:
        ui->comboBox->setCurrentIndex(0);
        break;
    case 1:
        ui->comboBox->setCurrentIndex(1);
        break;
    case 2:
        ui->comboBox->setCurrentIndex(2);
        break;
    case 3:
        ui->comboBox->setCurrentIndex(3);
        break;
    case 4:
        ui->comboBox->setCurrentIndex(4);
        break;
    default:
        break;
    }
}

void LogSet::on_buttonBox_rejected()
{
    this->hide();
}

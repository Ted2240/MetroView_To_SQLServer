#include "orcle.h"
#include "ui_orcle.h"

orcle::orcle(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::orcle)
{
    ui->setupUi(this);
}

orcle::~orcle()
{
    delete ui;
}

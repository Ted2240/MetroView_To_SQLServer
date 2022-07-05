#include "loading.h"
#include "ui_loading.h"

Loading::Loading(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Loading)
{
    ui->setupUi(this);
    setWindowFlags ( Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint );
    setWindowModality(Qt::ApplicationModal);
    setWindowTitle(tr("Loading"));
}

Loading::~Loading()
{
    delete ui;
}

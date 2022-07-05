#ifndef ADDDIALOG_H
#define ADDDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include "device.h"
#include "mainwindow.h"

namespace Ui
{
    class AddDialog;
}

class AddDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddDialog(QWidget *parent = nullptr);
    ~AddDialog();

signals:
    void fresh_Table();

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::AddDialog *ui;
};

#endif // ADDDIALOG_H

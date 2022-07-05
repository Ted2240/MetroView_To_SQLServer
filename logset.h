#ifndef LOGSET_H
#define LOGSET_H

#include <QDialog>
#include <QDebug>

namespace Ui
{
    class LogSet;
}

class LogSet : public QDialog
{
    Q_OBJECT

public:
    explicit LogSet(QWidget *parent = nullptr);
    ~LogSet();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    void showEvent(QShowEvent *event);
    Ui::LogSet *ui;
};

extern int level_log;

#endif // LOGSET_H

#ifndef ORCLE_H
#define ORCLE_H

#include <QDialog>

namespace Ui {
class orcle;
}

class orcle : public QDialog
{
    Q_OBJECT

public:
    explicit orcle(QWidget *parent = nullptr);
    ~orcle();

private:
    Ui::orcle *ui;
};

#endif // ORCLE_H

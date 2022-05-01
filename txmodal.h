#ifndef txModal_H
#define txModal_H

#include <QDialog>
#include "portal.h"
namespace Ui {
class txModal;
}

class txModal : public QDialog
{
    Q_OBJECT

public:
    int Baud;
    explicit txModal(QWidget *parent,int txPower);
    ~txModal();
signals:
    void txSetSig(int);
private slots:
    void on_txSetBut_accepted();
    void on_txSetBut_rejected();

private:
    Ui::txModal *ui;
};

#endif // txModal_H

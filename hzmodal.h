#ifndef hzModal_H
#define hzModal_H

#include <QDialog>
#include "portal.h"
namespace Ui {
class hzModal;
}

class hzModal : public QDialog
{
    Q_OBJECT

public:
    int Baud;
    explicit hzModal(QWidget *parent,int hz);
    ~hzModal();
signals:
    void hzSetSig(int);
public slots:
    void on_hzSetBut_accepted();
    void on_hzSetBut_rejected();


private:
    Ui::hzModal *ui;
};

#endif // hzModal_H

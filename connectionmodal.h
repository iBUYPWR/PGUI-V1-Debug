#ifndef CONNECTIONMODAL_H
#define CONNECTIONMODAL_H

#include <QDialog>
#include "portal.h"
namespace Ui {
class connectionModal;
}

class connectionModal : public QDialog
{
    Q_OBJECT

public:
    int Baud;
    explicit connectionModal(QWidget *parent);
    ~connectionModal();
    int getBaud();
    QString getComPort();
    QString getParity();
    void setComPortOptions(QStringList list);
    void setBaudRateOptions(QStringList list);
    void setParityOptions(QStringList list);
signals:
    void initPortalSig(QString,int,QString);
private slots:
    void on_initBut_accepted();

    void on_initBut_rejected();

private:
    Ui::connectionModal *ui;
};

#endif // CONNECTIONMODAL_H

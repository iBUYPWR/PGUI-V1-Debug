#include "hzmodal.h"
#include "ui_hzmodal.h"
#include <qdebug.h>
#include "iostream"
hzModal::hzModal(QWidget *parent,int hz) :
QDialog(parent),
ui(new Ui::hzModal)
{
    ui->setupUi(this);
    ui->hzSettler->setValue(hz);
}
hzModal::~hzModal()
{
    delete ui;
}
void hzModal::on_hzSetBut_accepted()
{
    emit hzSetSig(ui->hzSettler->value());
    close();
}


void hzModal::on_hzSetBut_rejected()
{
    close();
}

		

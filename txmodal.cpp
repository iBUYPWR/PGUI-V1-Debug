#include "txmodal.h"
#include "ui_txmodal.h"
#include <qdebug.h>
#include "iostream"
txModal::txModal(QWidget *parent,int txPower) :
QDialog(parent),
ui(new Ui::txModal)
{
    ui->setupUi(this);
    ui->txSettler->setValue(txPower);
}

txModal::~txModal()
{
    delete ui;
}


		

void txModal::on_txSetBut_accepted()
{
    emit txSetSig(ui->txSettler->value());
    close();
}


void txModal::on_txSetBut_rejected()
{
    close();
}


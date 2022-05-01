#include "connectionmodal.h"
#include "ui_connectionmodal.h"
#include <qdebug.h>
#include "iostream"
#include "qserialportinfo.h"
connectionModal::connectionModal(QWidget *parent) :
QDialog(parent),
ui(new Ui::connectionModal)
{
    QStringList box;
    Q_FOREACH(QSerialPortInfo port, QSerialPortInfo::availablePorts()) {
        box.append(port.portName());
    }
    ui->setupUi(this);
    setBaudRateOptions({"115200"});
    setParityOptions({"Odd","None"});
    setComPortOptions(box);
}

int connectionModal::getBaud(){
    return ui->BaudRateInp->currentText().toInt();
}
QString connectionModal::getComPort(){
    return ui->ComPortInp->currentText();
}
QString connectionModal::getParity(){
    return ui->ParityInp->currentText();
}
connectionModal::~connectionModal()
{
    delete ui;
}



void connectionModal::setComPortOptions(QStringList list){
    ui->ComPortInp->addItems(list);
}
void connectionModal::setBaudRateOptions(QStringList list){
    ui->BaudRateInp->addItems(list);
}

void connectionModal::setParityOptions(QStringList list){
    ui->ParityInp->addItems(list);
}

void connectionModal::on_initBut_accepted()
{
    QString setting = getComPort();
    QString parity = getParity();
    int baudrate = getBaud();
    emit initPortalSig(setting,baudrate,parity);
    close();
	}
		

void connectionModal::on_initBut_rejected()
{
    close();
}


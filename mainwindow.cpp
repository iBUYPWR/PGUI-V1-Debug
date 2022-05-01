#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newtype.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //QSizePolicy sp_retain = //ui->tabwidget_2->sizePolicy();
    //sp_retain.setRetainSizeWhenHidden(true);
    //ui->tabwidget_2->setSizePolicy(sp_retain);
    //ui->tabwidget_2->setVisible(false);
    //ui->tabwidget_2->setEnabled(false);
    //ui->connLED->setPixmap(QPixmap("red.png"));
    //ui->syncLED->setPixmap(QPixmap("red.png"));
    //connect(ui->txIndicator,SIGNAL(valueChanged(int)),this,SLOT(txSetPrompt(int)));
    //connect(ui->hzIndicator,SIGNAL(valueChanged(int)),this,SLOT(hzSetPrompt(int)));
    connect(ui->disconnButt,SIGNAL(clicked()),this,SLOT(quitPortal()));
    connect(ui->connBut,SIGNAL(clicked()),this,SLOT(showConnectionModal()));
    connect(ui->setTxModal,SIGNAL(clicked()),this,SLOT(showTxModal()));
    connect(ui->setHzModal,SIGNAL(clicked()),this,SLOT(showHzModal()));
    connect(ui->resetBut,SIGNAL(clicked()),this,SLOT(resetPrompt()));
    //Construct();
}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::showConnectionModal(){
    connectionModal m_conMod(this);
    connect(&m_conMod,SIGNAL(initPortalSig(QString,int,QString)),this,SLOT(initPortal(QString,int,QString)));
    m_conMod.setModal(true);
    m_conMod.exec();
}
void MainWindow::showTxModal(){
    txModal m_txMod(this,ui->txIndicator->text().toInt());
    connect(&m_txMod,SIGNAL(txSetSig(int)),this,SLOT(txSetPrompt(int)));
    m_txMod.setModal(true);
    m_txMod.exec();
}
void MainWindow::showHzModal(){
    hzModal m_hzMod(this,ui->hzIndicator->text().toInt());
    connect(&m_hzMod,SIGNAL(hzSetSig(int)),this,SLOT(hzSetPrompt(int)));
    m_hzMod.setModal(true);
    m_hzMod.exec();
}
void MainWindow::setConnLED(bool val){
    val ? ui->connLED->setPixmap(QPixmap(":/Resources/green.png")) : ui->connLED->setPixmap(QPixmap(":/Resources/red.png"));
}

void MainWindow::setSyncLED(bool val){
    ui->syncLED->setEnabled(true);
    val ? ui->syncLED->setPixmap(QPixmap(":/Resources/gray.png"))  : ui->syncLED->setPixmap(QPixmap(":/Resources/red.png")) ;
}

void MainWindow::setBaudDis(int val){
    ui->baudDis->setText(QString::number(val));
}

void MainWindow::setTxPower(int val){
    ui->txIndicator->setText(QString::number(val));
    guiSlaveTimer.restart();
    //ui->txIndicator->setStyleSheet("QLabel { color : green; }");
    //while(Q)
}

void MainWindow::sethz(int val){
    ui->hzIndicator->setText(QString::number(val));
}
void MainWindow::setModemTemp(int val){
    ui->modemTempIndicator->setText(QString::number(val));
}
void MainWindow::setOutputPower(int val){
    ui->outputPowerIndicator->setText(QString::number(val));
}
void MainWindow::setPowerAmpTemp(int val){
    ui->powerAmpTempIndicator->setText(QString::number(val));
}
void MainWindow::setPowerAmpStatus(QString val){
    ui->powerAmpStatusIndicator->setText(val);
}
void MainWindow::setIndicator(QString val){
    ui->indicator->setText(val);
}
void MainWindow::connectionStatus(bool connection){
    if(connection){
       ui->indicator->setText("Connection established.");
       ui->indicator->setStyleSheet("QLabel { color : green; }");
       ////ui->tabwidget_2->setVisible(true);
       setConnLED(true);
    }else{
        ui->indicator->setText("Connection failure.");
        ui->indicator->setStyleSheet("QLabel { color : red; }");
        ////ui->tabwidget_2->setVisible(false);
        setConnLED(false);
    }
}
void MainWindow::packetStatus(bool connection,QString val){
    connection ? ui->procIndicator->setStyleSheet("QLabel { color : green; }")  : ui->procIndicator->setStyleSheet("QLabel { color : red; }");
    connection ? setSyncLED(true)  : setSyncLED(false);
    ui->procIndicator->setText(val);
}
void MainWindow::initPortal(QString setting,int baudrate,QString parity){
    m_portal=new portal(this);
    //m_processor=new processor(this,&m_portal);
    connect(m_portal,SIGNAL(connectionStatusSig(bool)),this,SLOT(connectionStatus(bool)));
    connect(m_portal,SIGNAL(packetStatusSig(bool,QString)),this,SLOT(packetStatus(bool,QString)));
    connect(m_portal,SIGNAL(syncCallSig(QByteArray)),this,SLOT(syncCall(QByteArray)));
    connect(m_portal,SIGNAL(devoidSig()),this,SLOT(devoid()));
    m_portal->openPort(setting,baudrate,parity);
    m_flagInitiated=true;
}
void MainWindow::quitPortal(){
    if(m_flagInitiated){
    m_portal->closePort();
    m_portal->quit();
    packetStatus(false,"Connection is void.");
    //m_portal->thread()->quit();
    }else{
        packetStatus(false,"You have not initiated a connection!");
    }
}
void MainWindow::devoid(){
    ui->configBox->setEnabled(true);
}
void MainWindow::syncCall(QByteArray data){
    switch ((quint8)data[2]) {
        case hzSet:
            sethz(quint8(data[3])*256+quint8(data[4]));
            break;
        case hzInc:
            sethz(quint8(data[3])*256+quint8(data[4]));
            break;
        case hzDec:
            sethz(quint8(data[3])*256+quint8(data[4]));
            break;
        case txSet:
            setTxPower(quint8(data[3]));
            break;
        case txDec:
            setTxPower(quint8(data[3]));
            break;
        case txInc:
            setTxPower(quint8(data[3]));
            break;
        case reqStatus:
            setTxPower(quint8(data[4]));
            sethz(quint8(data[5])*256+quint8(data[6]));
            //setBaudDis(int(data[7]));
            setModemTemp(quint8(data[8]));
            setOutputPower((quint8(data[9])*256+quint8(data[10]))/100);
            setPowerAmpTemp(quint8(data[11]));
            if(int(data[12])!=0){
                setPowerAmpStatus("Error: Code (" + QString::number(int(data[12])) + ")");
            }else{
                setPowerAmpStatus("OK");
            }
            if(quint8(data[7])==0x01){
                ui->resetBut->setStyleSheet("background-color: #f57505;border-style: outset;border-width: 2px;border-radius: 10px;border-color: black;font: bold 14px;min-width: 8em;min-height: 1.75em;padding: 6px;");
            }else{
                ui->resetBut->setStyleSheet("background-color: #fbf7d9;border-style: outset;border-width: 2px;border-radius: 10px;border-color: black;font: bold 14px;min-width: 8em;min-height: 1.75em;padding: 6px;");
            }
        break;

    }
}
void MainWindow::txSetPrompt(int val){
    QByteArray data;
    data.append(firstChk);
    data.append(secondChk);
    data.append(txSet);
    data.append(quint8(val));
    emit initReqSig(data);
    m_portal->bodyReq(data);
}
void MainWindow::txIncPrompt(){
    QByteArray data;
    data.append(txInc);
    int val = ui->txIndicator->text().toInt();
    ui->txIndicator->setText(QString::number(val+1));
    m_portal->bodyReq(data);
}
void MainWindow::txDecPrompt(){
    QByteArray data;
    data.append(txDec);
    int val = ui->txIndicator->text().toInt();
    ui->txIndicator->setText(QString::number(val-1));
    m_portal->bodyReq(data);
}
void MainWindow::hzSetPrompt(int val){
    QByteArray data;
    data.append(firstChk);
    data.append(secondChk);
    data.append(hzSet);
    data.append((val >> 8) & 0xFF);
    data.append(val & 0xFF);
    m_portal->bodyReq(data);

}
void MainWindow::hzIncPrompt(){
    QByteArray data;
    data.append(hzInc);
    int val = ui->txIndicator->text().toInt();
    ui->txIndicator->setText(QString::number(val+1));
    m_portal->bodyReq(data);
}
void MainWindow::hzDecPrompt(){
    QByteArray data;
    data.append(hzDec);
    int val = ui->txIndicator->text().toInt();
    ui->txIndicator->setText(QString::number(val-1));
    m_portal->bodyReq(data);
}
void MainWindow::resetPrompt(){
    QByteArray data;
    data.append(firstChk);
    data.append(secondChk);
    data.append(resModem);
    m_portal->bodyReq(data);
}


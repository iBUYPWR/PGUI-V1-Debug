#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QtSerialPort/QSerialPort>
#include <QStringList>
#include <QTextStream>
#include "connectionmodal.h"
#include "txmodal.h"
#include "hzmodal.h"
#include "serialportwriter.h"
#include "serialportreader.h"
#include "worker.h"
#include <QDebug>
#include "iostream"
#include "portal.h"
#include "processor.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void showTxModal();
    void showHzModal();
    void showConnectionModal();
    void setSyncLED(bool val);
    void setBaudDis(int val);
    void setConnLED(bool val);
    void setTxPower(int val);
    void sethz(int val);
	void setModemTemp(int val);
	void setOutputPower(int val);
	void setPowerAmpTemp(int val);
    void setPowerAmpStatus(QString val);
    void connectionStatus(bool kk);
    void setIndicator(QString val);
    void packetStatus(bool kk,QString val);
    void initPortal(QString setting,int baudrate,QString parity);
    void quitPortal();
    void txSetPrompt(int val);
    void hzSetPrompt(int val);
    void txIncPrompt();
    void txDecPrompt();
    void hzDecPrompt();
    void hzIncPrompt();
    void resetPrompt();
    void syncCall(QByteArray);
    void devoid();
signals:
    void initReqSig(QByteArray);

private:
    portal *m_portal;
    processor *m_processor;
    Ui::MainWindow *ui;
    bool m_flagInitiated = false;
    QElapsedTimer guiSlaveTimer;
};
#endif // MAINWINDOW_H

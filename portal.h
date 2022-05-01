#ifndef PORTAL_H
#define PORTAL_H
#include "QtSerialPort/QSerialPort"
#include "QTimer"
#include "QThread"
#include "QObject"
#include <QByteArray>
#include <QQueue>
#include <QMutex>
#include <QDateTime>
#include <QMap>
#include <QVector>
#include "newtype.h"
#include "QElapsedTimer"
class portal: public QThread
{
    Q_OBJECT
public:
    portal(QObject *parent = nullptr);
    void openPort(QString,int,QString);
    void closePort();
    void mergePacket();
    void unleashPackets(QByteArray body);
    void initReq();
    void bodyReq(QByteArray body);
    quint8 makeChecksum(QByteArray data);
    QMutex  addNewReadDataMutex;
    void run() Q_DECL_OVERRIDE;
    QSerialPort *m_serialPort;
    State_Type	mode;
    State_Type	state;
    Prompt_Type	lastPr;
    Prompt_Type	conditionPr;
    Flag_State 	flagL;
    Prompt_Waiting_Type waitingResponse;
    QQueue<QByteArray> motherPacket;
    bool silencer();
    int makeCheckSum(QByteArray data);
    QTimer *t_timer;
public slots:
    void writeData(QByteArray data);
private slots:
    void handleReadyRead();
    void regularCheck();

signals:
    void packetStatusSig(bool,QString);
    void connectionStatusSig(bool);
    void syncCallSig(QByteArray);
    void devoidSig();
    void writeSig(QByteArray data);
private:
    QQueue<QByteArray>			m_recieveBufferQueue;
    QMutex                      m_receiveBufferMutex;
    QString		settingPort;
    int		settingbaudrate;
    QString		settingparity;
    bool		done;
    bool		m_flagClosePort;
    bool		m_flagSkipNextReq;
    bool		m_flagDataReady;
    quint8      timerRefreshMonitoringCounter;
    QByteArray	receiveData;
    QByteArray	receivePacket;
    int		responseCounter = 0;
    QByteArray data;
    QByteArray splittedQueue;
    QElapsedTimer timer;
    QElapsedTimer silence_timer;
    QElapsedTimer general_timer;
    QElapsedTimer response_timer;
    //QElapsedTimer t_timer;
};
#endif // PORTAL_H

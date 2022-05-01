#ifndef PROCESSOR_H
#define PROCESSOR_H
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
#include "portal.h"
class processor: public QThread
{
    Q_OBJECT
public:
    processor(QObject *parent = nullptr,portal *h_portal=nullptr);
    void openPort(QString,int,QString);
    void closePort();
    void mergePacket();
    void unleashPackets(QByteArray body);
    void initReq();
    void bodyReq(QByteArray body);
    quint8 makeChecksum(QByteArray data);
    void writeData(QByteArray data);
    QMutex  addNewReadDataMutex;
    void run() Q_DECL_OVERRIDE;
    QSerialPort *m_serialPort;
    State_Type	state;
    Prompt_Type	lastPr;
    Prompt_Type tempState=empty;
    QByteArray motherPacket;
    bool silencer();
    int makeCheckSum(QByteArray data);
    QTimer *t_timer;
private slots:
    void handleReadyRead();
    void regularCheck();

signals:
    void packetStatusSig(bool,QString);
    void connectionStatusSig(bool);
    void syncCallSig(QByteArray);
    void devoidSig();
private:
    QQueue<QByteArray>			m_recieveBufferQueue;
    QString		settingPort;
    int		settingbaudrate;
    QString		settingparity;
    bool		done;
    int			m_maxPacketSize;
    bool		m_flagClosePort;
    bool		m_flagSkipNextReq;
    bool		m_flagResponding;
    int		responseCounter = 0;
    QByteArray	receiveData;
    QByteArray	receivePacket;
    QByteArray data;
    QByteArray splittedQueue;
    QElapsedTimer timer;
    QElapsedTimer general_timer;
    QElapsedTimer response_timer;
    //QElapsedTimer t_timer;
};
#endif // PROCESSOR_H

#include "portal.h"
#include <qdebug.h>
#include <QElapsedTimer>
portal::portal(QObject *parent)
: QThread(parent)
{
    state=voided;
    t_timer = new QTimer(0);
    t_timer->moveToThread(this->thread());
    connect(t_timer, SIGNAL(timeout()), this, SLOT(regularCheck()));
    connect(this, SIGNAL(writeSig(QByteArray)), this, SLOT(writeData(QByteArray)));
}
void portal::run()
{
    Loop_State loopState = idleAck;
    QByteArray compiledData;
    int bytesToProcess=0;
    while(!done){
        if(state!=voided && !m_flagSkipNextReq){
            //qDebug() << "Doing regular check pass 1" ;
            if(response_timer.elapsed()>=1500 && general_timer.elapsed()>=1500){
                ///qDebug() << general_timer.elapsed();
                emit packetStatusSig(false,"No response from the Radio and the connection seems void.");
                state=voided;
            } else if (response_timer.elapsed()>=1500 && general_timer.elapsed()<=1500){
                emit packetStatusSig(false,"Packets are detected on line, but no response from the radio");
            }
        }
        if(m_receiveBufferMutex.tryLock()){
            if(m_recieveBufferQueue.size() > 0){
            data = m_recieveBufferQueue.dequeue();
            m_receiveBufferMutex.unlock();
            //int length = data.length();
            //QByteArray noCheckSum = data.left(length-2);
            for (int i = 0; i < data.size(); i++){
            //if((quint8)data[0]==firstChk && (quint8)data[1]==secondChk){
                //qDebug() << "signature true" ;
                switch (loopState) {
                    case idleAck:
                        bytesToProcess=0;
                        compiledData.clear();
                        //flagL=awaitingActivity;
                        if((quint8)data[i]==firstChk){
                            loopState=sigAck1;
                            compiledData.append(data[i]);
                        }else{
                            flagL= awaitingActivity;
                            loopState=idleAck;
                        }
                        break;
                    case sigAck1:
                        if((quint8)data[i]==secondChk){
                            loopState=sigAck2;
                            compiledData.append(data[i]);
                        }else{
                            flagL= awaitingActivity;
                            loopState=idleAck;
                        }
                        break;
                    case sigAck2:
                        compiledData.append(data[i]);
                        switch ((quint8)data[i]) {
                            case hzSet:
                                bytesToProcess=7;
                                lastPr=hzSetDone;
                                loopState=commandAck;
                                break;
                            case hzInc:
                                bytesToProcess=7;
                                lastPr=hzIncDone;
                                loopState=commandAck;
                                break;
                            case hzDec:
                                bytesToProcess=7;
                                lastPr=hzDecDone;
                                loopState=commandAck;
                                break;
                            case txSet:
                                bytesToProcess=6;
                                lastPr=txSetDone;
                                loopState=commandAck;
                                break;
                            case txDec:
                                bytesToProcess=6;
                                lastPr=txDecDone;
                                loopState=commandAck;
                                break;
                            case txInc:
                                bytesToProcess=6;
                                lastPr=txSetDone;
                                loopState=commandAck;
                                break;
                            case reqStatus:
                                bytesToProcess=22;
                                lastPr=wDone;
                                loopState=commandAck;
                                response_timer.restart();
                                emit packetStatusSig(true,"Connection is healthy.");
                                break;
                            default:
                                flagL= awaitingActivity;
                                loopState=idleAck;
                                break;
                        }
                        if((quint8)data[i]!=conditionPr)flagL=initial;
                        break;
                    case commandAck:
                        if(bytesToProcess - 5 > 0){
                            compiledData.append(data[i]);
                            bytesToProcess--;
                            loopState=commandAck;
                        }else{
                            i--; //To reprocess the same value
                            loopState=dataAck;
                            flagL= awaitingActivity;
                        }
                        break;
                    case dataAck:
                        if(((makeCheckSum(compiledData)>> 8)& 0xFF)==(quint8)data[i]){
                            loopState=msbAck;
                        }else{
                            loopState=idleAck;
                            flagL=awaitingActivity;
                            i--; //To reprocess the same value
                        }
                        break;
                    case msbAck:
                        if((makeCheckSum(compiledData) & 0xFF)==(quint8)data[i]){
                            loopState=idleAck;
                            flagL=completed;
                            responseCounter=0;
                            emit syncCallSig(compiledData);
                        }else{
                            loopState=idleAck;
                            flagL=awaitingActivity;
                            i--; //To reprocess the same value
                        }
                        break;
                    case lsbAck:
                        loopState=idleAck;
                        break;
                    }
                }
                qDebug() << "loop" << loopState << "flag" << flagL;
                if(loopState!=idleAck)flagL=awaitingCompletion; // States left midway
                if(flagL== awaitingResponse){
                    responseCounter++;
                    if(responseCounter>=2){ // Ignore 1 packet in awaitingResponse
                        flagL=awaitingActivity; // No answer; Get to the first state
                        responseCounter=0;
                        qDebug() << "response dropped / switching to first state";
                    }
                }
                if(flagL == initial || flagL== awaitingActivity){
                qDebug() << "passed" << data.toHex() << flagL;
                if(motherPacket.size()>0) {
                    QByteArray rawData=motherPacket.dequeue();
                    unleashPackets(rawData);
                    switch((quint8)rawData[0]){
                        case txSet:
                            conditionPr=txSetDone;
                            flagL=awaitingResponse;
                        case txInc:
                            conditionPr=txIncDone;
                            flagL=awaitingResponse;
                        case txDec:
                            conditionPr=txDecDone;
                            flagL=awaitingResponse;
                        case hzSet:
                            conditionPr=hzSetDone;
                            flagL=awaitingResponse;
                        case hzInc:
                            conditionPr=hzIncDone;
                            flagL=awaitingResponse;
                        case hzDec:
                            conditionPr=hzDecDone;
                            flagL=awaitingResponse;
                        case resModem:
                            flagL=awaitingActivity;
                    }

                    //switch((quint8)motherPacket[2]){
                }else{
                    initReq(); // Might cause lag
                    conditionPr=wDone;
                    flagL=awaitingResponse;
                }

                }
                if(flagL==completed)flagL=awaitingActivity;

            }else{
                m_receiveBufferMutex.unlock();
                msleep(1);
            }
         }else{
            msleep(5);
        }
    }
}

void portal::openPort(QString setting,int baudrate,QString parity){
    m_serialPort = new QSerialPort();
    m_serialPort->setPortName(setting);
    m_serialPort->setBaudRate(baudrate);
    //m_serialPort->setDataBits(QSerialPort::Data8);
    //m_serialPort->setStopBits(QSerialPort::OneStop);
    //m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
    if (parity == "None"){
        m_serialPort->setParity(QSerialPort::NoParity);
    }
    else if (parity == "Odd"){
        m_serialPort->setParity(QSerialPort::OddParity);
    }
    try
    {
        if (m_serialPort->open(QIODevice::ReadWrite))
        {
            emit connectionStatusSig(true);
            emit packetStatusSig(false,"Connection is void.");
            m_flagClosePort = false;
            done = false;
            connect(m_serialPort, SIGNAL(readyRead()), this, SLOT(handleReadyRead()));
            connect(m_serialPort,&QSerialPort::bytesWritten, this, [](const qint64 bytes) {
                ///qDebug() << "Sent data: " << bytes << " bytes.";
            });
            //t_timer->start(1000);
            general_timer.start();
            response_timer.start();
            if (!isRunning())
            {
                start();
            }
        }
        else
        {
            done = true;
            m_flagClosePort = true;
            emit connectionStatusSig(false);
        }
    }
    catch (...)
    {

    }
}
void portal::closePort()
{

    if (m_serialPort->isOpen()){
        m_serialPort->close();
	}
    done = true;
    m_flagClosePort = false;
    emit connectionStatusSig(false);
}

void portal::handleReadyRead(){
    QByteArray handleData=m_serialPort->readAll();
    qDebug() << "data:" << handleData.toHex() << handleData.size();
    //qDebug() << silence_timer.elapsed();
    general_timer.restart();
    silence_timer.restart();
    m_receiveBufferMutex.lock();
    m_recieveBufferQueue.enqueue(handleData);
    m_receiveBufferMutex.unlock();
    if(state==voided){
        emit devoidSig();
        state=silence;
    }

}

void portal::regularCheck(){
    qDebug() << "Doing regular check" <<general_timer.elapsed()<< response_timer.elapsed();

}
void portal::writeData(QByteArray data)
{
    //qint64 bytesWritten =
    m_serialPort->write(data);

}
int portal::makeCheckSum(QByteArray data){
	int sum = 0;
	
	for (int i = 0; i < data.count(); i++)
	{
		sum += quint8(data[i]);
	}
	return sum;
}
bool portal::silencer(){
    silence_timer.restart();
    int x=0;
    while(silence_timer.elapsed() < 50){
        x=silence_timer.elapsed();
        if(x>silence_timer.elapsed()){
            qDebug() << "reset" << x;
        }
        if(silence_timer.elapsed()>=49){
            ///qDebug() <<"macdo"<< silence_timer.elapsed();
            return true;
        }
    }
    return false;
}
void portal::unleashPackets(QByteArray body){
    if(silencer()){
        emit writeSig(body);
        qDebug() << "written" << body.toHex();
    }
}
void portal::initReq(){

    lastPr=wRes;
    //send a simple request, to fetch status
    QByteArray data;
    data.append(firstChk);
    data.append(secondChk);
    data.append(reqStatus);
    int checkSum=makeCheckSum(data);
    data.append((checkSum >> 8)& 0xFF);
    data.append(checkSum & 0xFF);
    ///qDebug() << "initReq" << data.toHex();
    if(silencer()){emit writeSig(data);}
}

void portal::bodyReq(QByteArray body){
    int checkSum=makeCheckSum(body);
    body.append((checkSum >> 8)& 0xFF);
    body.append(checkSum & 0xFF);
    motherPacket.enqueue(body);
}
		

#include "processor.h"
#include <qdebug.h>
#include <QElapsedTimer>
processor::processor(QObject *parent,portal *h_portal)
: QThread(parent)
{
    state=voided;
    t_timer = new QTimer(0);
    t_timer->moveToThread(this->thread());
    connect(t_timer, SIGNAL(timeout()), this, SLOT(regularCheck()));


}
void processor::run()
{
}

void processor::openPort(QString setting,int baudrate,QString parity){
    m_serialPort = new QSerialPort();
    m_serialPort->setPortName(setting);
    m_serialPort->setBaudRate(baudrate);
    //m_serialPort->setDataBits(QSerialPort::Data8);
    if (parity == "None"){
        m_serialPort->setParity(QSerialPort::NoParity);
    }
    else if (parity == "Odd"){
        m_serialPort->setParity(QSerialPort::OddParity);
        //m_serialPort->setStopBits(QSerialPort::OneStop);
        //m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
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
                qDebug() << "Sent data: " << bytes << " bytes.";
            });
            t_timer->start(3000);
            general_timer.start();
            response_timer.start();
            //m_timerRefreshMonitoring->start(1000);
            if (!isRunning())
            {
                start();
            }
        }
        else
        {
            //m_timerRefreshMonitoring->stop();
            done = true;
            m_flagClosePort = true;
            emit connectionStatusSig(false);
        }
    }
    catch (...)
    {

    }
}
void processor::closePort()
{

    if (m_serialPort->isOpen()){
        m_serialPort->close();
	}
    done = true;
    m_flagClosePort = false;
    emit connectionStatusSig(false);
}

void processor::handleReadyRead(){
    if(splittedQueue!=""){
        splittedQueue.append(m_serialPort->readAll());
        data=splittedQueue;
        splittedQueue="";
        qDebug() << "split:" << data;
    }else{
    data=m_serialPort->readAll();
    }
    qDebug() << "data:" << data.toHex() << data.size();
    if(data.length()<2) return;
    if(state==voided){
        emit devoidSig();
        state=silence;

    }
    int length = data.length();
    QByteArray noCheckSum = data.left(length-2);
    int msb=(quint8)data[length-2];
    int lsb=(quint8)data[length-1];
    if((quint8)data[0]==firstChk && (quint8)data[1]==secondChk){
        //qDebug() << "signature true" ;
        int dividerLength=0;
        switch ((quint8)data[2]) {
            case hzSet:
                dividerLength=7;
                break;
            case hzInc:
                dividerLength=7;
                break;
            case hzDec:
                dividerLength=7;
                break;
            case txSet:
                dividerLength=6;
                break;
            case txDec:
                dividerLength=6;
                break;
            case txInc:
                dividerLength=6;
                break;
            case reqStatus:
                dividerLength=22;
                break;
        }
        if(data.length()==dividerLength){
            if(((makeCheckSum(noCheckSum)>> 8)& 0xFF)==msb && (makeCheckSum(noCheckSum) & 0xFF)==lsb){
                //qDebug() << "check true";
                emit syncCallSig(data);
                switch ((quint8)data[2]) {
                    case hzSet:
                        lastPr=hzSetDone;
                        break;
                    case hzInc:
                        lastPr=hzIncDone;
                        break;
                    case hzDec:
                        lastPr=hzDecDone;
                        break;
                    case txSet:
                        lastPr=txSetDone;
                        break;
                    case txDec:
                        lastPr=txDecDone;
                        break;
                    case txInc:
                        lastPr=txSetDone;
                        break;
                    case reqStatus:
                        //qDebug() << "wDone";
                        response_timer.restart();
                        emit packetStatusSig(true,"Connection is healthy.");
                        lastPr=wDone;
                    break;

                }
            }else{emit packetStatusSig(false,"Checksum Failed");}
        }else{splittedQueue.append(data);qDebug() << "split happened" << splittedQueue;}
    }
    general_timer.restart();
    //qDebug() << "mother" << motherPacket.toHex();
    motherPacket!="" ? unleashPackets(motherPacket) : initReq();
    motherPacket="";
}

void processor::regularCheck(){
    qDebug() << "Doing regular check" <<general_timer.elapsed()<< response_timer.elapsed();
    if(state!=voided && !m_flagSkipNextReq){
        //qDebug() << "Doing regular check pass 1" ;
        if(response_timer.elapsed()>=2000 && general_timer.elapsed()>=2000){
            qDebug() << general_timer.elapsed();
            emit packetStatusSig(false,"No response from the Radio and the connection seems void.");
            state=voided;
        } else if (response_timer.elapsed()>=2000 && general_timer.elapsed()<=2000){
            emit packetStatusSig(false,"Packets are detected on line, but no response from the radio");
        }
    }
}
void processor::writeData(QByteArray data)
{
    //qint64 bytesWritten =
    qint64 bytesWritten =m_serialPort->write(data);
    qDebug() << "bytesWritten" << bytesWritten;
}
int processor::makeCheckSum(QByteArray data){
	int sum = 0;
	
	for (int i = 0; i < data.count(); i++)
	{
		sum += quint8(data[i]);
	}
    //qDebug() << sum;
	return sum;
}
bool processor::silencer(){
    timer.restart();
    //int i =0;
    while(timer.elapsed() < 11){
        if(state==silence){
            //qDebug() << "Silence:" << i;
            //i++;
		}
        else{
            timer.restart();
            //i=0;
		}
        if(timer.elapsed()>=10){
            return true;
		}
	}
    return false;
}
void processor::unleashPackets(QByteArray body){
    if(silencer()){writeData(body);
    qDebug() << "written" << body.toHex();
    }
    /*
    //m_flagSkipNextReq=true;
    for(int i=0;i<2;i++){
        //emit packetStatusSig(false,"Prompting for response" + (QString)i);
        qDebug() << "Prompting for response" + (QString)i;
        initReq();
        timer.restart();
        //while(timer.elapsed() < 500){
            //msleep(500);
            //qDebug() << "last state is:" << lastPr;
        //if(lastPr==wRes){
            //counter for catching error
            //i++;
            //}else if(lastPr==wDone){
            //successful; initiate packet launch
            //qDebug() << "wDone done";
            if(body!=nullptr){qDebug() << "body initialized";bodyReq(body);}
            m_flagSkipNextReq=false;
            return true;
        //}
    }
    m_flagSkipNextReq=false;
    return false;
    */
}
void processor::initReq(){

    lastPr=wRes;
    //send a simple request, to fetch status
    QByteArray data;
    data.append(firstChk);
    data.append(secondChk);
    data.append(reqStatus);
    int checkSum=makeCheckSum(data);
    data.append((checkSum >> 8)& 0xFF);
    data.append(checkSum & 0xFF);
    qDebug() << "initReq" << data.toHex();
    if(silencer()){writeData(data);}
}

void processor::bodyReq(QByteArray body){
    /*
    Prompt_Type tempState=empty;
	switch (quint8(body[2])) {
		case hzSet:
            tempState=hzSetDone;
            break;
		case hzInc:
            tempState=hzIncDone;
            break;
		case hzDec:
            tempState=hzDecDone;
            break;
		case txSet:
            tempState=txSetDone;
            break;
        case txDec:
            tempState=txDecDone;
            break;
        case txInc:
            tempState=txSetDone;
            break;
	}
	if(silencer()){writeData(body);}
    //int i=0;
    timer.restart();
    while (timer.elapsed() <6){
        if(lastPr!=tempState){
            //i++;
        }
        if(timer.elapsed()==5){
            emit packetStatusSig(false,"No response from the Radio (main body).");
        }
    }
    */
    int checkSum=makeCheckSum(body);
    body.append((checkSum >> 8)& 0xFF);
    body.append(checkSum & 0xFF);
    motherPacket=body;
}
		

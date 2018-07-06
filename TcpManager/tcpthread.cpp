#include "tcpthread.h"

#include <QDebug>
#include <QTimer>
#include <QByteArray>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QAbstractSocket>
char READSIZE = 16;

char HEAD_L  = 0xAA;
char HEAD_H  = 0x00;

char HEAD_AA = 0;
char HEAD_00 = 1;
char CMD     = 2;
char NET     = 3;
char ADD_H   = 4;
char ADD_L   = 5;
char LONG    = 6;
char TYPE    = 7;
char STATE   = 8;
char CUR_L   = 9;
char CUR_H   = 10;
char BASE_L  = 11;
char BASE_H  = 12;
char ALA_L   = 13;
char ALA_H   = 14;
char CRC     = 15;



TcpThread::TcpThread(QObject *parent) : QObject(parent)
{

}

TcpThread::TcpThread(QString host, quint16 port)
{
    m_host = host;  m_port = port;
    m_isOkConect = false;

    m_tcpSocket  = new QTcpSocket;
    initConnect();
}

TcpThread::~TcpThread()
{
    delete m_tcpTimer;
    delete m_tcpSocket;
}

void TcpThread::initConnect()
{
    m_tcpTimer = new QTimer;
    connect(m_tcpTimer,&QTimer::timeout,this,&TcpThread::slotConnectTimeOut);
    m_tcpTimer->start(5000);

    m_tcpCmd = new QTimer;
    connect(m_tcpCmd,&QTimer::timeout,this,&TcpThread::slotTcpCmd);


    connect(m_tcpSocket,&QTcpSocket::readyRead,this,&TcpThread::slotReceiveData);
    connect(m_tcpSocket,&QTcpSocket::disconnected,this,&TcpThread::slotDisconnect);
    connect(m_tcpSocket,&QTcpSocket::connected,this,&TcpThread::slotConnectSuccess);

    connect(m_tcpSocket,QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            [=](QAbstractSocket::SocketError socketError){
        qDebug()<<"socketError : "<<socketError;
    });

}

bool TcpThread::checkSum(QByteArray byteArray)
{
    char crcSum = 0;
    for(int i = 2; i < byteArray.count()-1;i++)
    {
        crcSum += byteArray.at(i);
    }
    if(crcSum == byteArray.at(15))
    {
        return true;
    }
    return false;
}

void TcpThread::parseDate(QByteArray byteArray)
{

    uint pass = byteArray.at(NET);
    uint canId = byteArray.at(ADD_H) | byteArray.at(ADD_L);

    uint state = byteArray.at(STATE);

    uint type  = byteArray.at(TYPE);
    if(type == 2)//漏电
    {
        uint curLeak   = (uchar)byteArray.at(CUR_L) | (uchar)byteArray.at(CUR_H) << 8;
        uint baseValue = (uchar)byteArray.at(BASE_L)| (uchar)byteArray.at(BASE_H)<< 8;
        uint alarmSet  = (uchar)byteArray.at(ALA_L) | (uchar)byteArray.at(ALA_H) << 8;
        emit sigNodeUpdate(pass,canId,type,state,curLeak,baseValue,alarmSet);
    }
    else
    {
        uint curTemp = byteArray.at(CUR_L);
        uint tempSet = byteArray.at(ALA_L);
        emit sigNodeUpdate(pass,canId,type,state,curTemp,0,tempSet);
    }

}

void TcpThread::slotConnectSuccess()
{
    qDebug()<<"slotConnectSuccess";
    //连接成功
    m_tcpCmd->start(200);
    m_tcpTimer->stop();
    m_isOkConect = true;
    emit sigConnectStatus(m_isOkConect);
}

void TcpThread::slotDisconnect()
{
    qDebug()<<"slotDisconnect";
    m_isOkConect = false;
    m_tcpCmd->stop();
    m_tcpTimer->start(5000);
    emit sigConnectStatus(m_isOkConect);
}

//#define DEBUG

void TcpThread::slotReceiveData()
{

    if(m_tcpSocket->bytesAvailable() == READSIZE)
    {
        QByteArray byteArray = m_tcpSocket->readAll();

#ifdef  DEBUG
        for(int i = 0; i < byteArray.size();i++)
        {
            qDebug()<<"byteArray["<<i<<"]:"<<(uchar)byteArray.at(i);
        }
        qDebug()<<"******************************";
#endif

        //判断头帧和校验和
        if(byteArray.at(HEAD_AA) == HEAD_L && byteArray.at(HEAD_00) == HEAD_H && true == checkSum(byteArray))
        {
            //数据包解析
            parseDate(byteArray);
        }
    }

}

void TcpThread::slotConnectTimeOut()
{
    qDebug()<<"slotTcpTimeOut";
    if(!m_isOkConect)//未连接成功后自动连接
    {
        m_tcpSocket->connectToHost(m_host,m_port);
    }
}

void TcpThread::slotTcpCmd()
{
    QByteArray ba;
    ba.append(0x0A);
    ba.append(0x01);
    m_tcpSocket->write(ba);
}



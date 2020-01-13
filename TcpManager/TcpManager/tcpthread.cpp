#include "tcpthread.h"

#include <QDebug>
#include <QTimer>
#include <QByteArray>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QAbstractSocket>


char READSIZE = 16;
char HEAD_L  = 0xAA;
char HEAD_H  = 0x00;



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



void TcpThread::analysisDate(QByteArray byteArray)
{
    qreal ai1;  qreal _ai_int;  qreal _ai_dec;
    qreal bi1;  qreal _bi_int;  qreal _bi_dec;
    qreal ci1;  qreal _ci_int;  qreal _ci_dec;

    uint av1;    uint av2;
    uint bv1;    uint bv2;
    uint cv1;    uint cv2;

    uint pass = byteArray.at(PASS);
    uint canId= byteArray.at(CANID);
    uint type = byteArray.at(TYPE);
    uint state= byteArray.at(STS);

    switch (type) {
    case MOD_V:
    case MOD_V3://三项双路无零
    case MOD_VN3://三项双路有零

        av1 = byteArray.at(AV_1)*2;
        bv1 = byteArray.at(BV_1)*2;
        cv1 = byteArray.at(CV_1)*2;

        av2 = byteArray.at(AV_2)*2;
        bv2 = byteArray.at(BV_2)*2;
        cv2 = byteArray.at(CV_2)*2;

        emit sigModUpdate(pass,canId,type,state,av1,bv1,cv1,av2,bv2,cv2,0,0,0);
        break;
    case MOD_VA:
    case MOD_VA3: //电压电流无零
    case MOD_VAN3://电压电流有零

        av1 = byteArray.at(AV_1)*2;
        bv1 = byteArray.at(BV_1)*2;
        cv1 = byteArray.at(CV_1)*2;

        //整数部分
        _ai_int = byteArray.at(AI_1) >> 4;
        _bi_int = byteArray.at(BI_1) >> 4;
        _ci_int = byteArray.at(CI_1) >> 4;
        //小数部分
        _ai_dec = byteArray.at(AI_1) & 0x0F;
        _bi_dec = byteArray.at(BI_1) & 0x0F;
        _ci_dec = byteArray.at(CI_1) & 0x0F;
        //整数+小数
        ai1 = _ai_int + _ai_dec / 10;
        bi1 = _bi_int + _bi_dec / 10;
        ci1 = _ci_int + _ci_dec / 10;

        emit sigModUpdate(pass,canId,type,state,av1,bv1,cv1,0,0,0,ai1,bi1,ci1);
        break;
    case MOD_2VAN3://两路三项电压一路三项电流

        av1 = byteArray.at(AV_1)*2;
        bv1 = byteArray.at(BV_1)*2;
        cv1 = byteArray.at(CV_1)*2;

        av2 = byteArray.at(AV_2)*2;
        bv2 = byteArray.at(BV_2)*2;
        cv2 = byteArray.at(CV_2)*2;

        //整数部分
        _ai_int = byteArray.at(AI_1) >> 4;
        _bi_int = byteArray.at(BI_1) >> 4;
        _ci_int = byteArray.at(CI_1) >> 4;
        //小数部分
        _ai_dec = byteArray.at(AI_1) & 0x0F;
        _bi_dec = byteArray.at(BI_1) & 0x0F;
        _ci_dec = byteArray.at(CI_1) & 0x0F;
        //整数+小数
        ai1 = _ai_int + _ai_dec / 10;
        bi1 = _bi_int + _bi_dec / 10;
        ci1 = _ci_int + _ci_dec / 10;

        emit sigModUpdate(pass,canId,type,state,av1,bv1,cv1,av2,bv2,cv2,ai1,bi1,ci1);
        break;
    default:
        break;
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

#define DEBUG

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
            //parseDate(byteArray);
            analysisDate(byteArray);
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



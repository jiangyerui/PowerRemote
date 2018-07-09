#ifndef TCPTHREAD_H
#define TCPTHREAD_H

#include <QObject>
#include "tcpmanager.h"
class QTimer;
class QTcpSocket;

#define NORMAL          0x00//正常
#define OVERCURRENT     0x01//过流
#define PHASELOSS       0x02//错相
#define OVERVOLTAGE     0x03//过压
#define UNDERVOLTAGE    0x04//欠压
#define INTERRUPTION    0x05//供电中断
#define CANERROR        0x06//通讯中断


#define MOD_NONE    1//无类型
#define MOD_V3      2//双路三相电压型
#define MOD_V       3//六路单相电压型
#define MOD_VA3     4//电压电流型
#define MOD_DVA     5//直流电压电流型
#define MOD_VA      6//单项电压电流
#define MOD_VN3     7//三项双路有零
#define MOD_VAN3    8//电压电流有零
#define MOD_2VAN3   9//两路三项电压一路三项电流

#define HEAD_AA 0
#define HEAD_00 1
#define PASS    2
#define CANID   3
#define TYPE    4
#define STS     5
//电压1
#define AV_1    6
#define BV_1    7
#define CV_1    8
//电压2
#define AV_2    9
#define BV_2    10
#define CV_2    11
//电流1
#define AI_1    12
#define BI_1    13
#define CI_1    14
#define CRC     15

/*
 *   AA 00 PASS CANID TYPE STS A1_V B2_V C1_V A2_V B2_V C2_V A1_I B1_I C1_I CRC
 */


class TCPMANAGERSHARED_EXPORT TcpThread : public QObject
{
    Q_OBJECT
public:
    explicit TcpThread(QObject *parent = nullptr);
    TcpThread(QString host,quint16 port);
    ~TcpThread();
private:



    void initConnect();
    bool checkSum(QByteArray byteArray);
    //电气火灾
    void parseDate(QByteArray byteArray);
    //电源监控
    void analysisDate(QByteArray byteArray);
    QString m_host;
    quint16 m_port;
    bool m_isOkConect;  //连接状态
    QTimer *m_tcpTimer;
    QTimer *m_tcpCmd;
    QTcpSocket *m_tcpSocket;//客户端套接字
signals:
    void sigConnectStatus(bool status);
    void sigUpdateInfo();
    void sigNodeUpdate(uint pass,uint id,uint type,uint sts,uint curValue,uint baseValue,uint alarmValue);
    void sigModUpdate(uint pass,uint id,uint type,uint sts,uint av_1,uint bv_1,uint cv_1,uint av_2,uint bv_2,uint cv_2,
                      qreal ai_1 = 0,qreal bi_1 = 0,qreal ci_1 = 0);
public slots:

private slots:
    void slotConnectSuccess();
    void slotDisconnect();
    void slotReceiveData();
    void slotConnectTimeOut();
    void slotTcpCmd();
    //void slotConnectError(QAbstractSocket::SocketError socketError);
};

#endif // TCPTHREAD_H

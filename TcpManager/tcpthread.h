#ifndef TCPTHREAD_H
#define TCPTHREAD_H

#include <QObject>
#include "tcpmanager.h"
class QTimer;
class QTcpSocket;
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
    void parseDate(QByteArray byteArray);
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

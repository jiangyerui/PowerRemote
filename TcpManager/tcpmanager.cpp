#include "tcpmanager.h"
#include "tcpthread.h"

TcpManager::TcpManager(QObject *parent) : QObject(parent)
{

}

TcpManager::TcpManager(QString host, quint16 port)
{
    tcpThread = new TcpThread(host,port);
    connect(tcpThread,&TcpThread::sigModUpdate,this,&TcpManager::sigModUpdate);
    //connect(tcpThread,&TcpThread::sigNodeUpdate,this,&TcpManager::sigNodeUpdate);
    connect(tcpThread,&TcpThread::sigConnectStatus,this,&TcpManager::sigConnectStatus);

//    connect(tcpManager,&TcpManager::sigModUpdate,this,&DisplayUnit::slotModUpdate);///更新modUnit[canId]
//    connect(tcpManager,&TcpManager::sigConnectStatus,this,&DisplayUnit::slotConnectStatus);

}

#include "tcpmanager.h"
#include "tcpthread.h"

TcpManager::TcpManager(QObject *parent) : QObject(parent)
{

}

TcpManager::TcpManager(QString host, quint16 port)
{
    tcpThread = new TcpThread(host,port);
    connect(tcpThread,&TcpThread::sigNodeUpdate,this,&TcpManager::sigNodeUpdate);
    connect(tcpThread,&TcpThread::sigConnectStatus,this,&TcpManager::sigConnectStatus);
}

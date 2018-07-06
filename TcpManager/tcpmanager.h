#ifndef TCPMANAGER_H
#define TCPMANAGER_H

#include <QObject>
#include "tcpmanager_global.h"
class TcpThread;

class TCPMANAGERSHARED_EXPORT TcpManager : public QObject
{
    Q_OBJECT
public:
    explicit TcpManager(QObject *parent = nullptr);
    TcpManager(QString host,quint16 port);
    TcpThread *tcpThread;
signals:
    void sigConnectStatus(bool status);
    void sigNodeUpdate(uint pass,uint id,uint type,uint sts,uint curValue,uint baseValue,uint alarmValue);
};

#endif // TCPMANAGER_H

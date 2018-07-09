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
    void sigModUpdate(uint pass,uint id,uint type,uint sts,uint av_1,uint bv_1,uint cv_1,uint av_2,uint bv_2,uint cv_2,
                      qreal ai_1 = 0,qreal bi_1 = 0,qreal ci_1 = 0);
};

#endif // TCPMANAGER_H

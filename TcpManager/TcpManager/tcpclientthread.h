#ifndef TCPCLIENTTHREAD_H
#define TCPCLIENTTHREAD_H

#include <QObject>

class TcpClientThread : public QObject
{
    Q_OBJECT
public:
    explicit TcpClientThread(QObject *parent = nullptr);
    TcpClientThread(QString host,quint16 port);
    ~TcpClientThread();

signals:

public slots:
};

#endif // TCPCLIENTTHREAD_H

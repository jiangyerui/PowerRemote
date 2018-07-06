#include "displaycontainer.h"
#include "ui_displaycontainer.h"
#include "displayunit.h"
#include "sqlmanager.h"
#include <QDebug>
DisplayContainer::DisplayContainer(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::DisplayContainer)
{
    ui->setupUi(this);
    confContainer();
}

DisplayContainer::~DisplayContainer()
{
    delete ui;

}

void DisplayContainer::confContainer()
{
    QString query = "select HOST,PORT,ABLE,ADDRESS from HOSTINFO;";
    QSqlDatabase db = SqlManager::openConnection();
    QList<QStringList> nodeList = SqlManager::getHostList(db,query,SqlManager::DisplayList);
    SqlManager::closeConnection(db);


    QString text = "主机-";
    for(int i = 0;i < nodeList.count();i++)
    {
        QStringList hostInfo = nodeList.value(i);
        QString hostStr = hostInfo.value(0);
        QString portStr =hostInfo.value(1);
        quint16 port = portStr.toUInt();

        DisplayUnit *disPlayUint = new DisplayUnit;
        disPlayUint->confTcpInfo(hostStr,port);

        addTab(disPlayUint,text+QString::number(i+1));
    }

}

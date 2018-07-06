#ifndef DISPLAYUNIT_H
#define DISPLAYUNIT_H

#include <QWidget>
#include "displayinfo.h"
#include "displayinfo.h"

#define PASSNUM 2
#define CANIDNUM 61


class QTimer;
class QThread;
class TcpManager;
class QGridLayout;
class QButtonGroup;

namespace Ui {
class DisplayUnit;
}

class BtnUnitInfo
{
public:

    bool used;
    int pass;
    int canId;
    int nodeStatus;
    int nodeType;

    int leakSet;  //漏电报警值
    int leakCur;  //电流实时
    int leakBase; //固有漏电
    int leakAlarm;//漏电设定值
    int leakAlarmLock;//漏电报警锁定值

    int tempCur;  //温度实时
    int tempSet;  //温度设定值
    int tempAlarm;//温度报警值
    int tempAlarmLock;//温度报警锁定值

    bool dropFlag;
    bool alarmFlag;
    bool errorFlag;
    bool normalFlag;

    void initData(){
        used = false;
        nodeStatus = 0;
        nodeType = 0;
        leakAlarm = 0;
        leakAlarmLock = false;
        leakBase = 0;
        leakCur = 0;
        leakSet = 0;

        tempAlarm = 0;
        tempAlarmLock = false;
        tempCur = 0;
        tempSet = 0;


        dropFlag  = false;
        alarmFlag = false;
        errorFlag = false;
        normalFlag = false;


    }
};
class DISPLAYINFOSHARED_EXPORT DisplayUnit : public QWidget
{
    Q_OBJECT

public:
    explicit DisplayUnit(QWidget *parent = 0);
    DisplayUnit(QString host,quint16 port);
    ~DisplayUnit();
    void confTcpInfo(QString host, quint16 port);

private:
    Ui::DisplayUnit *ui;
    QTimer *updateTimer;

    QThread *thread;
    TcpManager *tcpManager;
    QButtonGroup *tBtnGroup;

    int nodeCount;
    int errorCount;
    int alarmCount;
    QString host;

    uint curPass;
    uint curCanId;
    QList<BtnUnitInfo> tBtnUnitInfoList;

    BtnUnitInfo mod[PASSNUM][CANIDNUM];
    void initBtn();
    void initMod();
    void initWidget();

    void dateClean();//清空数据
    void updateNodeValue(uint pass, uint canId);//探测器实时数据


private slots:
    void slotNodeUpdate(uint pass,uint id,uint type,uint sts,uint curValue,uint baseValue,uint alarmValue);
    void slotConnectStatus(bool state);
    void slotBtnClick(int index);
    void slotUpdateTime();

    void slotBtnReset();
};

#endif // DISPLAYUNIT_H

#ifndef DISPLAYUNIT_H
#define DISPLAYUNIT_H

#include <QWidget>
#include "displayinfo.h"
#include "displayinfo.h"
#include <QMediaPlayer>

#define PASSNUM 2
#define CANIDNUM 127


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


class ModularUnit{
public:
    bool used;
    int pass;///CAN通道1或2
    int canId;
    int nodeStatus;///模块状态，如供电中断
    int nodeType;

    bool dropFlag;          //掉线
    bool normalFlag;        //正常
    bool phaseLossFlag;     //错相
    bool overCurrentFlag;   //过流
    bool overVoltageFlag;   //过压
    bool underVoltageFlag;  //欠压
    bool interruptionFlag;  //供电中断

    int _AV_1;
    int _BV_1;
    int _CV_1;

    int _AV_2;
    int _BV_2;
    int _CV_2;

    qreal _AI_1;
    qreal _BI_1;
    qreal _CI_1;

    void initData(){
        used = false;
        nodeType = 0;///模块类型
        nodeStatus = 0;///工作状态

        dropFlag = false;///掉线
        normalFlag = false;
        phaseLossFlag = false;
        overCurrentFlag = false;
        overVoltageFlag = false;
        underVoltageFlag = false;
        interruptionFlag = false;//供电中断

        _AV_1 = 0;
        _BV_1 = 0;
        _CV_1 = 0;

        _AV_2 = 0;
        _BV_2 = 0;
        _CV_2 = 0;

        _AI_1 = 0;
        _BI_1 = 0;
        _CI_1 = 0;
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
    QMediaPlayer *myPlayer;
    bool voiceFlag;

    int nodeCount;
    int errorCount;
    int oldErrorCount;
    int alarmCount;
    int oldAlarmCount;
    int pageCount=1;
    QString host;

    uint curPass;
    uint curCanId;
    QList<BtnUnitInfo> tBtnUnitInfoList;

    BtnUnitInfo mod[PASSNUM][CANIDNUM];

    ModularUnit modUnit[CANIDNUM+1];

    void initBtn();
    void initMod();
    void initWidget();
    void initPage();

    void dateClean();//清空数据
    void updateNodeValue(uint canId);//探测器实时数据


private slots:

    void slotConnectStatus(bool state);
    void slotBtnClick(int index);
    void slotUpdateTime();

    void slotBtnReset();

    void slotModUpdate(uint pass, uint canId, uint type, uint sts, uint av_1, uint bv_1, uint cv_1, uint av_2, uint bv_2, uint cv_2,
                       qreal ai_1 = 0, qreal bi_1 = 0, qreal ci_1 = 0);
    void on_upPageBtn_clicked();
    void on_downPageBtn_clicked();
    void on_tBtn_Voice_clicked();
};

#endif // DISPLAYUNIT_H

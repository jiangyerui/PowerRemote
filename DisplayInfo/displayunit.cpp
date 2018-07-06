#include "displayunit.h"
#include "ui_displayunit.h"
#include <QTimer>
#include <QDebug>
#include <QScrollBar>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QButtonGroup>
#include <QThread>
#include <QSqlDatabase>

#include "sqlmanager.h"
#include "tcpmanager.h"
#include "tcpthread.h"
#define MOD_LEAK 2//漏电
#define MOD_TEMP 3//温度

#define NORMAL 0
#define ALARM  1
#define ERROR  2
#define DROPED 4

DisplayUnit::DisplayUnit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DisplayUnit)
{
    ui->setupUi(this);
    initWidget();

    curPass = 0;
    curCanId= 0;
    alarmCount = 0;
    errorCount = 0;
    updateTimer = new QTimer;
    connect(updateTimer,&QTimer::timeout,this,&DisplayUnit::slotUpdateTime);
    updateTimer->start(2000);

    connect(ui->tBtnReset,&QToolButton::clicked,this,&DisplayUnit::slotBtnReset);

}


DisplayUnit::~DisplayUnit()
{
    delete ui;
}


void DisplayUnit::initBtn()
{
    tBtnGroup = new QButtonGroup;
    tBtnGroup->addButton(ui->tBtn_1,  1);tBtnGroup->addButton(ui->tBtn_2,  2);tBtnGroup->addButton(ui->tBtn_3,  3);
    tBtnGroup->addButton(ui->tBtn_4,  4);tBtnGroup->addButton(ui->tBtn_5,  5);tBtnGroup->addButton(ui->tBtn_6,  6);
    tBtnGroup->addButton(ui->tBtn_7,  7);tBtnGroup->addButton(ui->tBtn_8,  8);tBtnGroup->addButton(ui->tBtn_9,  9);
    tBtnGroup->addButton(ui->tBtn_10,10);

    tBtnGroup->addButton(ui->tBtn_11,11);tBtnGroup->addButton(ui->tBtn_12,12);tBtnGroup->addButton(ui->tBtn_13,13);
    tBtnGroup->addButton(ui->tBtn_14,14);tBtnGroup->addButton(ui->tBtn_15,15);tBtnGroup->addButton(ui->tBtn_16,16);
    tBtnGroup->addButton(ui->tBtn_17,17);tBtnGroup->addButton(ui->tBtn_18,18);tBtnGroup->addButton(ui->tBtn_19,19);
    tBtnGroup->addButton(ui->tBtn_20,20);

    tBtnGroup->addButton(ui->tBtn_21,21);tBtnGroup->addButton(ui->tBtn_22,22);tBtnGroup->addButton(ui->tBtn_23,23);
    tBtnGroup->addButton(ui->tBtn_24,24);tBtnGroup->addButton(ui->tBtn_25,25);tBtnGroup->addButton(ui->tBtn_26,26);
    tBtnGroup->addButton(ui->tBtn_27,27);tBtnGroup->addButton(ui->tBtn_28,28);tBtnGroup->addButton(ui->tBtn_29,29);
    tBtnGroup->addButton(ui->tBtn_30,30);

    tBtnGroup->addButton(ui->tBtn_31,31);tBtnGroup->addButton(ui->tBtn_32,32);tBtnGroup->addButton(ui->tBtn_33,33);
    tBtnGroup->addButton(ui->tBtn_34,34);tBtnGroup->addButton(ui->tBtn_35,35);tBtnGroup->addButton(ui->tBtn_36,36);
    tBtnGroup->addButton(ui->tBtn_37,37);tBtnGroup->addButton(ui->tBtn_38,38);tBtnGroup->addButton(ui->tBtn_39,39);
    tBtnGroup->addButton(ui->tBtn_40,40);

    tBtnGroup->addButton(ui->tBtn_41,41);tBtnGroup->addButton(ui->tBtn_42,42);tBtnGroup->addButton(ui->tBtn_43,43);
    tBtnGroup->addButton(ui->tBtn_44,44);tBtnGroup->addButton(ui->tBtn_45,45);tBtnGroup->addButton(ui->tBtn_46,46);
    tBtnGroup->addButton(ui->tBtn_47,47);tBtnGroup->addButton(ui->tBtn_48,48);tBtnGroup->addButton(ui->tBtn_49,49);
    tBtnGroup->addButton(ui->tBtn_50,50);

    tBtnGroup->addButton(ui->tBtn_51,51);tBtnGroup->addButton(ui->tBtn_52,52);tBtnGroup->addButton(ui->tBtn_53,53);
    tBtnGroup->addButton(ui->tBtn_54,54);tBtnGroup->addButton(ui->tBtn_55,55);tBtnGroup->addButton(ui->tBtn_56,56);
    tBtnGroup->addButton(ui->tBtn_57,57);tBtnGroup->addButton(ui->tBtn_58,58);tBtnGroup->addButton(ui->tBtn_59,59);
    tBtnGroup->addButton(ui->tBtn_60,60);
    for(int i = 1; i <= 60;i++)
    {
        tBtnGroup->button(i)->setVisible(false);
        tBtnGroup->button(i)->setIcon(QIcon(QPixmap(":/Image/normal.png")));
        tBtnGroup->button(i)->setText(QString::number(1)+"-"+QString::number(i));
    }
    connect(tBtnGroup,SIGNAL(buttonPressed(int)),this,SLOT(slotBtnClick(int)));
}

void DisplayUnit::initMod()
{
    for(int pass = 0;pass < PASSNUM;pass++)
    {
        for(int id = 0;id < CANIDNUM;id++)
        {
            mod[pass][id].initData();
        }
    }
}

void DisplayUnit::initWidget()
{
    initMod();
    initBtn();
    dateClean();

    ui->tBtnNest->setVisible(false);
    ui->tBtnLast->setVisible(false);


}

void DisplayUnit::confTcpInfo(QString host, quint16 port)
{
    this->host = host;
    thread = new QThread;
    tcpManager = new TcpManager(this->host,port);
    tcpManager->moveToThread(thread);
    thread->start();

    connect(tcpManager,&TcpManager::sigNodeUpdate,this,&DisplayUnit::slotNodeUpdate);
    connect(tcpManager,&TcpManager::sigConnectStatus,this,&DisplayUnit::slotConnectStatus);
}

void DisplayUnit::dateClean()
{
    //温度
    ui->lcdNb_tempAlarm->display("0");//温度报警
    ui->lcdNb_tempCur->display("0"); //温度实时
    ui->lcdNb_tempSet->display("0");//温度设定
    //漏电
    ui->lcdNb_leakBase->display("0");//固有漏电
    ui->lcdNb_leakCur->display("0");//实时漏电
    ui->lcdNb_leakAlarm->display("0");//漏电报警
    ui->lcdNb_leakSet->display("0");//漏电设定
}

void DisplayUnit::updateNodeValue(uint pass, uint canId)
{
    int leakSet    = mod[pass][canId].leakSet;
    int leakCur    = mod[pass][canId].leakCur;
    int leakBase   = mod[pass][canId].leakBase;
    int leakAlarm  = mod[pass][canId].leakAlarm;

    int tempCur    = mod[pass][canId].tempCur;
    int tempSet    = mod[pass][canId].tempSet;
    int tempAlarm  = mod[pass][canId].tempAlarm;

    int nodeType   = mod[pass][canId].nodeType;



    if(nodeType == 3)
    {
        //温度
        ui->lcdNb_tempCur->display(QString::number(tempCur));
        ui->lcdNb_tempSet->display(QString::number(tempSet));
        ui->lcdNb_tempAlarm->display(QString::number(tempAlarm));

        ui->lcdNb_leakCur->display("0");
        ui->lcdNb_leakSet->display("0");
        ui->lcdNb_leakBase->display("0");
        ui->lcdNb_leakAlarm->display("0");

    }
    else
    {
        //漏电
        ui->lcdNb_leakCur->display(QString::number(leakCur));
        ui->lcdNb_leakSet->display(QString::number(leakSet));
        ui->lcdNb_leakBase->display(QString::number(leakBase));
        ui->lcdNb_leakAlarm->display(QString::number(leakAlarm));

        ui->lcdNb_tempCur->display("0");
        ui->lcdNb_tempSet->display("0");
        ui->lcdNb_tempAlarm->display("0");
    }


}

void DisplayUnit::slotNodeUpdate(uint pass, uint canId, uint type, uint sts, uint curValue, uint baseValue, uint alarmValue)
{

    mod[pass][canId].used = true;
    mod[pass][canId].nodeType = type;
    if(type == 2)
    {
        mod[pass][canId].leakCur   = curValue;
        mod[pass][canId].leakBase  = baseValue;
        mod[pass][canId].leakSet   = alarmValue;
        /*************************************/
        mod[pass][canId].tempCur = 0;
        mod[pass][canId].tempSet = 0;
        mod[pass][canId].tempAlarm = 0;
    }
    else
    {
        mod[pass][canId].leakSet   = 0;
        mod[pass][canId].leakCur   = 0;
        mod[pass][canId].leakBase  = 0;
        mod[pass][canId].leakAlarm = 0;
        /*************************************/
        mod[pass][canId].tempCur = curValue;
        mod[pass][canId].tempSet = alarmValue;
        if(sts == 1 && mod[pass][canId].alarmFlag == false)
        {

            //插入数据
        }
    }

    switch (sts) {
    case NORMAL:
        //传感器故障
        if(mod[pass][canId].errorFlag == true)
        {
            mod[pass][canId].errorFlag = false;
            errorCount--;
        }
        //报警
        if(mod[pass][canId].alarmFlag == true)
        {
            mod[pass][canId].alarmFlag = false;
            alarmCount--;
        }
        //掉线
        if(mod[pass][canId].dropFlag == true)
        {
            mod[pass][canId].dropFlag  = false;
            errorCount--;
        }
        //正常
        if(mod[pass][canId].errorFlag == false&&mod[pass][canId].alarmFlag == false&&mod[pass][canId].dropFlag == false)
        {
            mod[pass][canId].normalFlag = true;
        }

        break;
    case ALARM:
        if(mod[pass][canId].alarmFlag == false)
        {
            mod[pass][canId].alarmFlag = true;
            alarmCount++;
            if(type == MOD_LEAK)
            {
                mod[pass][canId].leakAlarm = curValue;
            }
            else
            {
                mod[pass][canId].tempAlarm = curValue;
            }
            //插入数据
            uint alarmTime = QDateTime::currentDateTime().toTime_t();
            QSqlDatabase db = SqlManager::openConnection();
            SqlManager::insertAlarmRecord(db,host,pass,canId,type,sts,curValue,alarmTime);
            SqlManager::closeConnection(db);
        }
        break;
    case ERROR:

        if(mod[pass][canId].errorFlag == false)
        {
            errorCount++;
            mod[pass][canId].errorFlag = true;
            //
            uint alarmTime = QDateTime::currentDateTime().toTime_t();
            QSqlDatabase db = SqlManager::openConnection();
            SqlManager::insertAlarmRecord(db,host,pass,canId,type,sts,0,alarmTime);
            SqlManager::closeConnection(db);
        }
        break;
    case DROPED:
        if(mod[pass][canId].dropFlag == false)
        {
            errorCount++;
            mod[pass][canId].dropFlag = true;
            //
            uint alarmTime = QDateTime::currentDateTime().toTime_t();
            QSqlDatabase db = SqlManager::openConnection();
            SqlManager::insertAlarmRecord(db,host,pass,canId,type,sts,0,alarmTime);
            SqlManager::closeConnection(db);
        }

        break;
    default:
        break;
    }

    QString alarmText = "报警"+QString::number(alarmCount)+"个";
    QString errorText = "故障"+QString::number(errorCount)+"个";
    ui->lb_alarmNum->setText(alarmText);
    ui->lb_errorNum->setText(errorText);

    int index = 1;
    for(int canId = 1;canId < CANIDNUM; canId++)
    {
        tBtnGroup->button(index)->setText(" ");
        tBtnGroup->button(index)->setVisible(false);
        if(mod[pass][canId].used == true)
        {
            QString text = QString::number(pass)+"-"+QString::number(canId);
            tBtnGroup->button(index)->setText(text);
            tBtnGroup->button(index)->setVisible(true);

            if(mod[pass][canId].normalFlag == true)
            {
                tBtnGroup->button(index)->setIcon(QIcon(QPixmap(":/Image/normal.png")));
            }
            if(mod[pass][canId].errorFlag  == true)
            {
                tBtnGroup->button(index)->setIcon(QIcon(QPixmap(":/Image/error.png")));
            }
            if(mod[pass][canId].dropFlag   == true)
            {
                tBtnGroup->button(index)->setIcon(QIcon(QPixmap(":/Image/error.png")));
            }
            if(mod[pass][canId].alarmFlag  == true)
            {
                tBtnGroup->button(index)->setIcon(QIcon(QPixmap(":/Image/alarm.png")));
            }
            index++;
        }
    }

    ui->lb_nodeNum->setText(QString::number(index-1));
}

void DisplayUnit::slotConnectStatus(bool state)
{
    if(state == true)
    {
        ui->lb_netStatus->setText("连接");
    }
    else
    {
        ui->lb_netStatus->setText("断开");
    }
}

void DisplayUnit::slotBtnClick(int index)
{
    //文本长度
    int length = tBtnGroup->button(index)->text().length();
    QString text = tBtnGroup->button(index)->text();
    curPass  = 1;
    curCanId = text.right(length-2).toUInt();
    ui->lcdNb_address->display(QString("%1-%2").arg(curPass).arg(curCanId));
    dateClean();
}

void DisplayUnit::slotUpdateTime()
{
    updateNodeValue(curPass,curCanId);
}

void DisplayUnit::slotBtnReset()
{
    int ret = QMessageBox::question(this,"复位操作","你确定要复位模块吗？","确定","取消");

    if(ret == 0)
    {
        for(int i = 1;i <= 60;i++)
        {
            tBtnGroup->button(i)->setIcon(QIcon(QPixmap(":/Image/normal.png")));
            tBtnGroup->button(i)->setVisible(false);
        }
        initMod();
        dateClean();
        alarmCount = 0;
        errorCount = 0;
        ui->lb_nodeNum->setText("0");
        ui->lb_alarmNum->setText("报警0个");
        ui->lb_errorNum->setText("故障0个");
    }
}

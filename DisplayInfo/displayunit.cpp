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
    ui->tBtnReset->hide();


    //    connect(ui->tBtnReset,&QToolButton::clicked,this,&DisplayUnit::slotBtnReset);

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
}

void DisplayUnit::confTcpInfo(QString host, quint16 port)
{
    this->host = host;
    thread = new QThread;
    tcpManager = new TcpManager(this->host,port);
    tcpManager->moveToThread(thread);
    thread->start();

    connect(tcpManager,&TcpManager::sigModUpdate,this,&DisplayUnit::slotModUpdate);
    connect(tcpManager,&TcpManager::sigConnectStatus,this,&DisplayUnit::slotConnectStatus);
}

void DisplayUnit::dateClean()
{
    ui->lcdNb_AV_1->display("0");//温度报警
    ui->lcdNb_BV_1->display("0");//温度实时
    ui->lcdNb_CV_1->display("0");//温度设定
    ui->lcdNb_AV_2->display("0");//固有漏电
    ui->lcdNb_BV_2->display("0");//实时漏电
    ui->lcdNb_CV_2 ->display("0");//漏电报警
    ui->lcdNb_AI_1->display("0");//漏电设定
    ui->lcdNb_BI_1->display("0");
    ui->lcdNb_CI_1->display("0");
}


void DisplayUnit::updateNodeValue(uint canId)
{

    switch (modUnit[canId].nodeType) {
    case MOD_V:
    case MOD_V3:
    case MOD_VN3://三项双路有零
        ui->lcdNb_AV_1->display(modUnit[canId]._AV_1);
        ui->lcdNb_BV_1->display(modUnit[canId]._BV_1);
        ui->lcdNb_CV_1->display(modUnit[canId]._CV_1);

        ui->lcdNb_AV_2->display(modUnit[canId]._AV_2);
        ui->lcdNb_BV_2->display(modUnit[canId]._BV_2);
        ui->lcdNb_CV_2->display(modUnit[canId]._CV_2);

        ui->lcdNb_AI_1->display(0);
        ui->lcdNb_BI_1->display(0);
        ui->lcdNb_CI_1->display(0);
        break;
    case MOD_VA:
    case MOD_VA3://电压电流有零
    case MOD_VAN3://电压电流有零
        ui->lcdNb_AV_1->display(modUnit[canId]._AV_1);
        ui->lcdNb_BV_1->display(modUnit[canId]._BV_1);
        ui->lcdNb_CV_1->display(modUnit[canId]._CV_1);

        ui->lcdNb_AV_2->display(0);
        ui->lcdNb_BV_2->display(0);
        ui->lcdNb_CV_2->display(0);

        ui->lcdNb_AI_1->display(modUnit[canId]._AI_1);
        ui->lcdNb_BI_1->display(modUnit[canId]._BI_1);
        ui->lcdNb_CI_1->display(modUnit[canId]._CI_1);

        break;
    case MOD_2VAN3://两路三项电压一路三项电流
        ui->lcdNb_AV_1->display(modUnit[canId]._AV_1);
        ui->lcdNb_BV_1->display(modUnit[canId]._BV_1);
        ui->lcdNb_CV_1->display(modUnit[canId]._CV_1);

        ui->lcdNb_AV_2->display(modUnit[canId]._AV_2);
        ui->lcdNb_BV_2->display(modUnit[canId]._BV_2);
        ui->lcdNb_CV_2->display(modUnit[canId]._CV_2);

        ui->lcdNb_AI_1->display(modUnit[canId]._AI_1);
        ui->lcdNb_BI_1->display(modUnit[canId]._BI_1);
        ui->lcdNb_CI_1->display(modUnit[canId]._CI_1);

        break;
    default:
        break;
    }
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
    updateNodeValue(curCanId);
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

void DisplayUnit::slotModUpdate(uint pass, uint canId, uint type, uint sts, uint av_1, uint bv_1, uint cv_1,
                                uint av_2, uint bv_2, uint cv_2, qreal ai_1, qreal bi_1, qreal ci_1)
{
    Q_UNUSED(pass)
    modUnit[canId].used = true;
    modUnit[canId].nodeType = type;

    switch (type) {
    case MOD_V:
    case MOD_V3:
    case MOD_VN3://三项双路有零
        modUnit[canId]._AV_1 = av_1;
        modUnit[canId]._BV_1 = bv_1;
        modUnit[canId]._CV_1 = cv_1;

        modUnit[canId]._AV_2 = av_2;
        modUnit[canId]._BV_2 = bv_2;
        modUnit[canId]._CV_2 = cv_2;

        break;
    case MOD_VA:
    case MOD_VA3://电压电流有零
    case MOD_VAN3://电压电流有零
        modUnit[canId]._AV_1 = av_1;
        modUnit[canId]._BV_1 = bv_1;
        modUnit[canId]._CV_1 = cv_1;

        modUnit[canId]._AI_1 = ai_1;
        modUnit[canId]._BI_1 = bi_1;
        modUnit[canId]._CI_1 = ci_1;

        break;
    case MOD_2VAN3://两路三项电压一路三项电流
        modUnit[canId]._AV_1 = av_1;
        modUnit[canId]._BV_1 = bv_1;
        modUnit[canId]._CV_1 = cv_1;

        modUnit[canId]._AV_2 = av_2;
        modUnit[canId]._BV_2 = bv_2;
        modUnit[canId]._CV_2 = cv_2;

        modUnit[canId]._AI_1 = ai_1;
        modUnit[canId]._BI_1 = bi_1;
        modUnit[canId]._CI_1 = ci_1;
        break;
    default:
        break;
    }

    switch (sts) {
    case NORMAL:

        if(modUnit[canId].dropFlag == true)
        {
            errorCount--;
            modUnit[canId].dropFlag = false;
        }
        if(modUnit[canId].phaseLossFlag == true)
        {
            errorCount--;
            modUnit[canId].phaseLossFlag = false;
        }
        if(modUnit[canId].overCurrentFlag == true)
        {
            errorCount--;
            modUnit[canId].overCurrentFlag = false;
        }
        if(modUnit[canId].overVoltageFlag == true)
        {
            errorCount--;
            modUnit[canId].overVoltageFlag = false;
        }
        if(modUnit[canId].underVoltageFlag == true)
        {
            errorCount--;
           modUnit[canId].underVoltageFlag = false;
        }
        if(modUnit[canId].interruptionFlag == false)
        {
            alarmCount--;
            modUnit[canId].interruptionFlag = true;
        }
        if(modUnit[canId].dropFlag == false&&modUnit[canId].phaseLossFlag == false&&
                modUnit[canId].overCurrentFlag == false&&modUnit[canId].overVoltageFlag == false&&
                modUnit[canId].underVoltageFlag == false&&modUnit[canId].interruptionFlag == false)
        {
            ui->lbState->setText("正常");
            modUnit[canId].normalFlag = true;
        }


    case CANERROR:

        if(modUnit[canId].dropFlag == false)
        {
            errorCount++;
            ui->lbState->setText("通讯故障");
            modUnit[canId].dropFlag = true;
            uint alarmTime = QDateTime::currentDateTime().toTime_t();
            QSqlDatabase db = SqlManager::openConnection();
            SqlManager::insertAlarmRecord(db,host,1,canId,type,sts,0,alarmTime);
            SqlManager::closeConnection(db);
        }

        break;
    case PHASELOSS:

        if(modUnit[canId].phaseLossFlag == false)
        {
            errorCount++;
            modUnit[canId].phaseLossFlag = true;
            ui->lbState->setText("缺相");
            uint alarmTime = QDateTime::currentDateTime().toTime_t();
            QSqlDatabase db = SqlManager::openConnection();
            SqlManager::insertAlarmRecord(db,host,1,canId,type,sts,0,alarmTime);
            SqlManager::closeConnection(db);
        }

        break;
    case OVERCURRENT:

        if(modUnit[canId].overCurrentFlag == false)
        {
            errorCount++;
            ui->lbState->setText("过流");
            modUnit[canId].overCurrentFlag = true;
            uint alarmTime = QDateTime::currentDateTime().toTime_t();
            QSqlDatabase db = SqlManager::openConnection();
            SqlManager::insertAlarmRecord(db,host,1,canId,type,sts,0,alarmTime);
            SqlManager::closeConnection(db);
        }

        break;
    case OVERVOLTAGE:

        if(modUnit[canId].overVoltageFlag == false)
        {
            errorCount++;
            ui->lbState->setText("过压");
            modUnit[canId].overVoltageFlag = true;
            uint alarmTime = QDateTime::currentDateTime().toTime_t();
            QSqlDatabase db = SqlManager::openConnection();
            SqlManager::insertAlarmRecord(db,host,1,canId,type,sts,0,alarmTime);
            SqlManager::closeConnection(db);
        }

        break;
    case UNDERVOLTAGE:

        if(modUnit[canId].underVoltageFlag == false)
        {
            errorCount++;
            ui->lbState->setText("欠压");
            modUnit[canId].underVoltageFlag = true;
            uint alarmTime = QDateTime::currentDateTime().toTime_t();
            QSqlDatabase db = SqlManager::openConnection();
            SqlManager::insertAlarmRecord(db,host,1,canId,type,sts,0,alarmTime);
            SqlManager::closeConnection(db);
        }

        break;
    case INTERRUPTION:

        if(modUnit[canId].interruptionFlag == false)
        {
            alarmCount++;
            ui->lbState->setText("供电中断");
            modUnit[canId].interruptionFlag = true;
            uint alarmTime = QDateTime::currentDateTime().toTime_t();
            QSqlDatabase db = SqlManager::openConnection();
            SqlManager::insertAlarmRecord(db,host,1,canId,type,sts,0,alarmTime);
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
        if(modUnit[canId].used == true)
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
    //节点数目
    ui->lb_nodeNum->setText(QString::number(index-1));

}

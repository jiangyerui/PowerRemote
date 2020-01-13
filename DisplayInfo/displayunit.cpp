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
#include <QDir>
#include <QDebug>


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
    initWidget();///初始化模块结构体，和界面状态

    curPass = 0;
    curCanId= 0;
    alarmCount = 0;
    errorCount = 0;
    updateTimer = new QTimer;
    connect(updateTimer,&QTimer::timeout,this,&DisplayUnit::slotUpdateTime);///更新当前显示模块的状态
    updateTimer->start(2000);
    ui->tBtnReset->hide();


    //    connect(ui->tBtnReset,&QToolButton::clicked,this,&DisplayUnit::slotBtnReset);
    ///jiang start 2018.12.03
    ui->lb_alarmNum->setVisible(false);
    ui->label_2->setVisible(false);
    ///jiang end 2018.12.03

    /// ///jiang end 2019.04.27创建音频 播放
    QDir dir;
//    qDebug() << "currentPath : "+dir.currentPath()+"/alert.mp3";
//    qDebug() << "QApplication : "+QApplication::applicationFilePath()+"/alert.mp3";
//    qDebug() << "QCoreApplication : "+QCoreApplication::applicationDirPath()+"/alert.mp3";
    oldAlarmCount=0;
    oldErrorCount=0;
    voiceFlag = true;
    myPlayer = new QMediaPlayer;
    myPlayer->setMedia(QUrl::fromLocalFile(dir.currentPath()+"/alert.mp3"));
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
    for(int pass = 0;pass < PASSNUM;pass++)///两个通道
    {
        for(int id = 0;id < CANIDNUM+1;id++)///每个通道预设61个模块
        {
            modUnit[id].initData();///初始化模块数据
        }
    }
}

void DisplayUnit::initWidget()
{
    initMod();///初始化模块数据
    initBtn();///初始化模块界面状态
    dateClean();///初始化模块界面数据
}

void DisplayUnit::confTcpInfo(QString host, quint16 port)
{
    this->host = host;
    thread = new QThread;
    tcpManager = new TcpManager(this->host,port);
    tcpManager->moveToThread(thread);
    thread->start();


    connect(tcpManager,&TcpManager::sigModUpdate,this,&DisplayUnit::slotModUpdate);///更新modUnit[canId]
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

    ///jiang start 2018.12.03
    int sts = modUnit[canId].nodeStatus;
    switch (sts) {
    case NORMAL:
        ui->lbState->setText("正常");
        break;
    case CANERROR:
        ui->lbState->setText("通讯故障");
        break;
    case PHASELOSS:
        ui->lbState->setText("缺相");
        break;
    case OVERCURRENT:
        ui->lbState->setText("过流");
        break;
    case OVERVOLTAGE:
        ui->lbState->setText("过压");
        break;
    case UNDERVOLTAGE:
        ui->lbState->setText("欠压");
        break;
    case INTERRUPTION:
        ui->lbState->setText("供电中断");
        break;
    default:
        break;
    }
    ///jiang end 2018.12.03
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

    int sts = modUnit[curCanId].nodeStatus;
    switch (sts) {
    case NORMAL:
        ui->lbState->setText("正常");
        break;
    case CANERROR:
        ui->lbState->setText("通讯故障");
        break;
    case PHASELOSS:
        ui->lbState->setText("缺相");
        break;
    case OVERCURRENT:
        ui->lbState->setText("过流");
        break;
    case OVERVOLTAGE:
        ui->lbState->setText("过压");
        break;
    case UNDERVOLTAGE:
        ui->lbState->setText("欠压");
        break;
    case INTERRUPTION:
        ui->lbState->setText("供电中断");
        break;
    default:
        break;
    }


    dateClean();
//    updateNodeValue(index-1);
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
        ui->lb_alarmNum->setText("供电中断0个");
        ui->lb_errorNum->setText("通讯故障0个");
    }
}

void DisplayUnit::slotModUpdate(uint pass, uint canId, uint type, uint sts, uint av_1, uint bv_1, uint cv_1,
                                uint av_2, uint bv_2, uint cv_2, qreal ai_1, qreal bi_1, qreal ci_1)
{
   // qDebug()<<"canId:"<<canId<<"type:"<<type<<"sts:"<<sts;
//    qDebug()<<"=============void DisplayUnit::slotModUpdate=================="+host;


    Q_UNUSED(pass)
    modUnit[canId].used = true;
    modUnit[canId].nodeType = type;

//#define MOD_VN3     7//三项双路有零
//#define MOD_VAN3    8//电压电流有零
//#define MOD_2VAN3   9//两路三项电压一路三项电流
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

//#define NORMAL          0x00//正常
//#define OVERCURRENT     0x01//过流
//#define PHASELOSS       0x02//错相
//#define OVERVOLTAGE     0x03//过压
//#define UNDERVOLTAGE    0x04//欠压
//#define CANERROR        0x05//通讯中断
//#define INTERRUPTION    0x06//供电中断
//    qDebug()<<"sts : "<<sts;

///jiang start 2018.12.02
//    modUnit[curCanId].nodeStatus = sts;
        modUnit[canId].nodeStatus = sts;
///jiang end 2018.12.02
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
        if(modUnit[canId].interruptionFlag == true)
        {
            alarmCount--;
            modUnit[canId].interruptionFlag = false;
        }
        if(modUnit[canId].dropFlag == false && modUnit[canId].phaseLossFlag == false &&
                modUnit[canId].overCurrentFlag == false && modUnit[canId].overVoltageFlag == false&&
                modUnit[canId].underVoltageFlag == false && modUnit[canId].interruptionFlag == false)
        {
            //ui->lbState->setText("正常");
            modUnit[canId].normalFlag = true;
        }
        break;

    case CANERROR:///通讯中断

        if(modUnit[canId].dropFlag == false)
        {
            errorCount++;
            //ui->lbState->setText("通讯故障");
            modUnit[canId].dropFlag = true;
            uint alarmTime = QDateTime::currentDateTime().toTime_t();
            QSqlDatabase db = SqlManager::openConnection();
            SqlManager::insertAlarmRecord(db,host,1,canId,type,sts,0,alarmTime);
            SqlManager::closeConnection(db);
        }
        ///jiang end
        else

        {

            modUnit[canId].normalFlag = false;

            modUnit[canId].phaseLossFlag = false;

            modUnit[canId].overCurrentFlag = false;

            modUnit[canId].overVoltageFlag = false;

            modUnit[canId].underVoltageFlag = false;

            modUnit[canId].interruptionFlag = false;
        }
///jiang end
        break;
    case PHASELOSS:

        if(modUnit[canId].phaseLossFlag == false)
        {
            errorCount++;
            modUnit[canId].phaseLossFlag = true;
            //ui->lbState->setText("缺相");
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
            //ui->lbState->setText("过流");
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
            //ui->lbState->setText("过压");
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
            //ui->lbState->setText("欠压");
            modUnit[canId].underVoltageFlag = true;
            uint alarmTime = QDateTime::currentDateTime().toTime_t();
            QSqlDatabase db = SqlManager::openConnection();
            SqlManager::insertAlarmRecord(db,host,1,canId,type,sts,0,alarmTime);
            SqlManager::closeConnection(db);
        }

        break;
    case INTERRUPTION:///供电中断

        if(modUnit[canId].interruptionFlag == false)
        {
            alarmCount++;
            //ui->lbState->setText("供电中断");
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

    QString alarmText = "供电中断"+QString::number(alarmCount)+"个";
    QString errorText = "通讯故障"+QString::number(errorCount)+"个";

    ui->lb_alarmNum->setText(alarmText);
    ui->lb_errorNum->setText(errorText);
    if((alarmCount>0||errorCount>0)&&voiceFlag==true){
        //声音报警
//        qDebug()<<"声音报警****************************";
        myPlayer->play();
    }else{
        //消音
        myPlayer->pause();
    }

    if((alarmCount>oldAlarmCount)||(errorCount>oldErrorCount)){
        voiceFlag=true;
        myPlayer->play();
        oldAlarmCount=alarmCount;
        oldErrorCount=errorCount;
    }



    int index = 1;
    if(pageCount==1){
        for(int canId = 1;canId < 61; canId++)
        {
            tBtnGroup->button(index)->setText(" ");
            tBtnGroup->button(index)->setVisible(false);
            if(modUnit[canId].used == true)
            {
                QString text = QString::number(pass)+"-"+QString::number(canId);
                tBtnGroup->button(index)->setText(text);
                tBtnGroup->button(index)->setVisible(true);

                if(modUnit[canId].dropFlag== true)
                {
                    tBtnGroup->button(index)->setIcon(QIcon(QPixmap(":/Image/drop.png")));
                }
                if(modUnit[canId].normalFlag== true)
                {
                    tBtnGroup->button(index)->setIcon(QIcon(QPixmap(":/Image/normal.png")));
                }
                if(modUnit[canId].phaseLossFlag == true)
                {
                    tBtnGroup->button(index)->setIcon(QIcon(QPixmap(":/Image/error.png")));
                }
                if(modUnit[canId].overCurrentFlag== true)
                {
                    tBtnGroup->button(index)->setIcon(QIcon(QPixmap(":/Image/alarm.png")));
                }
                if(modUnit[canId].overVoltageFlag== true)
                {
                    tBtnGroup->button(index)->setIcon(QIcon(QPixmap(":/Image/alarm.png")));
                }
                if(modUnit[canId].underVoltageFlag== true)
                {
                    tBtnGroup->button(index)->setIcon(QIcon(QPixmap(":/Image/alarm.png")));
                }
                if(modUnit[canId].interruptionFlag == true)
                {
                    tBtnGroup->button(index)->setIcon(QIcon(QPixmap(":/Image/error.png")));
                }
                index++;
            }
    }
    }
    if(pageCount==2){
        for(int canId = 61;canId < 121; canId++)
        {
            tBtnGroup->button(index)->setText(" ");
            tBtnGroup->button(index)->setVisible(false);
            if(modUnit[canId].used == true)
            {
                QString text = QString::number(pass)+"-"+QString::number(canId);
                tBtnGroup->button(index)->setText(text);
                tBtnGroup->button(index)->setVisible(true);

                if(modUnit[canId].dropFlag== true)
                {
                    tBtnGroup->button(index)->setIcon(QIcon(QPixmap(":/Image/drop.png")));
                }
                if(modUnit[canId].normalFlag== true)
                {
                    tBtnGroup->button(index)->setIcon(QIcon(QPixmap(":/Image/normal.png")));
                }
                if(modUnit[canId].phaseLossFlag == true)
                {
                    tBtnGroup->button(index)->setIcon(QIcon(QPixmap(":/Image/error.png")));
                }
                if(modUnit[canId].overCurrentFlag== true)
                {
                    tBtnGroup->button(index)->setIcon(QIcon(QPixmap(":/Image/alarm.png")));
                }
                if(modUnit[canId].overVoltageFlag== true)
                {
                    tBtnGroup->button(index)->setIcon(QIcon(QPixmap(":/Image/alarm.png")));
                }
                if(modUnit[canId].underVoltageFlag== true)
                {
                    tBtnGroup->button(index)->setIcon(QIcon(QPixmap(":/Image/alarm.png")));
                }
                if(modUnit[canId].interruptionFlag == true)
                {
                    tBtnGroup->button(index)->setIcon(QIcon(QPixmap(":/Image/error.png")));
                }
                index++;
            }
    }
    }
    if(pageCount==3){
        for(int canId = 121;canId < 128; canId++)
        {
            tBtnGroup->button(index)->setText(" ");
            tBtnGroup->button(index)->setVisible(false);
            if(modUnit[canId].used == true)
            {
                QString text = QString::number(pass)+"-"+QString::number(canId);
                tBtnGroup->button(index)->setText(text);
                tBtnGroup->button(index)->setVisible(true);

                if(modUnit[canId].dropFlag== true)
                {
                    tBtnGroup->button(index)->setIcon(QIcon(QPixmap(":/Image/drop.png")));
                }
                if(modUnit[canId].normalFlag== true)
                {
                    tBtnGroup->button(index)->setIcon(QIcon(QPixmap(":/Image/normal.png")));
                }
                if(modUnit[canId].phaseLossFlag == true)
                {
                    tBtnGroup->button(index)->setIcon(QIcon(QPixmap(":/Image/error.png")));
                }
                if(modUnit[canId].overCurrentFlag== true)
                {
                    tBtnGroup->button(index)->setIcon(QIcon(QPixmap(":/Image/alarm.png")));
                }
                if(modUnit[canId].overVoltageFlag== true)
                {
                    tBtnGroup->button(index)->setIcon(QIcon(QPixmap(":/Image/alarm.png")));
                }
                if(modUnit[canId].underVoltageFlag== true)
                {
                    tBtnGroup->button(index)->setIcon(QIcon(QPixmap(":/Image/alarm.png")));
                }
                if(modUnit[canId].interruptionFlag == true)
                {
                    tBtnGroup->button(index)->setIcon(QIcon(QPixmap(":/Image/error.png")));
                }
                index++;
            }
    }
    }

    int canCount=0;
    for(int i=0 ;i<128;i++){
        if(modUnit[i].used == true){
            canCount++;
        }
    }
//    qDebug()<< canCount;

    //节点数目
    ui->lb_nodeNum->setText(QString::number(canCount));


//    QString threadName = QStringLiteral("@0x%1").arg(quintptr(thread->currentThreadId()),16,16,QLatin1Char('0'));

//    qDebug()<<"========="+threadName;

}

void DisplayUnit::initPage(){
    switch (pageCount) {
    case 1:
        for(int i = 1; i <= 60;i++)
        {
            tBtnGroup->button(i)->setVisible(false);
            tBtnGroup->button(i)->setIcon(QIcon(QPixmap(":/Image/normal.png")));
            tBtnGroup->button(i)->setText(QString::number(1)+"-"+QString::number(i));
        }
        break;
    case 2:
        for(int i = 1; i <= 60;i++)
        {
            tBtnGroup->button(i)->setVisible(false);
            tBtnGroup->button(i)->setIcon(QIcon(QPixmap(":/Image/normal.png")));
            tBtnGroup->button(i)->setText(QString::number(1)+"-"+QString::number(i+60));
        }
        break;
    case 3:
        for(int i = 1; i <= 60;i++)
        {
            tBtnGroup->button(i)->setVisible(false);
            tBtnGroup->button(i)->setIcon(QIcon(QPixmap(":/Image/normal.png")));
            tBtnGroup->button(i)->setText(QString::number(1)+"-"+QString::number(i+120));
        }
        break;
    }
}
void DisplayUnit::on_upPageBtn_clicked()
{
    pageCount=pageCount-1;
    if(pageCount<1||pageCount>3){
        pageCount=1;
    }
    initPage();

}

void DisplayUnit::on_downPageBtn_clicked()
{
    pageCount=pageCount+1;
    if(pageCount<1||pageCount>3){
        pageCount=3;
    }
    initPage();
}

void DisplayUnit::on_tBtn_Voice_clicked()
{

    if(voiceFlag){
        myPlayer->pause();
        voiceFlag=false;
    }

    else{
        myPlayer->play();
        voiceFlag=true;
    }

}

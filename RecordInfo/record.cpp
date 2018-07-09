#include "record.h"
#include "ui_record.h"
#include <QDebug>
#include <QMessageBox>
#include <QTimer>

#include "sqlmanager.h"


#define HOST    0
#define PASS    1
#define CANID   2
#define TYPE    3
#define STATE   4
#define TIME    5


#define OVERCURRENT     0x01//过流
#define PHASELOSS       0x02//错相
#define OVERVOLTAGE     0x03//过压
#define UNDERVOLTAGE    0x04//欠压
#define INTERRUPTION    0x05//供电中断
#define CANERROR        0x06//通讯中断





Record::Record(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Record)
{
    ui->setupUi(this);


    timer = new QTimer;
    connect(timer,&QTimer::timeout,this,&Record::slotUpdateTime);
    timer->start(60*1000);

    ui->dTEditStop->setDateTime(QDateTime::currentDateTime());

    initTableWidget(ui->tableWidget);

    QString query = "select HOST,PORT,ABLE,ADDRESS from HOSTINFO;";
    QSqlDatabase db = SqlManager::openConnection();
    QList<QStringList> nodeList = SqlManager::getHostList(db,query,SqlManager::DisplayList);
    SqlManager::closeConnection(db);

    for(int i = 0;i < nodeList.count();i++)
    {
        QStringList hostInfo = nodeList.value(i);
        QString hostStr = hostInfo.value(0);
        ui->cbBoxHost->addItem(hostStr);

    }

    connect(ui->tBtnQuery,&QToolButton::clicked,this,&Record::slotBtnQuery);
    connect(ui->tBtnDelete,&QToolButton::clicked,this,&Record::slotBtnDelete);
}

Record::~Record()
{
    delete ui;
}

void Record::initTableWidget(QTableWidget *tableWidget)
{
    QString styleStr = "QHeaderView::section{font: 12pt '楷体';background-color:rgb(53, 156, 212);"
                       "color: black;border: 1px solid #6c6c6c;}";
    tableWidget->horizontalHeader()->setStyleSheet(styleStr);
    tableWidget->verticalHeader()->setStyleSheet(styleStr);
    tableWidget->horizontalHeader()->setEnabled(false);
    tableWidget->horizontalHeader()->setVisible(true);//设置表头显示
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->horizontalHeader()->setHighlightSections(false);
    tableWidget->setFocusPolicy(Qt::NoFocus); //去除选中虚线框
    tableWidget->setEditTriggers(QTableWidget::NoEditTriggers);//单元格不可编
    tableWidget->setSelectionBehavior (QAbstractItemView::SelectRows); //设置选择行为，以行为单位
    tableWidget->setSelectionMode (QAbstractItemView::NoSelection); //设置选择模式，选择单行

    QStringList headList;
    headList<<tr("IP地址")<<tr("通道")<<tr("地址")<<tr("类型")<<tr("状态")<<tr("报警时间");
    tableWidget->setColumnCount(headList.count());
    tableWidget->setHorizontalHeaderLabels(headList);
    tableWidget->horizontalHeader()->setFixedHeight(30);
    tableWidget->verticalHeader()->setFixedWidth(22);


    tableWidget->setColumnWidth(0,200);
    tableWidget->setColumnWidth(1,100);
    tableWidget->setColumnWidth(2,100);
    tableWidget->setColumnWidth(3,100);
    tableWidget->setColumnWidth(4,100);
    tableWidget->setColumnWidth(5,300);

}

QString Record::confQuerySql()
{
    QString querySql = "select HOST,NET,ID,TYPE,STS,TIME from RECORD where";

    QString host = ui->cbBoxHost->currentText();

    querySql += " HOST = '"+host+"'";

    int state = ui->cbBoxType->currentIndex();
    switch (state) {
    case 0:
        break;
    case OVERCURRENT:
        querySql += " and STS = 1";
        break;
    case PHASELOSS:
        querySql += " and STS = 2";
        break;
    case OVERVOLTAGE:
        querySql += " and STS = 3";
        break;
    case UNDERVOLTAGE:
        querySql += " and STS = 4";
        break;
    case INTERRUPTION:
        querySql += " and STS = 5";
        break;
    case CANERROR:
        querySql += " and STS = 6";
        break;
    default:
        break;
    }

    QString startTime = QString::number(ui->dTEditStart->dateTime().toTime_t());
    QString stopTime  = QString::number(ui->dTEditStop->dateTime().toTime_t());

    querySql += " and TIME between "+startTime+" and "+stopTime+";";

    return querySql;
}


#define MOD_V3      2//双路三相电压型
#define MOD_VN3     7//三项双路有零

#define MOD_V       3//六路单相电压型

#define MOD_VA      6//单项电压电流

#define MOD_VA3     4//电压电流型
#define MOD_VAN3    8//电压电流有零

#define MOD_2VAN3   9//两路三项电压一路三项电流

void Record::recordListShow(QTableWidget *tableWidget,QString querySql)
{
    tableWidget->clearContents();
    int columnCount = tableWidget->columnCount();
    //获取数据列表
    QSqlDatabase db = SqlManager::openConnection();
    QList<QStringList> nodeList = SqlManager::getHostList(db,querySql,SqlManager::RecordList);
    SqlManager::closeConnection(db);

    tableWidget->setRowCount(nodeList.count());
    QFont ft("楷体",14);
    QTableWidgetItem *item;
    for(int row = 0; row < nodeList.count();row++)
    {
        QStringList itemStr = nodeList.at(row);
        QString host,pass,canId,type,state,value,time;
        host  = itemStr.at(0);
        pass  = itemStr.at(1);
        canId = itemStr.at(2);
        type  = itemStr.at(3);
        state = itemStr.at(4);
        time  = itemStr.at(5);
        //tableWidget->setRowHeight(row,27);

        for(int column = 0;column < columnCount;column++)
        {
            item = new QTableWidgetItem;
            item->setFont(ft);
            item->setTextAlignment(Qt::AlignCenter);
            item->setTextColor(QColor(255,255,255));

            switch (column)
            {
            case HOST://主机地址
                item->setText(host);
                break;
            case PASS://通道
                item->setText(pass);
                break;
            case CANID://地址
                item->setText(canId);
                break;
            case TYPE://类型
                switch (type.toInt()) {
                case MOD_V3:
                case MOD_VN3:
                    item->setText("V32");
                    break;
                case MOD_V:
                    item->setText("V");
                    break;
                case MOD_VA:
                    item->setText("VA3");
                    break;
                case MOD_VA3:
                case MOD_VAN3:
                    item->setText("VA");
                    break;
                case MOD_2VAN3:
                    item->setText("VA32");
                    break;
                default:
                    break;
                }
                break;
            case STATE://状态
                switch (state.toInt()) {
                case OVERCURRENT:
                    item->setText("过流报警");
                    break;
                case PHASELOSS:
                    item->setText("缺相故障");
                    break;
                case OVERVOLTAGE:
                    item->setText("过压掉线");
                    break;
                case UNDERVOLTAGE:
                    item->setText("欠压报警");
                    break;
                case INTERRUPTION:
                    item->setText("通讯故障");
                    break;
                case CANERROR:
                    item->setText("供电中断");
                    break;
                default:
                    break;
                }
                break;
            case TIME://时间
                item->setText(QDateTime::fromTime_t(time.toUInt()).toString("yyyy年MM月dd日 hh:mm:ss"));
                break;
            default:
                break;
            }
            tableWidget->setItem(row,column,item);
        }
    }
}

void Record::slotBtnQuery()
{
    QString querySql = confQuerySql();
    recordListShow(ui->tableWidget,querySql);
}

void Record::slotBtnBackup()
{

}

void Record::slotBtnDelete()
{
    //QMessageBox::question(this,"历史信息删除","你确定要删除历史信息吗？","全部删除","单条删除","取消");
    int ret = QMessageBox::question(this,"历史信息删除","你确定要删除全部的历史信息吗？","取消","确定");
    if(ret)
    {
        QString sql = "delete from RECORD;";
        QSqlDatabase db = SqlManager::openConnection();
        bool delFlag = SqlManager::delAllData(db,sql);
        SqlManager::closeConnection(db);

        if(delFlag)
        {
            ui->tableWidget->clearContents();
            ui->tableWidget->setRowCount(0);
        }
    }

}

void Record::slotUpdateTime()
{
    ui->dTEditStop->setDateTime(QDateTime::currentDateTime());
}

#include "confhost.h"
#include "ui_confhost.h"
#include <sqlmanager.h>
#include <QMessageBox>


#define HOST    0
#define PORT    1
#define ABLE    2
#define ADDRESS 3


ConfHost::ConfHost(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfHost)
{
    ui->setupUi(this);
    initTableWidget(ui->tableWidget);
    confHostList(ui->tableWidget);
    initConnect();
}

ConfHost::~ConfHost()
{
    delete ui;
}

void ConfHost::initTableWidget(QTableWidget *tableWidget)
{
    QString styleStr = "QHeaderView::section{font: 12pt '楷体';background-color:rgb(53, 156, 212);"
                       "color: black;border: 1px solid #6c6c6c;}";
    tableWidget->horizontalHeader()->setStyleSheet(styleStr);
    tableWidget->verticalHeader()->setStyleSheet(styleStr);
    tableWidget->horizontalHeader()->setEnabled(false);
    tableWidget->horizontalHeader()->setVisible(true);//设置表头显示
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->horizontalHeader()->setHighlightSections(false);

    tableWidget->setEditTriggers(QTableWidget::NoEditTriggers);//单元格不可编
    tableWidget->setSelectionBehavior (QAbstractItemView::SelectRows); //设置选择行为，以行为单位
    tableWidget->setSelectionMode (QAbstractItemView::SingleSelection); //设置选择模式，选择单行

    QStringList headList;
    headList<<tr("IP地址")<<tr("主机端口")<<tr("状态")<<tr("主机地址");
    tableWidget->setColumnCount(headList.count());
    tableWidget->setHorizontalHeaderLabels(headList);
    tableWidget->horizontalHeader()->setFixedHeight(30);
    tableWidget->verticalHeader()->setFixedWidth(22);

    tableWidget->setColumnWidth(0,200);
    tableWidget->setColumnWidth(1,100);
    tableWidget->setColumnWidth(2,60);
    tableWidget->setColumnWidth(3,300);
}

void ConfHost::initConnect()
{
    connect(ui->tBtnAdd,&QToolButton::clicked,this,&ConfHost::slotBtnAddHost);
    connect(ui->tBtnDel,&QToolButton::clicked,this,&ConfHost::slotBtnDelHost);
    connect(ui->tBtnSave,&QToolButton::clicked,this,&ConfHost::slotBtnSaveHost);
    connect(ui->tableWidget,&QTableWidget::cellClicked,this,&ConfHost::slotCellCheckClick);
}

void ConfHost::delAll()
{
    QString query = "delete from HOSTINFO;";
    QSqlDatabase db = SqlManager::openConnection();
    SqlManager::delAllData(db,query);
    SqlManager::closeConnection(db);

}

void ConfHost::confHostList(QTableWidget *tableWidget)
{
    int columnCount = tableWidget->columnCount();
    QString query = "select HOST,PORT,ABLE,ADDRESS from HOSTINFO;";
    QSqlDatabase db = SqlManager::openConnection();
    QList<QStringList> nodeList = SqlManager::getHostList(db,query,SqlManager::HostList);
    SqlManager::closeConnection(db);

    tableWidget->setRowCount(nodeList.count());
    QFont ft("楷体",12);
    QTableWidgetItem *item;
    for(int row = 0; row < nodeList.count();row++)
    {
        QStringList itemStr = nodeList.at(row);
        QString host,port,number,able,address;
        host    = itemStr.at(0);
        port    = itemStr.at(1);
        able    = itemStr.at(2);
        address = itemStr.at(3);
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
            case PORT://端口
                item->setText(port);
                break;
            case ABLE://功能
                if(able.toInt() == 1)
                {
                    item->setText("启用");
                    item->setCheckState(Qt::Checked);
                }
                else
                {
                    item->setText("禁用");
                    item->setCheckState(Qt::Unchecked);
                }
                break;
            case ADDRESS://地址
                item->setText(address);
                break;
            default:
                break;
            }
            tableWidget->setItem(row,column,item);
        }
    }
}

void ConfHost::slotBtnAddHost()
{
    QString hostIP = ui->lEditHostIP->text();
    QString hostAddress = ui->lEditHostAddress->text();

    if(hostAddress.isEmpty())
    {
        hostAddress+="-";
    }

    ui->tableWidget->setRowCount(ui->tableWidget->rowCount()+1);

    QFont ft("楷体",16);
    QTableWidgetItem *item;
    for(int column = 0;column < ui->tableWidget->columnCount();column++)
    {
        item = new QTableWidgetItem;
        item->setFont(ft);
        item->setTextColor(QColor(0,0,0));
        item->setTextAlignment(Qt::AlignCenter);
        switch (column) {
        case HOST:
            item->setText(hostIP);
            break;
        case PORT:
            item->setText("5000");
            break;
        case ABLE:
            item->setCheckState(Qt::Checked);
            item->setText("启用");
            break;
        case ADDRESS:
            item->setText(hostAddress);
            break;
        default:
            break;
        }
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,column,item);
    }

}

void ConfHost::slotBtnDelHost()
{
    int row = ui->tableWidget->currentRow();
    if(row >= 0 && ui->tableWidget->rowCount() != 1)
    {
        QString host = ui->tableWidget->item(row,0)->text();
        QString port = ui->tableWidget->item(row,1)->text();
        ui->tableWidget->removeRow(row);
        QSqlDatabase db = SqlManager::openConnection();
        bool flag = SqlManager::delelteHostItem(db,host,port);
        SqlManager::closeConnection(db);

        if(flag)
        {
            QMessageBox::information(this,QStringLiteral("删除提示"),QStringLiteral("该条信息已经删除!"),QStringLiteral("关闭"));
        }
    }
    else
    {
        QMessageBox::information(this,QStringLiteral("删除提示"),QStringLiteral("地址信息至少要有一条!"),QStringLiteral("关闭"));
    }
}

void ConfHost::slotBtnSaveHost()
{
    int rowCount = ui->tableWidget->rowCount();
    if(rowCount > 0)
    {
        delAll();
        for(int row = 0;row < rowCount;row++)
        {
            QString host = ui->tableWidget->item(row,HOST)->text();
            QString port = ui->tableWidget->item(row,PORT)->text();
            QString able;
            if(ui->tableWidget->item(row,ABLE)->checkState() == Qt::Checked)
            {
                able = QString("1");
            }
            else
            {
                able = QString("0");
            }
            QString address = ui->tableWidget->item(row,ADDRESS)->text();
            QSqlDatabase db = SqlManager::openConnection();
            bool flag = SqlManager::insertHostList(db,host,port,able,address);
            SqlManager::closeConnection(db);
            if(!flag)
            {
                QMessageBox::warning(this,"保存提示","信息保存失败！","关闭");
                return;
            }
        }
        QMessageBox::warning(this,"保存提示","信息保存成功！","关闭");
    }
}


void ConfHost::slotCellCheckClick(int row, int column)
{
    if(column == ABLE)
    {
        if(ui->tableWidget->item(row,column)->checkState() == Qt::Unchecked)
        {
            qDebug()<<"启用";
            ui->tableWidget->item(row,column)->setText(tr("启用"));
            ui->tableWidget->item(row,column)->setCheckState(Qt::Checked);
        }
        else
        {
            qDebug()<<"禁用";
            ui->tableWidget->item(row,column)->setText(tr("禁用"));
            ui->tableWidget->item(row,column)->setCheckState(Qt::Unchecked);
        }
    }
}

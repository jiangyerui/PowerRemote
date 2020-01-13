#include "mainwindow.h"
#include "ui_mainwindow.h"

//lib
#include "about.h"
#include "record.h"
#include "confhost.h"
#include "displaycontainer.h"
#include <QDateTime>
#include <QTimer>
#include <QDebug>
//////////////////
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQueryModel>
MainWindow::MainWindow(QWidget *parent)
    :QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    setWindowFlags(Qt::FramelessWindowHint);
//    showFullScreen();

    ui->stackedWidget->addWidget((new DisplayContainer));///显示监控信息
    ui->stackedWidget->addWidget((new Record));///显示历史记录
    ui->stackedWidget->addWidget((new ConfHost));///显示主机配置
    ui->stackedWidget->addWidget((new About));///显示版本信息

    initConnnet();///初始化连接
    ///jiang start 2018.12.02
//    normalStyle = "QToolButton{font: 22pt '楷体';border:2px none;color: rgb(85, 170, 127);border-color: rgb(0, 170, 127);}"
//                  "QToolButton:hover{font: 24pt '楷体';color: rgb(255, 255, 255);background-color: rgb(3, 127, 119);}";
//    selectStyle = "font: 24pt '楷体';color: rgb(255, 255, 255);background-color: rgb(3, 127, 119);";

    normalStyle = "QToolButton{font: 22pt '楷体';border:2px none;color: rgb(85, 170, 127);border-color: rgb(0, 170, 127);}"
                  "QToolButton:hover{font: 24pt '楷体';color: rgb(255, 255, 255);background-color: rgb(40, 40, 41);}";
    selectStyle = "font: 24pt '楷体';color: rgb(255, 255, 255);background-color: rgb(40, 40, 41);";
      ///jiang end 2018.12.02
    slotBtnDisplayInfo();///软件启动即可显示监控信息


    systemTimer = new QTimer;
    connect(systemTimer,&QTimer::timeout,this,&MainWindow::slotSystemTime);///更新系统时间
    systemTimer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initConnnet()
{
    connect(ui->tBtnAboutInfo,&QToolButton::clicked,this,&MainWindow::slotBtnAboutInfo);
    connect(ui->tBtnRecordInfo,&QToolButton::clicked,this,&MainWindow::slotBtnRecordInfo);
    connect(ui->tBtnDisplayInfo,&QToolButton::clicked,this,&MainWindow::slotBtnDisplayInfo);
    connect(ui->tBtnConfHostInfo,&QToolButton::clicked,this,&MainWindow::slotBtnConfHostInfo);

}

void MainWindow::slotBtnDisplayInfo()
{

    ui->stackedWidget->setCurrentIndex(0);

    ui->tBtnAboutInfo->setStyleSheet(normalStyle);
    ui->tBtnRecordInfo->setStyleSheet(normalStyle);
    ui->tBtnDisplayInfo->setStyleSheet(selectStyle);
    ui->tBtnConfHostInfo->setStyleSheet(normalStyle);
}

void MainWindow::slotBtnRecordInfo()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->tBtnAboutInfo->setStyleSheet(normalStyle);
    ui->tBtnRecordInfo->setStyleSheet(selectStyle);
    ui->tBtnDisplayInfo->setStyleSheet(normalStyle);
    ui->tBtnConfHostInfo->setStyleSheet(normalStyle);
}

void MainWindow::slotBtnConfHostInfo()
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->tBtnAboutInfo->setStyleSheet(normalStyle);
    ui->tBtnRecordInfo->setStyleSheet(normalStyle);
    ui->tBtnDisplayInfo->setStyleSheet(normalStyle);
    ui->tBtnConfHostInfo->setStyleSheet(selectStyle);
}

void MainWindow::slotBtnAboutInfo()
{
    ui->stackedWidget->setCurrentIndex(3);
    ui->tBtnAboutInfo->setStyleSheet(selectStyle);
    ui->tBtnRecordInfo->setStyleSheet(normalStyle);
    ui->tBtnDisplayInfo->setStyleSheet(normalStyle);
    ui->tBtnConfHostInfo->setStyleSheet(normalStyle);
}

void MainWindow::slotSystemTime()
{
    if(timeFlag == false)
    {
        timeFlag = true;
        QString curTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm");
        ui->lcdNumber->display(curTime);
    }
    else
    {
        timeFlag = false;
        QString curTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh mm");
        ui->lcdNumber->display(curTime);
    }


}

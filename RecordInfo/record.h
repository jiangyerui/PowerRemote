#ifndef RECORD_H
#define RECORD_H

#include <QWidget>
#include "recordinfo.h"
namespace Ui {
class Record;
}
class QTimer;
class QTableWidget;
class RECORDINFOSHARED_EXPORT Record : public QWidget
{
    Q_OBJECT

public:
    explicit Record(QWidget *parent = 0);
    ~Record();

private:
    Ui::Record *ui;
    QTimer *timer;
    void initTableWidget(QTableWidget *tableWidget);
    QString confQuerySql();
    void recordListShow(QTableWidget *tableWidget, QString querySql);
private slots:
    void slotBtnQuery();
    void slotBtnBackup();
    void slotBtnDelete();

public slots:
    void slotUpdateTime();
};

#endif // RECORD_H

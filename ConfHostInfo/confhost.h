#ifndef CONFHOST_H
#define CONFHOST_H

#include <QWidget>
#include "confhostinfo.h"
namespace Ui {
class ConfHost;
}


class QTableWidget;
class CONFHOSTINFOSHARED_EXPORT ConfHost : public QWidget
{
    Q_OBJECT

public:
    explicit ConfHost(QWidget *parent = 0);
    ~ConfHost();

private:
    Ui::ConfHost *ui;
    void initTableWidget(QTableWidget *tableWidget);
    void initConnect();
    void delAll();
    void confHostList(QTableWidget *tableWidget);
private slots:
    void slotBtnAddHost();
    void slotBtnDelHost();
    void slotBtnSaveHost();
    void slotCellCheckClick(int row,int column);
};

#endif // CONFHOST_H

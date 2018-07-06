#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}
class QTimer;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void initConnnet();
    QTimer *systemTimer;
    QString normalStyle;
    QString selectStyle;
    bool timeFlag;

private slots:
    void slotBtnRecordInfo();
    void slotBtnDisplayInfo();
    void slotBtnConfHostInfo();
    void slotBtnAboutInfo();

    void slotSystemTime();
};

#endif // MAINWINDOW_H

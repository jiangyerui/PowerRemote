#ifndef DISPLAYCONTAINER_H
#define DISPLAYCONTAINER_H


#include <QTabWidget>
#include "displayinfo.h"
namespace Ui {
class DisplayContainer;
}

class DISPLAYINFOSHARED_EXPORT DisplayContainer : public QTabWidget
{
    Q_OBJECT

public:
    explicit DisplayContainer(QWidget *parent = 0);
    ~DisplayContainer();
    void confContainer();

private:
    Ui::DisplayContainer *ui;

};

#endif // DISPLAYCONTAINER_H

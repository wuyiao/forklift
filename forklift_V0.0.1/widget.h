#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <qgridlayout.h>
#include <QAudioOutput>
#include "videoWorkThread.h"
#include "tcpServer.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    QAudioOutput *audio;
    videoWorkThread *fVideoThread[5];
    QThread *fVideoWorkThread[5];

    tcpServer *fTcpServer;
    QThread *fTcpServerWorkThread;


private:
    Ui::Widget *ui;

    int bind_status;

    void widgetWorkThreadInit();
    void widgetWorkThreadRun();
    void widgetWorkThreadStop();
    void widgetWorkThreadClear();
    void widgetworkThreadDestory();

    void InitWidget();

    void showBindQRCode(QString msg);
    void showCheckQRCode(QString msg);
    void showTest(QString msg);


    void widget0();
    void widget0_1();
    void widget1(QString msg);
    void widget2();
    void widget3();
    void widget4();


    void faceInfo();

    void event0();
    void event1();
    void event2();
    void event3(int camera_id);
    void event4();
    void event5();
    void event6();
    void event7();

    QString fUuidTime;
    void uuid_data();

private:
    QLabel *fEvent0;
    QLabel *fEvent1;
    QLabel *fEvent2;
    QLabel *fEvent3;
    QLabel *fEvent4;
    QLabel *fEvent5;
    QLabel *fEvent6;
    QLabel *fEvent7;

signals:
    void widgetThreadStopWork();
    void videoWorkThreadStartWork();
    void tcpServerWorkThreadStartWork();

};
#endif // WIDGET_H

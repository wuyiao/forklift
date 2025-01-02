#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <QThread>
//#include <QTcpServer>
//#include <QTcpSocket>
#include <qlocalsocket.h>
#include <qtimer.h>
#include "common.hpp"
#include "buffer_queue.h"

class tcpServer : public QThread
{
    Q_OBJECT
public:
    explicit tcpServer();
    ~tcpServer();

    void doWork();
    void stopWork();
    void send_image_data(int image_type,int image_len,unsigned char * buff);

    QTimer *reconnect_timer;
    QLocalSocket * m_socket;
//    QTcpServer * fTcpServer;
//    QTcpSocket * fTcpSocket;
    int ret;
    unsigned int len;
    int accept_fd;
    int socket_fd;
    char * recv_buf;
    unsigned char * image0;
    unsigned char * image1;
    unsigned char * image2;
    unsigned char * image3;
    unsigned char * image4;

    unsigned int data_head;
    int data_type;
    int data_len;
    int driver_info[2];

    BUFFQUEUE_HANDLE fImageHandle;
    BUFFQUEUE_HANDLE fImageHandle1;
    BUFFQUEUE_HANDLE fImageHandle2;
    BUFFQUEUE_HANDLE fImageHandle3;
    BUFFQUEUE_HANDLE fImageHandle4;
    VIDEO_FRAME_INFO_T fImageInfo;
    VIDEO_FRAME_INFO_T fImageInfo1;
    VIDEO_FRAME_INFO_T fImageInfo2;
    VIDEO_FRAME_INFO_T fImageInfo3;
    VIDEO_FRAME_INFO_T fImageInfo4;

public slots:
//    void newConnection_Slot();
//    void readyRead_Slot();
//    void rcv_data();
//    void connect_success();
//    void disconnect_from_server();
//    void reconect_to_server();
//    void error_proc(QLocalSocket::LocalSocketError state);

private:
    bool tcpServerThreadStatu;


signals:
    void endWorkClear();

    void widgetTo0();
    void widgetTo0_1();
    void widgetTo1(QString);
    void widgetTo2();
    void widgetTo3();
    void widgetTo4();

    void eventTo0();
    void eventTo1();
    void eventTo2();
    void eventTo3(int);
    void eventTo4();
    void eventTo5();
    void eventTo6();
    void eventTo7();
};

#endif

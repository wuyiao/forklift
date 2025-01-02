#include "tcpServer.hpp"
#include <QDebug>
//#include <QMessageBox>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <qtimer.h>
#include <qlocalsocket.h>
#include <qstring.h>
#include "buffer_queue.h"
#include "common.hpp"

#define IMAGE_WIDTH        (1280)
#define IMAGE_HEIGHT       (720)
#define IMAGE_SIZE_RGB24         (IMAGE_WIDTH * IMAGE_HEIGHT * 3)  //单张图片大小
#define SERVER_NAME "wuyiao"
#define UN_PATH "/tmp/socket_file"

socklen_t clt_addr_len;
struct sockaddr_un clt_addr;
struct sockaddr_un srv_addr;



tcpServer::tcpServer():
    recv_buf(nullptr),
    image0(nullptr),
    image1(nullptr),
    image2(nullptr),
    image3(nullptr),
    image4(nullptr)
{
    driver_info[0] = 0;
    driver_info[1] = 0;
    recv_buf = (char *)malloc(0x10000);
    image0 = (unsigned char *)malloc(IMAGE_SIZE_RGB24);
    image1 = (unsigned char *)malloc(IMAGE_SIZE_RGB24);
    image2 = (unsigned char *)malloc(IMAGE_SIZE_RGB24);
    image3 = (unsigned char *)malloc(IMAGE_SIZE_RGB24);
    image4 = (unsigned char *)malloc(IMAGE_SIZE_RGB24);

    int ret  = -1;
    ret = Buffer_Queue_Producer_Create(&fImageHandle, FACE_VIDEO, NULL, 1024*10000, 30000);
    if (ret < 0) {
        qDebug("FACE_VIDEO create failed\n");
        return ;
    }
    ret = Buffer_Queue_Producer_Create(&fImageHandle1, FRONT_VIDEO, NULL, 1024*10000, 30000);
    if (ret < 0) {
        qDebug("FRONT_VIDEO create failed\n");
        return ;
    }
    ret = Buffer_Queue_Producer_Create(&fImageHandle2, BACK_VIDEO, NULL, 1024*10000, 30000);
    if (ret < 0) {
        qDebug("BACK_VIDEO create failed\n");
        return ;
    }
    ret = Buffer_Queue_Producer_Create(&fImageHandle3, LEFT_VIDEO, NULL, 1024*10000, 30000);
    if (ret < 0) {
        qDebug("LEFT_VIDEO create failed\n");
        return ;
    }
    ret = Buffer_Queue_Producer_Create(&fImageHandle4, RIGHT_VIDEO, NULL, 1024*10000, 30000);
    if (ret < 0) {
        qDebug("RIGHT_VIDEO create failed\n");
        return ;
    }


    socket_fd=socket(PF_UNIX,SOCK_STREAM,0);
    if(socket_fd<0)
    {
        perror("cannot create communication socket");
        return ;
    }

    // 设置服务器参数
    srv_addr.sun_family=AF_UNIX;
    strncpy(srv_addr.sun_path,UN_PATH,sizeof(srv_addr.sun_path)-1);
    unlink(UN_PATH);

    // 绑定socket地址
    ret=bind(socket_fd,(struct sockaddr*)&srv_addr,sizeof(srv_addr));
    if(ret==-1)
    {
        perror("cannot bind server socket");
        close(socket_fd);
        unlink(UN_PATH);
        return ;
    }


}

tcpServer::~tcpServer()
{
    free(recv_buf);
    free(image0);
    free(image1);
    free(image2);
    free(image3);
    free(image4);
    // 关闭socket
    close(accept_fd);
    close(socket_fd);
    unlink(UN_PATH);
    stopWork();
    if (fImageHandle) {
            Buffer_Queue_Producer_Destroy(&fImageHandle);
    }
}

void tcpServer::doWork()
{
    QString str;
    int bind_value = 0,len1 = 0,len2 = 0,num = 0,camera_id = 0;
    // 监听
    ret=listen(socket_fd,1);
    if(ret==-1)
    {
        perror("cannot listen the client connect request");
        close(socket_fd);
        unlink(UN_PATH);
        return ;
    }

    // 接受connect请求
    len=sizeof(clt_addr);
    accept_fd=::accept(socket_fd,(struct sockaddr*)&clt_addr,&len);
    if(accept_fd<0)
    {
        perror("cannot accept client connect request");
        close(socket_fd);
        unlink(UN_PATH);
        return ;
    }

    // 读取和写入
    memset(recv_buf,0,0x10000);
    while(1)
    {
//        usleep(100000);
        memset(recv_buf,0,0x10000);

        len1 = 0;
        len2 = 0;

        read(accept_fd,&num,sizeof(num));
//        qDebug("num %d \n",num);


        read(accept_fd,&data_head,sizeof(data_head));
        if(data_head == 0xaa55)
        {
//            qDebug("data_head : %d\n",data_head);
        }
        else
        {
            qDebug("head error\n");
            continue;
        }


        read(accept_fd,&data_type,sizeof(data_type));
        if(data_type > 100)
                continue;
//        qDebug("data_type : %d\n",data_type);
        read(accept_fd,&data_len,sizeof(data_len));
        if(data_len > IMAGE_SIZE_RGB24)
            continue;
//        qDebug("data_len : %d\n",data_len);
        switch (data_type)
        {
            case FACE_VIDEO:
                do
                {
                    len1 = read(accept_fd,image0 + len2,data_len - len2);
                    len2 = len2 + len1;
//                    qDebug("len1 %d  len2 %d \n",len1,len2);
                }while(data_len != len2);
//                qDebug("len2 %d",len2);
                fImageInfo.frameSize = data_len;
                fImageInfo.pBuffer = image0;
                fImageInfo.frameType = FACE_VIDEO;
                Buffer_Queue_AddData(fImageHandle, FACE_VIDEO, BUFFER_TYPE_VIDEO, sizeof(VIDEO_FRAME_INFO_T),
                                         (char *)&fImageInfo,fImageInfo.frameSize,(char *)fImageInfo.pBuffer);
            break;
            case FRONT_VIDEO:
                do
                {
                    len1 = read(accept_fd,image1 + len2,data_len - len2);
                    len2 = len2 + len1;
//                    qDebug("len1 %d  len2 %d \n",len1,len2);
                }while(data_len != len2);
//                qDebug("len2 %d",len2);
                fImageInfo1.frameSize = data_len;
                fImageInfo1.pBuffer = image1;
                fImageInfo1.frameType = FRONT_VIDEO;
                Buffer_Queue_AddData(fImageHandle1, FRONT_VIDEO, BUFFER_TYPE_VIDEO, sizeof(VIDEO_FRAME_INFO_T),
                                         (char *)&fImageInfo1,fImageInfo1.frameSize,(char *)fImageInfo1.pBuffer);
                break;
            case BACK_VIDEO:
                do
                {
                    len1 = read(accept_fd,image2 + len2,data_len - len2);
                    len2 = len2 + len1;
//                    qDebug("len1 %d  len2 %d \n",len1,len2);
                }while(data_len != len2);
//                qDebug("len2 %d",len2);
                fImageInfo2.frameSize = data_len;
                fImageInfo2.pBuffer = image2;
                fImageInfo2.frameType = BACK_VIDEO;
                Buffer_Queue_AddData(fImageHandle2, BACK_VIDEO, BUFFER_TYPE_VIDEO, sizeof(VIDEO_FRAME_INFO_T),
                                         (char *)&fImageInfo2,fImageInfo2.frameSize,(char *)fImageInfo2.pBuffer);
                break;
            case LEFT_VIDEO:
                do
                {
                    len1 = read(accept_fd,image3 + len2,data_len - len2);
                    len2 = len2 + len1;
//                    qDebug("len1 %d  len2 %d \n",len1,len2);
                }while(data_len != len2);
//                qDebug("len2 %d",len2);
                fImageInfo3.frameSize = data_len;
                fImageInfo3.pBuffer = image3;
                fImageInfo3.frameType = LEFT_VIDEO;
                Buffer_Queue_AddData(fImageHandle3, LEFT_VIDEO, BUFFER_TYPE_VIDEO, sizeof(VIDEO_FRAME_INFO_T),
                                         (char *)&fImageInfo3,fImageInfo3.frameSize,(char *)fImageInfo3.pBuffer);
                break;
            case RIGHT_VIDEO:
                do
                {
                    len1 = read(accept_fd,image4 + len2,data_len - len2);
                    len2 = len2 + len1;
//                    qDebug("len1 %d  len2 %d \n",len1,len2);
                }while(data_len != len2);
//                qDebug("len2 %d",len2);
                fImageInfo4.frameSize = data_len;
                fImageInfo4.pBuffer = image4;
                fImageInfo4.frameType = RIGHT_VIDEO;
                Buffer_Queue_AddData(fImageHandle4, RIGHT_VIDEO, BUFFER_TYPE_VIDEO, sizeof(VIDEO_FRAME_INFO_T),
                                         (char *)&fImageInfo4,fImageInfo4.frameSize,(char *)fImageInfo4.pBuffer);
                break;
            case TERMINAL_BIND:
                read(accept_fd,&bind_value,data_len);
                if(bind_value == 500)
                {
                    qDebug("11111111111111");
                    emit widgetTo0();
                }
                else if(bind_value == 200)
                {
                    emit widgetTo0_1();
                }
                break;
            case EMPLOYEE_BIND:
                read(accept_fd,driver_info,data_len);
//                qDebug("lllllll    %d  %d    lllllll",driver_info[0],driver_info[1]);
                emit widgetTo4();
                break;
            case QR_CODE:
                read(accept_fd,recv_buf,data_len);
                str = QString::fromUtf8(recv_buf);
                emit widgetTo1(str);
                break;
            case CHECK_TO_DAY:
                read(accept_fd,recv_buf,data_len);
                if(data_len == 5){
                    qDebug()<<"day check faild";
//                    emit widgetTo2();
                }
                else if(data_len == 4)
                {
                    qDebug()<<"day check succees";
                    emit widgetTo2();
                }

                break;
            case ELECTRONIC_FENCE:
                break;
            case SPEED_LIMITED:
                break;
            case LOCATION:
                break;
            case ILLEGAL_OPERATIONS:
                break;
            case anquandai:
                read(accept_fd,recv_buf,data_len);
                emit eventTo0();
                break;
            case anquanmao:
                read(accept_fd,recv_buf,data_len);
                emit eventTo1();
                break;
            case dianziweilan:
                read(accept_fd,recv_buf,data_len);
                emit eventTo2();
                break;
            case renyuanjiejin:
                read(accept_fd,&camera_id,data_len);
                emit eventTo3(camera_id);
                break;
            case weishouquanjiashi:
                read(accept_fd,recv_buf,data_len);
                emit eventTo4();
                break;
            case wuzhengjiashi:
                read(accept_fd,recv_buf,data_len);
                emit eventTo5();
                break;
            case pilaojiashi:
                read(accept_fd,recv_buf,data_len);
                emit eventTo6();
                break;
            case chaosubaojing:
                read(accept_fd,recv_buf,data_len);
                emit eventTo7();
                break;
            default:
                break;
        }

    }

}

void tcpServer::stopWork()
{
//    fTcpServer->close();
    emit endWorkClear();
}

void tcpServer::send_image_data(int image_type, int image_len,unsigned char *buff)
{
    write(accept_fd,&image_type,4);
    write(accept_fd,&image_len,4);
    write(accept_fd,buff,len);
}


//void tcpServer::newConnection_Slot()
//{
//    /* 获得已经连接的客户端的socket */
//    fTcpSocket = fTcpServer->nextPendingConnection();
//    /* 获得了socket之后就可以进行读写的操作 */
//    connect(fTcpSocket, SIGNAL(readyRead()), this, SLOT(readyRead_Slot()));
//}

//void tcpServer::readyRead_Slot()
//{
//    QString buf;

//    buf = fTcpSocket->readAll();
//    qDebug() << buf;
//}


//void tcpServer::rcv_data() //收到数据
//{
//    QByteArray data = m_socket->readAll();
//    qDebug()<<"data:"<<data;
//}

//void tcpServer::connect_success()
//{
//    qDebug("111111111111");
//    m_socket->write("3");
//}

//void tcpServer::disconnect_from_server()
//{
//}

//void tcpServer::reconect_to_server()
//{
//    qDebug("222222222");
//    reconnect_timer->stop();
//    if(m_socket)
//        m_socket->connectToServer("wuyiao");
//}

//void tcpServer::error_proc(QLocalSocket::LocalSocketError state)
//{
//    m_socket->close();
//    reconnect_timer->start(1000);  //1s后重连
//}

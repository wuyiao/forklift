#include <QDebug>
#include "socketServer.hpp"

#pragma execution_character_set("utf-8")

socketServer::socketServer(QWidget *parent):
    QWidget(parent)
{
    tcpServer = NULL;
    tcpSocket = NULL;
    //监听套接字，指定父对象，让其自动回收空间
    tcpServer = new QTcpServer(this);
    //1.QTcpServer对象负责监听是否有客户端连接此服务器。它是通过这样一个函数监听的
    //第一个参数表示服务器监听的地址，如果后面是Any表示监听本机的所有网口的，第二个参数表示监听的网络端口
    tcpServer->listen(QHostAddress::LocalHost, 7777);
//    setWindowTitle("Sever");
    //2.如果服务器监听到有客户端和它进行连接，服务器就会触发newConnection这个信号。同时客户端一旦和服务器连接成功，
    //客户端会触发connected这个信号，表示已经成功和服务器连接。
    connect(tcpServer, &QTcpServer::newConnection,
            [=]()
            {
                //3.当服务器和客户端建立好连接之后，服务器需要返回一个QTcpSocket对象来和客户端进行通信，通常通过这个函数来返回一个建立好连接的套接字。
                //客户端和服务器各有一个通信套接字，可以看出，每一方的套接字都可以获得对方的相关信息
                //接着就可以通过两个的通信套接字来完成通信。
                //取出建立好连接的套接字
                tcpSocket = tcpServer->nextPendingConnection();
                //获取对方的IP和端口
//                QString ip = tcpSocket->peerAddress().toString();
//                qint16 port = tcpSocket->peerPort();
//                QString temp = QString("[%1:%2]:成功连接").arg(ip).arg(port);

//                ui->textEditRead->setText(temp);
                //4.当一端发送成功之后，"接收方"会触发这么一个信号，readyRead，这样我们就能够读取套接字当中的内容了。
                connect(tcpSocket, &QTcpSocket::readyRead,
                        [=]()
                        {
                            //从通信套接字中取出内容
                           QByteArray array = tcpSocket->readAll();
//                           ui->textEditRead->append(array);
                           qDebug() << array;
                        }
               );
             }
             );
}

socketServer::~socketServer()
{
//    delete ui;
}

void socketServer::on_buttonSend_clicked()
{
     if(NULL == tcpSocket)
     {
         return;
     }
     //获取编辑区内容
     QString str = "ui->textEditWrite->toPlainText()";
     //给对方发送数据， 使用套接字是tcpSocket
     tcpSocket->write(str.toUtf8().data() );
}

void socketServer::on_buttonClose_clicked()
{
    if(NULL == tcpSocket)
     {
         return;
     }
     //主动和客户端端口连接
     tcpSocket->disconnectFromHost();
     tcpSocket->close();
     tcpSocket = NULL;
}

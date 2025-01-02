#ifndef SOCKETSERVER_HPP
#define SOCKETSERVER_HPP

#include <QWidget>
#include <QTcpServer> //监听套接字
#include <QTcpSocket> //通信套接字

class socketServer: public QWidget
{
    Q_OBJECT

public:
    explicit socketServer(QWidget *parent = 0);
    ~socketServer();

private slots:
    void on_buttonSend_clicked();

    void on_buttonClose_clicked();

private:
    QTcpServer *tcpServer; //监听套接字
    QTcpSocket *tcpSocket; //通信套接字
};

#endif // SOCKETSERVER_HPP

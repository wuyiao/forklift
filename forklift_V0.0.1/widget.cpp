#include <vector>
#include <QPushButton>
#include <unistd.h>
#include "widget.h"
#include "ui_widget.h"
#include "qrcodegen.hpp"
#include <QDebug>
#include <QFile>
#include <QSound>
#include <QAudioDeviceInfo>
#include <QAudioOutput>
#include "socketServer.hpp"

using std::uint8_t;
using qrcodegen::QrCode;
using qrcodegen::QrSegment;

Widget::Widget(QWidget *parent)
    : QWidget(parent),
      audio(nullptr),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    InitWidget();
    for (int i = 0; i < 5; i++) {
        fVideoThread[i] = nullptr;
        fVideoWorkThread[i] = nullptr;
    }
            fTcpServer = nullptr;
            fTcpServerWorkThread = nullptr;


    widgetWorkThreadInit();

    bind_status = 0;
    int a = 0;
    QAudioDeviceInfo device;
    const auto deviceInfos = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    for (const QAudioDeviceInfo &deviceInfo : deviceInfos)
    {
        device = deviceInfo;
        a++;
        if(a == 3)
            break;
    }
    // 创建音频格式对象
    QAudioFormat format;
    format.setSampleRate(16000);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    // 创建音频输出对象
    audio = new QAudioOutput(device,format);

//    widget4();
}

Widget::~Widget()
{
    widgetworkThreadDestory();
    delete ui;
}

void Widget::widgetWorkThreadInit()
{
    for (int i = 0;i < 5;i++) {
        if (!fVideoWorkThread[i]) {
            fVideoWorkThread[i] = new QThread;
        }

        if (!fVideoThread[i]) {
            if(i == FACE_VIDEO)
            {
                fVideoThread[i] = new videoWorkThread(FACE_VIDEO);
                if (!fVideoThread[i]) {
                    return;
                }
                fVideoThread[i]->fVideoDisplay = this->ui->label2;
//                fVideoThread[i]->fVideoDisplay = this->ui->label4_1;
            }
            else if(i == FRONT_VIDEO)
            {
                fVideoThread[i] = new videoWorkThread(FRONT_VIDEO);
                if (!fVideoThread[i]) {
                    return;
                }
                fVideoThread[i]->fVideoDisplay = this->ui->label4_1;
            }
            else if(i == BACK_VIDEO)
            {
                fVideoThread[i] = new videoWorkThread(BACK_VIDEO);
                if (!fVideoThread[i]) {
                    return;
                }
                fVideoThread[i]->fVideoDisplay = this->ui->label4_2;
            }
            else if(i == LEFT_VIDEO)
            {
                fVideoThread[i] = new videoWorkThread(LEFT_VIDEO);
                if (!fVideoThread[i]) {
                    return;
                }
                fVideoThread[i]->fVideoDisplay = this->ui->label4_3;
            }
            else if(i == RIGHT_VIDEO)
            {
                fVideoThread[i] = new videoWorkThread(RIGHT_VIDEO);
                if (!fVideoThread[i]) {
                    return;
                }
                fVideoThread[i]->fVideoDisplay = this->ui->label4_4;
            }

        }


        fVideoThread[i]->moveToThread(fVideoWorkThread[i]);
        connect(this,&Widget::videoWorkThreadStartWork,fVideoThread[i],&videoWorkThread::doWork);
        connect(this,&Widget::widgetThreadStopWork,fVideoThread[i],&videoWorkThread::stopWork,Qt::DirectConnection);
        connect(fVideoThread[i],&videoWorkThread::endWorkClear,this,&Widget::widgetWorkThreadClear);
        connect(fVideoThread[i],&videoWorkThread::sendImageData,fVideoThread[i],&videoWorkThread::playVideo);


    }

            if (!fTcpServerWorkThread) {
                fTcpServerWorkThread = new QThread;
            }

            if (!fTcpServer) {
                fTcpServer = new tcpServer();
                if (!fTcpServer) {
                    return;
                }
            }

            fVideoThread[0]->fImageData = fTcpServer->fImageHandle;
            fVideoThread[1]->fImageData = fTcpServer->fImageHandle1;
            fVideoThread[2]->fImageData = fTcpServer->fImageHandle2;
            fVideoThread[3]->fImageData = fTcpServer->fImageHandle3;
            fVideoThread[4]->fImageData = fTcpServer->fImageHandle4;


            fTcpServer->moveToThread(fTcpServerWorkThread);
            connect(this,&Widget::tcpServerWorkThreadStartWork,fTcpServer,&tcpServer::doWork);
            connect(this,&Widget::widgetThreadStopWork,fTcpServer,&tcpServer::stopWork,Qt::DirectConnection);
            connect(fTcpServer,&tcpServer::endWorkClear,this,&Widget::widgetWorkThreadClear);
//            connect(fTcpServer, SIGNAL(newConnection()), fTcpServer, SLOT(newConnection_Slot()));

            connect(fTcpServer,&tcpServer::widgetTo0,this,&Widget::widget0);
            connect(fTcpServer,&tcpServer::widgetTo0_1,this,&Widget::widget0_1);
            connect(fTcpServer,&tcpServer::widgetTo1,this,&Widget::widget1);
            connect(fTcpServer,&tcpServer::widgetTo2,this,&Widget::widget2);
            connect(fTcpServer,&tcpServer::widgetTo3,this,&Widget::widget3);
            connect(fTcpServer,&tcpServer::widgetTo4,this,&Widget::widget4);


            connect(fTcpServer,&tcpServer::eventTo0,this,&Widget::event0);
            connect(fTcpServer,&tcpServer::eventTo1,this,&Widget::event1);
            connect(fTcpServer,&tcpServer::eventTo2,this,&Widget::event2);
            connect(fTcpServer,&tcpServer::eventTo3,this,&Widget::event3);
            connect(fTcpServer,&tcpServer::eventTo4,this,&Widget::event4);
            connect(fTcpServer,&tcpServer::eventTo5,this,&Widget::event5);
            connect(fTcpServer,&tcpServer::eventTo6,this,&Widget::event6);
            connect(fTcpServer,&tcpServer::eventTo7,this,&Widget::event7);



    widgetWorkThreadRun();

}

void Widget::widgetWorkThreadRun()
{
    for (int i = 0;i < 5;i++) {
        if(!fVideoWorkThread[i]->isRunning())
        {
            qDebug("videoWorkThread %d\n",i);
            fVideoWorkThread[i]->start();
        }
    }
            if(!fTcpServerWorkThread->isRunning())
            {
                qDebug("tcpServerWorkThread \n");
                fTcpServerWorkThread->start();
            }

    emit videoWorkThreadStartWork();
    emit tcpServerWorkThreadStartWork();
    usleep(50000);
    qDebug("thread start \n");
}

void Widget::widgetWorkThreadClear()
{
    auto worker = sender();

        if(worker == fTcpServer) {
            qDebug("Terminate fTcpServer Process workThread");
            fTcpServerWorkThread->quit();
            fTcpServerWorkThread->wait();
        }

    if(worker == fVideoThread[0]) {
        qDebug("Terminate fVideoThread0 Process workThread");
        fVideoWorkThread[0]->quit();
        fVideoWorkThread[0]->wait();
    }else if (worker == fVideoThread[1]) {
        qDebug("Terminate fVideoThread1 Process workThread");
        fVideoWorkThread[1]->quit();
        fVideoWorkThread[1]->wait();
    }else if (worker == fVideoThread[2]) {
        qDebug("Terminate fVideoThread2 Process workThread");
        fVideoWorkThread[2]->quit();
        fVideoWorkThread[2]->wait();
    }else if (worker == fVideoThread[3]) {
        qDebug("Terminate fVideoThread3 Process workThread");
        fVideoWorkThread[3]->quit();
        fVideoWorkThread[3]->wait();
    }else if (worker == fVideoThread[4]) {
        qDebug("Terminate fVideoThread4 Process workThread ");
        fVideoWorkThread[4]->quit();
        fVideoWorkThread[4]->wait();
    }
}

void Widget::widgetworkThreadDestory()
{
    emit widgetThreadStopWork();

    for (int i = 0;i < 5;i++) {
        if(fVideoWorkThread[i]->isRunning())
        {
            fVideoWorkThread[i]->quit();
            fVideoWorkThread[i]->wait();
        }

        fVideoThread[i]->deleteLater();

        if (fVideoThread[i]) {
            delete fVideoThread[i];
            fVideoThread[i] = nullptr;
        }
    }
            if(fTcpServerWorkThread->isRunning())
            {
                fTcpServerWorkThread->quit();
                fTcpServerWorkThread->wait();
            }

            fTcpServer->deleteLater();

            if (fTcpServer) {
                delete fTcpServer;
                fTcpServer = nullptr;
            }

}


void Widget::InitWidget()
{
//    ui->label4_5_1->setStyleSheet("QLabel{"
//                                  "border-image:url(:/image/ui/r1.png);"
//                                  "}");
}

void Widget::showBindQRCode(QString msg)
{
    char * data;

    QByteArray ba = msg.toLatin1();
    data = ba.data();
    std::vector<QrSegment> segs =
        QrSegment::makeSegments(data);

    QrCode qr1 = QrCode::encodeSegments(
        segs, QrCode::Ecc::LOW, 5, 5, 2, false);

    QImage QrCode_Image=QImage(qr1.getSize(),qr1.getSize(),QImage::Format_RGB888);
    QrCode_Image.fill(Qt::transparent);
    for (int y = 0; y < qr1.getSize(); y++) {
        for (int x = 0; x < qr1.getSize(); x++) {
            if(qr1.getModule(x, y))
                QrCode_Image.setPixel(x,y,qRgb(0,0,0));
            else
                QrCode_Image.setPixel(x,y,qRgb(255,255,255));
        }
    }

    QrCode_Image=QrCode_Image.scaled(ui->label0->width(), ui->label0->height(),
                    Qt::KeepAspectRatio);

    ui->label0->setPixmap(QPixmap::fromImage(QrCode_Image));
    ui->label0->setAlignment(Qt::AlignCenter);
}



void Widget::showCheckQRCode(QString msg)
{

    std::string str = msg.toStdString();
    const char* ch = str.c_str();
    std::vector<QrSegment> segs =
        QrSegment::makeSegments(ch);

    QrCode qr1 = QrCode::encodeSegments(
        segs, QrCode::Ecc::LOW, 5, 5, 2, false);

    QImage QrCode_Image=QImage(qr1.getSize(),qr1.getSize(),QImage::Format_RGB888);
    QrCode_Image.fill(Qt::transparent);
    for (int y = 0; y < qr1.getSize(); y++) {
        for (int x = 0; x < qr1.getSize(); x++) {
            if(qr1.getModule(x, y))
                QrCode_Image.setPixel(x,y,qRgb(0,0,0));
            else
                QrCode_Image.setPixel(x,y,qRgb(255,255,255));
        }
    }

    QrCode_Image=QrCode_Image.scaled(ui->label1->width(), ui->label1->height(),
                    Qt::KeepAspectRatio);

    ui->label1->setPixmap(QPixmap::fromImage(QrCode_Image));
    ui->label1->setAlignment(Qt::AlignCenter);
}

void Widget::showTest(QString msg)
{
    char * data;

    QByteArray ba = msg.toLatin1();
    data = ba.data();
    std::vector<QrSegment> segs =
        QrSegment::makeSegments(data);

    QrCode qr1 = QrCode::encodeSegments(
        segs, QrCode::Ecc::HIGH, 5, 5, 2, false);

    QImage QrCode_Image=QImage(qr1.getSize(),qr1.getSize(),QImage::Format_RGB888);
    QrCode_Image.fill(Qt::transparent);
    for (int y = 0; y < qr1.getSize(); y++) {
        for (int x = 0; x < qr1.getSize(); x++) {
            if(qr1.getModule(x, y))
                QrCode_Image.setPixel(x,y,qRgb(0,0,0));
            else
                QrCode_Image.setPixel(x,y,qRgb(255,255,255));
        }
    }

    QrCode_Image=QrCode_Image.scaled(ui->label2->width(), ui->label2->height(),
                    Qt::KeepAspectRatio);

    ui->label2->setPixmap(QPixmap::fromImage(QrCode_Image));
    ui->label2->setAlignment(Qt::AlignCenter);
}

void Widget::widget0()
{
    uuid_data();
    showBindQRCode(fUuidTime);
    ui->widget1->setVisible(false);
    ui->widget2->setVisible(false);
    ui->widget3->setVisible(false);
    ui->widget4->setVisible(false);
    ui->widget0->setVisible(true);
    bind_status = 1;
    QFile file("/userdata/audio/noBind.wav");
    // 打开音频文件
    if (!file.open(QIODevice::ReadOnly)) {
        return ;
    }
    // 打开音频设备
    audio->start(&file);
    // 等待播放结束
    while (audio->state() == QAudio::ActiveState) {
        QCoreApplication::processEvents();
    }
    // 关闭音频设备
    audio->stop();

}

void Widget::widget0_1()
{
    ui->label0->setText("已绑定");
//    if(bind_status == 1)
//    {
//        QFile file("/userdata/audio/bind_succees.wav");
//        // 打开音频文件
//        if (!file.open(QIODevice::ReadOnly)) {
//            return ;
//        }
//        // 打开音频设备
//        audio->start(&file);
//        // 等待播放结束
//        while (audio->state() == QAudio::ActiveState) {
//            QCoreApplication::processEvents();
//        }
//        // 关闭音频设备
//        audio->stop();
//    }
//    else
//    {
//        QFile file("/userdata/audio/yesBind.wav");
//        // 打开音频文件
//        if (!file.open(QIODevice::ReadOnly)) {
//            return ;
//        }
//        // 打开音频设备
//        audio->start(&file);
//        // 等待播放结束
//        while (audio->state() == QAudio::ActiveState) {
//            QCoreApplication::processEvents();
//        }
//        // 关闭音频设备
//        audio->stop();
//    }
//    sleep(2);
}

void Widget::widget1(QString msg)
{
    showCheckQRCode(msg);
    ui->widget0->setVisible(false);
    ui->widget2->setVisible(false);
    ui->widget3->setVisible(false);
    ui->widget4->setVisible(false);
    ui->widget1->setVisible(true);

    QFile file("/userdata/audio/dayCheck.wav");
    // 打开音频文件
    if (!file.open(QIODevice::ReadOnly)) {
        return ;
    }
    // 打开音频设备
    audio->start(&file);
    // 等待播放结束
    while (audio->state() == QAudio::ActiveState) {
        QCoreApplication::processEvents();
    }
    // 关闭音频设备
    audio->stop();
//    sleep(2);

}

void Widget::widget2()
{
    ui->widget0->setVisible(false);
    ui->widget1->setVisible(false);
    ui->widget3->setVisible(false);
    ui->widget4->setVisible(false);
    ui->widget2->setVisible(true);
    QFile file("/userdata/audio/dayCheckSuccees.wav");
    // 打开音频文件
    if (!file.open(QIODevice::ReadOnly)) {
        return ;
    }
    // 打开音频设备
    audio->start(&file);
    // 等待播放结束
    while (audio->state() == QAudio::ActiveState) {
        QCoreApplication::processEvents();
    }
    // 关闭音频设备
    audio->stop();
//    sleep(2);
}

void Widget::widget3()
{
    ui->widget0->setVisible(false);
    ui->widget1->setVisible(false);
    ui->widget2->setVisible(false);
    ui->widget4->setVisible(false);
    ui->widget3->setVisible(true);

}

void Widget::widget4()
{
    faceInfo();
    ui->widget0->setVisible(false);
    ui->widget1->setVisible(false);
    ui->widget2->setVisible(false);
    ui->widget3->setVisible(false);
    ui->widget4->setVisible(true);
//    QFile file("/userdata/audio/faceCheckSuccees.wav");
//    // 打开音频文件
//    if (!file.open(QIODevice::ReadOnly)) {
//        return ;
//    }
//    // 打开音频设备
//    audio->start(&file);
//    // 等待播放结束
//    while (audio->state() == QAudio::ActiveState) {
//        QCoreApplication::processEvents();
//    }
//    // 关闭音频设备
//    audio->stop();
//    sleep(2);
}

void Widget::faceInfo()
{
    QString car_id;
    QString driver_name;
    QImage img;
    QPixmap pix;
    char filename[100] = {0};
    sprintf(filename,"/userdata/image/image%d.jpg",fTcpServer->driver_info[0]);
    img.load(filename);
    qDebug()<<filename;
    pix.convertFromImage(img.scaled(108, 108));

    if(pix.isNull()){
        qDebug() << "invalid pixmap";
        return ;
    }
    ui->label4_6_1->setPixmap(pix);

    QPalette white;
    white.setColor(QPalette::WindowText,Qt::white);
    white.setColor(QPalette::Background,Qt::transparent);

    QString path = "/userdata/driver_name";
    QString path1 = "/userdata/car_id";
    if(path.isEmpty() == false && path1.isEmpty() == false)
    {
        //创建文件对象
        QFile file(path);
        QFile file1(path1);

        bool isok = file.open(QIODevice::ReadOnly); //只读模式打开
        if(isok == true)
        {

             //读文件
            QByteArray array =  file.readAll();
            array = array.trimmed();
            driver_name = QString(array);
        }
        file.close();
        bool isok1 = file1.open(QIODevice::ReadOnly); //只读模式打开
        if(isok1 == true)
        {

             //读文件
            QByteArray array1 =  file1.readAll();
            array1 = array1.trimmed();
            car_id = QString(array1);
        }
        file.close();
    }

    ui->label4_6_2->setPalette(white);
    ui->label4_6_2->setText("人员姓名：");
    ui->label4_6_3->setPalette(white);
    ui->label4_6_3->setText("人员证件：");
    ui->label4_6_4->setPalette(white);
    ui->label4_6_4->setText("车  牌  号 ：");
    ui->label4_6_5->setPalette(white);
    ui->label4_6_5->setText(driver_name);
    ui->label4_6_6->setPalette(white);
    if(fTcpServer->driver_info[1] == 0)
    {
        ui->label4_6_6->setText("无证");
    }
    else if(fTcpServer->driver_info[1] == 1)
    {
        ui->label4_6_6->setText("正常");
    }
    else if(fTcpServer->driver_info[1] == 2)
    {
        ui->label4_6_6->setText("临期");
    }
    else if(fTcpServer->driver_info[1] == 3)
    {
        ui->label4_6_6->setText("逾期");
    }

    ui->label4_6_7->setPalette(white);
    ui->label4_6_7->setText(car_id);
}

void Widget::event0()
{
    ui->label4_5_2_1->show();
    ui->label4_5_3_1->show();
    ui->label4_5_4_1->show();
    ui->label4_5_5_1->show();
    ui->label4_5_6_1->show();
    ui->label4_5_7_1->show();
    ui->label4_5_8_1->show();
    ui->label4_5_1_1->hide();
    QFile file("/userdata/audio/safetyBelt.wav");
    // 打开音频文件
    if (!file.open(QIODevice::ReadOnly)) {
        return ;
    }
    // 打开音频设备
    audio->start(&file);
    // 等待播放结束
    while (audio->state() == QAudio::ActiveState) {
        QCoreApplication::processEvents();
    }
    // 关闭音频设备
    audio->stop();
//    sleep(2);
}

void Widget::event1()
{
    ui->label4_5_1_1->show();
    ui->label4_5_3_1->show();
    ui->label4_5_4_1->show();
    ui->label4_5_5_1->show();
    ui->label4_5_6_1->show();
    ui->label4_5_7_1->show();
    ui->label4_5_8_1->show();
    ui->label4_5_2_1->hide();
    QFile file("/userdata/audio/helmet.wav");
    // 打开音频文件
    if (!file.open(QIODevice::ReadOnly)) {
        return ;
    }
    // 打开音频设备
    audio->start(&file);
    // 等待播放结束
    while (audio->state() == QAudio::ActiveState) {
        QCoreApplication::processEvents();
    }
    // 关闭音频设备
    audio->stop();
//    sleep(2);
}

void Widget::event2()
{
    ui->label4_5_1_1->show();
    ui->label4_5_2_1->show();
    ui->label4_5_4_1->show();
    ui->label4_5_5_1->show();
    ui->label4_5_6_1->show();
    ui->label4_5_7_1->show();
    ui->label4_5_8_1->show();
    ui->label4_5_3_1->hide();
    QFile file("/userdata/audio/outOfRange.wav");
    // 打开音频文件
    if (!file.open(QIODevice::ReadOnly)) {
        return ;
    }
    // 打开音频设备
    audio->start(&file);
    // 等待播放结束
    while (audio->state() == QAudio::ActiveState) {
        QCoreApplication::processEvents();
    }
    // 关闭音频设备
    audio->stop();
//    sleep(2);
}

void Widget::event3(int camera_id)
{

    char filename[100] = {0};
    if(camera_id != 0)
    {
        ui->label4_5_1_1->show();
        ui->label4_5_3_1->show();
        ui->label4_5_2_1->show();
        ui->label4_5_5_1->show();
        ui->label4_5_6_1->show();
        ui->label4_5_7_1->show();
        ui->label4_5_8_1->show();
        ui->label4_5_4_1->hide();


        switch (camera_id) {
        case 1:
            sprintf(filename,"/userdata/audio/front.wav");
            break;
        case 2:
            sprintf(filename,"/userdata/audio/back.wav");
            break;
        case 3:
            sprintf(filename,"/userdata/audio/right.wav");
            break;
        case 4:
            sprintf(filename,"/userdata/audio/left.wav");
            break;
        default:
            break;
        }
        QFile file(filename);
        // 打开音频文件
        if (!file.open(QIODevice::ReadOnly)) {
            return ;
        }
        // 打开音频设备
        audio->start(&file);
        // 等待播放结束
        while (audio->state() == QAudio::ActiveState) {
            QCoreApplication::processEvents();
        }
        // 关闭音频设备
        audio->stop();
    //    sleep(2);
    }

}

void Widget::event4()
{
    ui->label4_5_1_1->show();
    ui->label4_5_3_1->show();
    ui->label4_5_4_1->show();
    ui->label4_5_2_1->show();
    ui->label4_5_6_1->show();
    ui->label4_5_7_1->show();
    ui->label4_5_8_1->show();
    ui->label4_5_5_1->hide();
    QFile file("/userdata/audio/noLicence.wav");
    // 打开音频文件
    if (!file.open(QIODevice::ReadOnly)) {
        return ;
    }
    // 打开音频设备
    audio->start(&file);
    // 等待播放结束
    while (audio->state() == QAudio::ActiveState) {
        QCoreApplication::processEvents();
    }
    // 关闭音频设备
    audio->stop();
//    sleep(2);
}

void Widget::event5()
{
    ui->label4_5_1_1->show();
    ui->label4_5_3_1->show();
    ui->label4_5_4_1->show();
    ui->label4_5_5_1->show();
    ui->label4_5_2_1->show();
    ui->label4_5_7_1->show();
    ui->label4_5_8_1->show();
    ui->label4_5_6_1->hide();
    QFile file("/userdata/audio/noCard.wav");
    // 打开音频文件
    if (!file.open(QIODevice::ReadOnly)) {
        return ;
    }
    // 打开音频设备
    audio->start(&file);
    // 等待播放结束
    while (audio->state() == QAudio::ActiveState) {
        QCoreApplication::processEvents();
    }
    // 关闭音频设备
    audio->stop();
//    sleep(2);
}

void Widget::event6()
{
    ui->label4_5_1_1->show();
    ui->label4_5_3_1->show();
    ui->label4_5_4_1->show();
    ui->label4_5_5_1->show();
    ui->label4_5_6_1->show();
    ui->label4_5_2_1->show();
    ui->label4_5_8_1->show();
    ui->label4_5_7_1->hide();
    QFile file("/userdata/audio/tired.wav");
    // 打开音频文件
    if (!file.open(QIODevice::ReadOnly)) {
        return ;
    }
    // 打开音频设备
    audio->start(&file);
    // 等待播放结束
    while (audio->state() == QAudio::ActiveState) {
        QCoreApplication::processEvents();
    }
    // 关闭音频设备
    audio->stop();
//    sleep(2);
}

void Widget::event7()
{
    ui->label4_5_1_1->show();
    ui->label4_5_3_1->show();
    ui->label4_5_4_1->show();
    ui->label4_5_5_1->show();
    ui->label4_5_6_1->show();
    ui->label4_5_7_1->show();
    ui->label4_5_2_1->show();
    ui->label4_5_8_1->hide();
    QFile file("/userdata/audio/speed.wav");
    // 打开音频文件
    if (!file.open(QIODevice::ReadOnly)) {
        return ;
    }
    // 打开音频设备
    audio->start(&file);
    // 等待播放结束
    while (audio->state() == QAudio::ActiveState) {
        QCoreApplication::processEvents();
    }
    // 关闭音频设备
    audio->stop();
//    sleep(2);
}

void Widget::uuid_data()
{
    QString path = "/userdata/uuid";
    if(path.isEmpty() == false)
    {
        //创建文件对象
        QFile file(path);

        bool isok = file.open(QIODevice::ReadOnly); //只读模式打开
        if(isok == true)
        {

             //读文件
            QByteArray array =  file.readAll();
            array = array.trimmed();
            fUuidTime = QString(array);
            qDebug() << fUuidTime;


        }
        file.close();
    }
}



#include <unistd.h>
#include <sys/syscall.h>
#include "videoWorkThread.h"
#include <QDebug>
#include "LPF_V4L2.h"
#include "buffer_queue.h"

//int a =0;
#define gettid() syscall(__NR_gettid)

videoWorkThread::videoWorkThread(VideoNameText videoNameType):
    QThread(),
    fVideoDisplay(nullptr),
    videoWorkThreadStatu(false)
{
//    a++;
    fCamCurrPlayPos = videoNameType;

    if (videoNameType == FACE_VIDEO) {
        fColumePoint = 94;
        fRowPoint = 252;
        fHeight = 432;
        fWidth = 432;
        fVideoPlayFlag0 = 0;
    }else if (videoNameType == FRONT_VIDEO) {
        fColumePoint = 208;
        fRowPoint = 69;
        fHeight = 140;
        fWidth = 188;
        fVideoPlayFlag1 = 0;
    }else if (videoNameType == BACK_VIDEO) {
        fColumePoint = 208;
        fRowPoint = 221;
        fHeight = 140;
        fWidth = 188;
        fVideoPlayFlag2 = 0;
    }else if (videoNameType == LEFT_VIDEO) {
        fColumePoint = 12;
        fRowPoint = 69;
        fHeight = 140;
        fWidth = 188;
        fVideoPlayFlag3 = 0;
    }else if (videoNameType == RIGHT_VIDEO) {
        fColumePoint = 403;
        fRowPoint = 69;
        fHeight = 140;
        fWidth = 188;
        fVideoPlayFlag4 = 0;
    }
    fVideoDisplay = new QLabel();

//    if(a == 1)
//        videoRun();
}

videoWorkThread::~videoWorkThread()
{
    stopWork();
}

void videoWorkThread::doWork()
{
    VIDEO_FRAME_INFO_T DepthVideoFrameInfo;
    unsigned int fBufferChan = 0;
    BUFFER_TYPE_ENUM fBufferType = BUFFER_TYPE_VIDEO;
    int fHeaderSize=0,fDataSize=0;
    char *fFrameBuff = (char *)malloc(0x1000000);
    if(fFrameBuff == NULL)
    {
        qDebug("fFrameBuff malloc Failure.....\n");
        return;
    }
    if(fImageData)
    {
        fImageDataC = Buffer_Queue_Consumer_Register(fImageData, gettid(), 0);
        if (!fImageDataC) {
            qDebug("DepthVideoFrameSend_ConsumerHandle create Falid\n");
        }
    }

    while(1)
    {
        usleep(1000);
        int ret = -1;
        ret = Buffer_Queue_GetData(fImageData,
                             fImageDataC,
                             &fBufferChan,&fBufferType,
                             &fHeaderSize,(char *)&DepthVideoFrameInfo,
                             &fDataSize,fFrameBuff);
        if(ret == 0)
        {
            if(DepthVideoFrameInfo.frameType == (unsigned int)fCamCurrPlayPos)
            {
//                qDebug("1111111111111  %d  22222222222222222  %d\n",DepthVideoFrameInfo.frameType,fCamCurrPlayPos);
                if(fCamCurrPlayPos == 0)
                {
                    QImage img;
                    img = QImage((unsigned char *)fFrameBuff, 640, 480, QImage::Format_RGB888);
                    emit sendImageData(img.copy());
                }
                else
                {
                    QImage img;
                    img = QImage((unsigned char *)fFrameBuff, 1280, 720, QImage::Format_RGB888);
                    emit sendImageData(img.copy());
                }

            }

        }
    }
}

void videoWorkThread::stopWork()
{
    videoWorkThreadStatu = true;
    emit endWorkClear();
}


void videoWorkThread::playVideo(QImage image)
{
    QPixmap pix;

    pix.convertFromImage(image.scaled(fWidth, fHeight));
    if(pix.isNull()){
        qDebug() << "invalid pixmap";
        return ;
    }
    fVideoDisplay->setPixmap(pix);
}



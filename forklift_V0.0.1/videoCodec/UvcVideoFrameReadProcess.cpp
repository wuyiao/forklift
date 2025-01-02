#include "UvcVideoFrameReadProcess.h"
#include <QDebug>
#include <QTime>

UvcVideoFrameReadProcess::UvcVideoFrameReadProcess(QObject *parent) :
    QThread(parent),
    fUvcVideoFrameReadProcess_ProduceHandle(nullptr),
    fUvcVideoFrameReadProcessThreadStatu(false)
{
    int ret  = -1;
    if (!fUvcVideoFrameReadProcess_ProduceHandle) {
        ret = Buffer_Queue_Producer_Create(&fUvcVideoFrameReadProcess_ProduceHandle, 0, NULL, 1024*1000, 30000);
        if (ret < 0) {
            qDebug("UvcVideoFrameRead Queue Producer create failed\n");
            return;
        }
    }
}

UvcVideoFrameReadProcess::~UvcVideoFrameReadProcess()
{
    if (fUvcVideoFrameReadProcess_ProduceHandle) {
        Buffer_Queue_Producer_Destroy(&fUvcVideoFrameReadProcess_ProduceHandle);
    }
}

void UvcVideoFrameReadProcess::doWork()
{
    VIDEO_FRAME_INFO_T UvcVideoFrameReadInfo;

    if (!fUvcVideoFrameReadProcess_ProduceHandle) {
        qDebug("UvcVideoFrameReadProcess_ProduceHandle Failed\n");
        return;
    }

    int readTime = 0;
    int readFps = 1000/30;
    int errCount = 0;
    QTime UvcstartTime;
    fUvcVideoFrameReadProcessThreadStatu = false;
    while (!fUvcVideoFrameReadProcessThreadStatu) {
        UvcstartTime = QTime::currentTime();
        //readTime = UvcstartTime.elapsed();
        //qDebug() << "uvc read start time = " << readTime;
        int ret = LPF_ReadFrame();
        if(ret == 0) {
            int WV = LPF_GetCurResWidth();
            int HV = LPF_GetCurResHeight();


            UvcVideoFrameReadInfo.frameSize = outBufSize;
            UvcVideoFrameReadInfo.pBuffer = outBuf;
            UvcVideoFrameReadInfo.width = WV;
            UvcVideoFrameReadInfo.height = HV;
            UvcVideoFrameReadInfo.frameType = outBufFmt;
            Buffer_Queue_AddData(fUvcVideoFrameReadProcess_ProduceHandle, 0, BUFFER_TYPE_VIDEO,
                                 sizeof(VIDEO_FRAME_INFO_T),(char *)&UvcVideoFrameReadInfo,UvcVideoFrameReadInfo.frameSize,
                                 (char *)UvcVideoFrameReadInfo.pBuffer);

            if (LPF_Videoc_Qbuf() < 0) {
                qDebug() << "GetFrame VIDIOC_QBUF Failed\n";
            }

            readTime = UvcstartTime.elapsed();
            //qDebug() << "uvc read end time = " << readTime;
        }

        if (ret == E_SELECT_TIMEOUT_ERR || ret == E_SELECT_ERR) {
            errCount++;
            if (errCount == 50) {
                errCount = 0;
                LPF_StopRun();
                emit showUvcReadMsgSig(ERROR_TYPE, "播放彩色数据失败,设备出现异常，请检查设备");
            }
        } else if (ret == E_NOT_GET_DATA_ERR) {
            errCount++;
            if (errCount == 50) {
                errCount = 0;
                emit showUvcReadMsgSig(ERROR_TYPE, "读取彩色数据出现异常，请检查设备连接或通信是否正常");
            }
        } else {
            errCount = 0;
        }

        if (readTime < readFps) {
            msleep(readFps - readTime);
        }
    }
}


void UvcVideoFrameReadProcess::stopWork()
{
    fUvcVideoFrameReadProcessThreadStatu = true;

    emit endWorkClear();
}

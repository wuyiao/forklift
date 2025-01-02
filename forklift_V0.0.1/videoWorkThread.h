#ifndef VIDEOWORKTHREAD_H
#define VIDEOWORKTHREAD_H
#include <QThread>
#include <QImage>
#include <QLabel>
#include "common.hpp"
#include "buffer_queue.h"

class videoWorkThread : public QThread
{
    Q_OBJECT
public:
    explicit videoWorkThread(VideoNameText videoNameType = FACE_VIDEO);
    ~videoWorkThread();

    void doWork();
    void stopWork();

    void playVideo(QImage image);

    int fColumePoint;
    int fRowPoint;
    int fHeight;
    int fWidth;
    int fVideoPlayFlag;
    int fVideoPlayFlag0;
    int fVideoPlayFlag1;
    int fVideoPlayFlag2;
    int fVideoPlayFlag3;
    int fVideoPlayFlag4;
    int fCamCurrPlayPos;
    QLabel *fVideoDisplay;

    BUFFQUEUE_HANDLE fImageData;
    CONSUMER_HANDLE fImageDataC;

private:
    QImage *fImage;


    bool videoWorkThreadStatu;

signals:
    void sendImageData(QImage image);
    void endWorkClear();

};

#endif // VIDEOWORKTHREAD_H

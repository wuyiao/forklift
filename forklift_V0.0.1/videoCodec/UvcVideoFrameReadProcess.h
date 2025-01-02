#ifndef UVCVIDEOFRAMEREADPROCESS_H
#define UVCVIDEOFRAMEREADPROCESS_H
#include <QThread>
#include "commonConfigInfo.h"
#include "bufferQueue/buffer_queue.h"
#include "LPF_V4L2.h"

class UvcVideoFrameReadProcess : public QThread
{
    Q_OBJECT
public:
    explicit UvcVideoFrameReadProcess(QObject *parent = nullptr);
    ~UvcVideoFrameReadProcess();

    BUFFQUEUE_HANDLE fUvcVideoFrameReadProcess_ProduceHandle;

    void doWork();
    void stopWork();

private:
    bool fUvcVideoFrameReadProcessThreadStatu;

signals:
    void endWorkClear();
    void showUvcReadMsgSig(ShowMsgType , QString);
};

#endif // UVCVIDEOFRAMEREADPROCESS_H_H

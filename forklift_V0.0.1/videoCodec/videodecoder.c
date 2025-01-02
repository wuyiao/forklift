#include <stdio.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>

#include "videodecoder.h"


#define CPrintf printf
struct AVCodecContext *pAVCodecCtxDecoder = NULL;
struct AVCodec *pAVCodecDecoder;
struct AVPacket mAVPacketDecoder;
struct AVFrame *pAVFrameDecoder = NULL;
struct SwsContext* pImageConvertCtxDecoder = NULL;
struct AVFrame *pFrameYUVDecoder = NULL;

unsigned char * buf;

int curWidth = 0;
int curHeight = 0;
void setCurDecode_width_height(int width, int height)
{
    curWidth = width;
    curHeight = height;
}

int ffmpeg_init_video_decoder(AVCodecParameters *codecParameters)
{
    if (!codecParameters) {
        CPrintf("Source codec context is NULL.");   //CPrintf需替换为printf
        return -1;
    }
    ffmpeg_release_video_decoder();

    pAVCodecDecoder = avcodec_find_decoder(codecParameters->codec_id);
    if (!pAVCodecDecoder) {
        CPrintf("Can not find codec:%d\n", codecParameters->codec_id);
        return -2;
    }

    pAVCodecCtxDecoder = avcodec_alloc_context3(pAVCodecDecoder);
    if (!pAVCodecCtxDecoder) {
        CPrintf("Failed to alloc codec context.");
        ffmpeg_release_video_decoder();
        return -3;
    }

    if (avcodec_parameters_to_context(pAVCodecCtxDecoder, codecParameters) < 0) {
        CPrintf("Failed to copy avcodec parameters to codec context.");
        ffmpeg_release_video_decoder();
        return -3;
    }


    if (avcodec_open2(pAVCodecCtxDecoder, pAVCodecDecoder, NULL) < 0){
        CPrintf("Failed to open h264 decoder");
        ffmpeg_release_video_decoder();
        return -4;
    }

    av_init_packet(&mAVPacketDecoder);

    pAVFrameDecoder = av_frame_alloc();
    pFrameYUVDecoder = av_frame_alloc();
    buf = (unsigned char *)(av_malloc((size_t)(av_image_get_buffer_size(AV_PIX_FMT_RGB24,
                                                                        curWidth,
                                                                        curHeight, 1))));
    if(av_image_fill_arrays(pFrameYUVDecoder->data,   // 需要填充的图像数据指针
                         pFrameYUVDecoder->linesize,
                         buf,
                         AV_PIX_FMT_RGB24, //图像的式
                         curWidth,
                         curHeight,
                            1) < 0) {

    }

    pImageConvertCtxDecoder = sws_getContext(curWidth, curHeight, AV_PIX_FMT_YUV420P, //input
                                             curWidth, curHeight, AV_PIX_FMT_RGB24, //output
                                             SWS_BICUBIC, NULL, NULL, NULL);
    if (!pImageConvertCtxDecoder) {
        perror("sws_getContext is failed \n");
        return -1;
    }

    return 0;
}

int ffmpeg_init_h264_decoder()
{
    AVCodec *pAVCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!pAVCodec){
        CPrintf("can not find H264 codec\n");
        return -1;
    }

    AVCodecContext *pAVCodecCtx = avcodec_alloc_context3(pAVCodec);
    if (pAVCodecCtx == NULL) {
        CPrintf("Could not alloc video context!\n");
        return -2;
    }

    AVCodecParameters *codecParameters = avcodec_parameters_alloc();
    if (avcodec_parameters_from_context(codecParameters, pAVCodecCtx) < 0) {
        CPrintf("Failed to copy avcodec parameters from codec context.");
        avcodec_parameters_free(&codecParameters);
        avcodec_free_context(&pAVCodecCtx);
        return -3;
    }

    int ret = ffmpeg_init_video_decoder(codecParameters);
    avcodec_parameters_free(&codecParameters);
    avcodec_free_context(&pAVCodecCtx);

    return ret;
}

int ffmpeg_release_video_decoder() {
    if (pAVCodecCtxDecoder != NULL) {
        avcodec_free_context(&pAVCodecCtxDecoder);
        avcodec_close(pAVCodecCtxDecoder);
        pAVCodecCtxDecoder = NULL;
    }

    if (pAVFrameDecoder != NULL) {
        av_packet_unref(&mAVPacketDecoder);
        av_free(pAVFrameDecoder);
        pAVFrameDecoder = NULL;
    }

    if (pFrameYUVDecoder) {
        av_frame_unref(pFrameYUVDecoder);
        av_free(pFrameYUVDecoder);
        av_free(buf);
        pFrameYUVDecoder = NULL;
        buf = NULL;
    }

    if (pImageConvertCtxDecoder != NULL) {
        sws_freeContext(pImageConvertCtxDecoder);
        pImageConvertCtxDecoder = NULL;
    }

    if (&mAVPacketDecoder) {
        av_packet_unref(&mAVPacketDecoder);
    }

    printf("5");
    return 0;
}

int ffmpeg_decode_h264(unsigned char *inbuf, int inbufSize, int *framePara, unsigned char **outRGBBuf, unsigned char *outYUVBuf)
{
    if (!pAVCodecCtxDecoder || !pAVFrameDecoder || !inbuf || inbufSize<=0 || !framePara || (!outRGBBuf && !outYUVBuf)) {
        return -1;
    }
    //av_frame_unref(pAVFrameDecoder);
    //av_frame_unref(pFrameYUVDecoder);

    framePara[0] = framePara[1] = 0;
    mAVPacketDecoder.data = inbuf;
    mAVPacketDecoder.size = inbufSize;

    int ret = avcodec_send_packet(pAVCodecCtxDecoder, &mAVPacketDecoder);
    if (ret == 0) {
        ret = avcodec_receive_frame(pAVCodecCtxDecoder, pAVFrameDecoder);
        if (ret == 0) {
            framePara[0] = pAVFrameDecoder->width;
            framePara[1] = pAVFrameDecoder->height;
            //printf("pAVFrameDecoder %d %d \n", pAVFrameDecoder->width, pAVFrameDecoder->height);
            if (outYUVBuf) {
                //*outYUVBuf = (unsigned char *)pAVFrameDecoder->data;
                memcpy(outYUVBuf, pAVFrameDecoder->data, sizeof(pAVFrameDecoder->data));

                framePara[2] = pAVFrameDecoder->linesize[0];
                framePara[3] = pAVFrameDecoder->linesize[1];
                framePara[4] = pAVFrameDecoder->linesize[2];
            } else if (outRGBBuf) {
                pFrameYUVDecoder->data[0] = outRGBBuf;
//                pFrameYUVDecoder->data[1] = NULL;
//                pFrameYUVDecoder->data[2] = NULL;
//                pFrameYUVDecoder->data[3] = NULL;
                //int linesize[8] = { pAVCodecCtxDecoder->width * 3, pAVCodecCtxDecoder->height * 3, 0, 0 };

//                pImageConvertCtxDecoder = sws_getContext(pAVCodecCtxDecoder->width, pAVCodecCtxDecoder->height,
//                                                            AV_PIX_FMT_YUV420P,
//                                                            pAVCodecCtxDecoder->width,
//                                                            pAVCodecCtxDecoder->height,
//                                                            AV_PIX_FMT_RGB24, SWS_BICUBIC,
//                                                            NULL, NULL, NULL);
                sws_scale(pImageConvertCtxDecoder, (const uint8_t* const *) pAVFrameDecoder->data, pAVFrameDecoder->linesize,
                          0, pAVCodecCtxDecoder->height, pFrameYUVDecoder->data, pFrameYUVDecoder->linesize);


//                if (pImageConvertCtxDecoder) {
//                    sws_freeContext(pImageConvertCtxDecoder);
//                    pImageConvertCtxDecoder = NULL;
//                }
                av_frame_unref(pAVFrameDecoder);
            }
            return 1;
        } else if (ret == AVERROR(EAGAIN)) {
            return 0;
        } else {
            return -1;
        }
    }

    return 0;
}

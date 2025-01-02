#ifndef COMMON_HPP
#define COMMON_HPP

enum VideoNameText {
    FACE_VIDEO = 0x00,
    FRONT_VIDEO,
    BACK_VIDEO,
    LEFT_VIDEO,
    RIGHT_VIDEO,
};

enum data_type {
    TERMINAL_BIND = 0x05,
    EMPLOYEE_BIND,
    QR_CODE,
    CHECK_TO_DAY,
    ELECTRONIC_FENCE,
    SPEED_LIMITED,
    LOCATION,
    ILLEGAL_OPERATIONS
};

enum error_type {
    anquandai = 13,
    anquanmao,
    dianziweilan,
    renyuanjiejin,
    weishouquanjiashi,
    wuzhengjiashi,
    pilaojiashi,
    chaosubaojing
};

typedef struct __FRAME_INFO_T
{
    unsigned int        frameSize;		//帧长
    unsigned char       *pBuffer;		//数据
    unsigned int        frameSeq;      	//帧计数
    unsigned int        framePts;       //pts
    unsigned int        curTimeSec;     //当前时间 秒
    unsigned int        curTimeUsec;    //当前时间 微秒
}FRAME_INFO_T;

typedef struct __VIDEO_FRAME_INFO_T
{
    unsigned int        frameSize;		//帧长
    unsigned char       *pBuffer;		//数据
    unsigned int        width;          //宽
    unsigned int        height;         //高
    unsigned int        frameType;      //帧类型
    unsigned int        frameSeq;      	//帧计数
    unsigned int        framePts;       //pts
    unsigned int        curTimeSec;     //当前时间 秒
    unsigned int        curTimeUsec;    //当前时间 微秒
}VIDEO_FRAME_INFO_T;


#endif // COMMON_HPP

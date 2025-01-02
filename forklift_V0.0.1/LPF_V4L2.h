#ifndef  __MAJORV4L2_H_
#define __MAJORV4L2_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

#ifdef __cplusplus
extern "C" {
#endif

#define E_NOT_GET_DATA_ERR        (22)
#define E_OK					  (0)
#define E_SELECT_ERR     		  (-13)
#define E_SELECT_TIMEOUT_ERR	  (-14)
#define E_UNKNOWN_ERR    		  (-40)

extern unsigned char *rgb240;
extern unsigned char *rgb241;
extern unsigned char *rgb242;
extern unsigned char *rgb243;
extern unsigned char *rgb244;
extern unsigned char *outBuf;
extern int outBufSize;
extern u_int32_t outBufFmt;
extern int videoIsRun0;
extern int videoIsRun1;
extern int videoIsRun2;
extern int videoIsRun3;
extern int videoIsRun4;

//int MJPEG2RGB(unsigned char* data_frame, unsigned char *rgb, int bytesused);
int convert_yuv_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height);
int h264ToRGB(unsigned char *h264Data, int dataLen, unsigned char *rgb);

int LPF_GetDeviceCount();
char *LPF_GetDeviceName(int index);
char *LPF_GetCameraName(int index);

int LPF_StartRun0(int index);
int LPF_StartRun1(int index);
int LPF_StartRun2(int index);
int LPF_StartRun3(int index);
int LPF_StartRun4(int index);
int LPF_GetFrame0();
int LPF_GetFrame1();
int LPF_GetFrame2();
int LPF_GetFrame3();
int LPF_GetFrame4();
int LPF_ReadFrame();
int LPF_Videoc_Qbuf();
int LPF_StopRun();
int LPF_DeviceRunState();

char *LPF_GetDevFmtDesc(int index);
int LPF_GetDevFpsDesc(int i, int width, int height, u_int32_t data);

int LPF_GetDevFmtWidth();
int LPF_GetDevFmtHeight();
int LPF_GetDevFmtSize();
int LPF_GetDevFmtBytesLine();

int LPF_GetResolutinCount();
int LPF_GetResolutionWidth(int index);
int LPF_GetResolutionHeight(int index);
int LPF_GetCurResWidth0();
int LPF_GetCurResHeight0();
int LPF_GetCurResWidth1();
int LPF_GetCurResHeight1();
int LPF_GetCurResWidth2();
int LPF_GetCurResHeight2();
int LPF_GetCurResWidth3();
int LPF_GetCurResHeight3();
int LPF_GetCurResWidth4();
int LPF_GetCurResHeight4();
char * LPF_GetCurPixformat();
int LPF_GetCurFps();

void LPF_SetDevControlParam(int control_id, int value);
int LPF_SetResFpsFormat(int width, int height, int fps, u_int32_t formatType);

int LPF_GetExposureMode();
int LPF_GetExposureValue();
int LPF_GetWhiteBalanceMode();
int LPF_GetWhiteBalance();
int LPF_GetBrightness();
int LPF_GetContrast();
int LPF_GetSaturation();
int LPF_GetHue();
int LPF_GetSharpness();
int LPF_GetGain();
int videoDeviceClose();
int videoDeviceOpen(int index);

#ifdef __cplusplus
}
#endif
#endif


#include "LPF_V4L2.h"
//#include <jpeglib.h>
#include <stdio.h>
//#include "videoCodec/videodecoder.h"

#ifdef __cplusplus
extern "C" {
#endif

static struct buffer0{
    void *start;
    unsigned int length;
}*buffers0;

static struct buffer1{
    void *start;
    unsigned int length;
}*buffers1;

static struct buffer2{
    void *start;
    unsigned int length;
}*buffers2;

static struct buffer3{
    void *start;
    unsigned int length;
}*buffers3;

static struct buffer4{
    void *start;
    unsigned int length;
}*buffers4;
int buffers_length;

char runningDev0[15] = "";
char runningDev1[15] = "";
char runningDev2[15] = "";
char runningDev3[15] = "";
char runningDev4[15] = "";
char devName0[15] = "";
char devName1[15] = "";
char devName2[15] = "";
char devName3[15] = "";
char devName4[15] = "";
char camName[32] = "";
char devFmtDesc[4] = "";

int fd0 = -1;
int fd1 = -1;
int fd2 = -1;
int fd3 = -1;
int fd4 = -1;
int videoIsRun0 = -1;
int videoIsRun1 = -1;
int videoIsRun2 = -1;
int videoIsRun3 = -1;
int videoIsRun4 = -1;
int deviceIsOpen0 = -1;
int deviceIsOpen1 = -1;
int deviceIsOpen2 = -1;
int deviceIsOpen3 = -1;
int deviceIsOpen4 = -1;
unsigned char *rgb240 = NULL;
unsigned char *rgb241 = NULL;
unsigned char *rgb242 = NULL;
unsigned char *rgb243 = NULL;
unsigned char *rgb244 = NULL;
unsigned char *outBuf = NULL;
int outBufSize = 0;
u_int32_t outBufFmt = 0;
static int WIDTH, HEIGHT, FPS;
static u_int32_t FORMAT_TYPE;

//V4l2相关结构体
static struct v4l2_capability cap;
static struct v4l2_fmtdesc fmtdesc;
static struct v4l2_frmsizeenum frmsizeenum;
static struct v4l2_format format;
static struct v4l2_queryctrl  queryctrl;
static struct v4l2_requestbuffers reqbuf0;
static struct v4l2_requestbuffers reqbuf1;
static struct v4l2_requestbuffers reqbuf2;
static struct v4l2_requestbuffers reqbuf3;
static struct v4l2_requestbuffers reqbuf4;
static struct v4l2_buffer buffer0;
static struct v4l2_buffer buffer1;
static struct v4l2_buffer buffer2;
static struct v4l2_buffer buffer3;
static struct v4l2_buffer buffer4;
struct v4l2_plane plane0[4];
struct v4l2_plane plane1[4];
struct v4l2_plane plane2[4];
struct v4l2_plane plane3[4];
struct v4l2_plane plane4[4];
//static struct v4l2_streamparm streamparm;

//用户控制项ID
static __u32 brightness_id = -1;    //亮度
static __u32 contrast_id = -1;  //对比度
static __u32 saturation_id = -1;    //饱和度
static __u32 hue_id = -1;   //色调
static __u32 white_balance_temperature_auto_id = -1; //白平衡色温（自动）
static __u32 white_balance_temperature_id = -1; //白平衡色温
static __u32 gamma_id = -1; //伽马
static __u32 power_line_frequency_id = -1;  //电力线频率
static __u32 sharpness_id = -1; //锐度，清晰度
static __u32 backlight_compensation_id = -1;    //背光补偿
//扩展摄像头项ID
static __u32 exposure_auto_id = -1;  //自动曝光
static __u32 exposure_absolute_id = -1;

void StartVideoPrePare();
void StartVideoStream();
void EndVideoStream();
void EndVideoStreamClear();
int test_device_exist(char *devName);

static int convert_yuv_to_rgb_pixel(int y, int u, int v)
{
    unsigned int pixel32 = 0;
    unsigned char *pixel = (unsigned char *)&pixel32;
    int r, g, b;
    r = y + (1.370705 * (v-128));
    g = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
    b = y + (1.732446 * (u-128));
    if(r > 255) r = 255;
    if(g > 255) g = 255;
    if(b > 255) b = 255;
    if(r < 0) r = 0;
    if(g < 0) g = 0;
    if(b < 0) b = 0;
    pixel[0] = r ;
    pixel[1] = g ;
    pixel[2] = b ;
    return pixel32;
}

////convert mjpeg frame to RGB24
//int MJPEG2RGB(unsigned char* data_frame, unsigned char *rgb, int bytesused)
//{
//    // variables:
//    struct jpeg_decompress_struct cinfo;
//    struct jpeg_error_mgr jerr;
//    //unsigned int width, height;
//    // data points to the mjpeg frame received from v4l2.
//    unsigned char *data = data_frame;
//    size_t data_size =  bytesused;

////    cinfo.image_width = WIDTH;
////    cinfo.image_height = HEIGHT;
////    cinfo.output_width = WIDTH;
////    cinfo.output_height = HEIGHT;

//    // all the pixels after conversion to RGB.
//    int pixel_size = 0;//size of one pixel
//    if ( data == NULL  || data_size <= 0)
//    {
//        printf("Empty data!\n");
//        return -1;
//    }

//    // ... In the initialization of the program:
//    cinfo.err = jpeg_std_error(&jerr); //错误处理设置为默认处理方式
//    jpeg_create_decompress(&cinfo);
//    jpeg_mem_src(&cinfo, data, data_size);
//     int rc = jpeg_read_header(&cinfo, TRUE);
//     if(!(1==rc))
//     {
//         //printf("Not a jpg frame.\n");
//         //return -2;
//     }
//    jpeg_start_decompress(&cinfo);
//    // 6.申请存储一行数据的内存空间
//    int row_stride = cinfo.output_width * cinfo.output_components;//
//    unsigned char *buffer = malloc(row_stride);//
//    int i = 0;//

////    width = cinfo.output_width;
////	height = cinfo.output_height;
//    pixel_size = cinfo.output_components;

//    // ... Every frame:

//    while (cinfo.output_scanline < cinfo.output_height)
//    {
////        unsigned char *temp_array[] ={ rgb + (cinfo.output_scanline) * WIDTH * pixel_size };
////        jpeg_read_scanlines(&cinfo, temp_array, 1);
//        jpeg_read_scanlines(&cinfo, &buffer, 1);//
//        memcpy(rgb + i * WIDTH * 3, buffer, row_stride);//
//        i++;//
//    }
//    free(buffer);
//    jpeg_finish_decompress(&cinfo);
//    jpeg_destroy_decompress(&cinfo);

//    return 0;
//}

int convert_yuv_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height)
{
    unsigned int in, out = 0;
    unsigned int pixel_16;
    unsigned char pixel_24[3];
    unsigned int pixel32;
    int y0, u, y1, v;

    for(in = 0; in < width * height * 2; in += 4)
    {
        pixel_16 =
                yuv[in + 3] << 24 |
                               yuv[in + 2] << 16 |
                                              yuv[in + 1] <<  8 |
                                                              yuv[in + 0];
        y0 = (pixel_16 & 0x000000ff);
        u  = (pixel_16 & 0x0000ff00) >>  8;
        y1 = (pixel_16 & 0x00ff0000) >> 16;
        v  = (pixel_16 & 0xff000000) >> 24;
        pixel32 = convert_yuv_to_rgb_pixel(y0, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
        pixel32 = convert_yuv_to_rgb_pixel(y1, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
    }
    return 0;
}
int h264ToRGB(unsigned char *h264Data, int dataLen, unsigned char *rgb)
{

    int g_ffmpegParamBuf[8] = {0};

//    ffmpeg_decode_h264(h264Data, dataLen, g_ffmpegParamBuf, rgb, NULL);

    return 0;
}

void StartVideoPrePare0()
{
    memset(&format, 0, sizeof (format));
    format.fmt.pix.field = V4L2_FIELD_ANY;
//    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    format.fmt.pix.width = WIDTH;
    format.fmt.pix.height = HEIGHT;
    format.fmt.pix.pixelformat = FORMAT_TYPE;//V4L2_PIX_FMT_MJPEG
    int ret = ioctl(fd0, VIDIOC_S_FMT, &format);
    if (ret < 0) {
        perror("change video format failed");
        return;
    }

    //申请帧缓存区
    memset (&reqbuf0, 0, sizeof (reqbuf0));
    reqbuf0.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    reqbuf0.memory = V4L2_MEMORY_MMAP;
    reqbuf0.count = 4;

    if (-1 == ioctl (fd0, VIDIOC_REQBUFS, &reqbuf0)) {
        if (errno == EINVAL)
            printf ("Video capturing or mmap-streaming is not supported\n");
        else
            perror ("VIDIOC_REQBUFS");
        return;
    }

    //分配缓存区
    buffers0 = calloc (reqbuf0.count, sizeof (*buffers0));
    if(buffers0 == NULL)
        perror("buffers is NULL");
    else
        assert (buffers0 != NULL);

    //mmap内存映射
    int i;
    for (i = 0; i < (int)reqbuf0.count; i++) {
        memset(&plane0[i],0,sizeof(struct v4l2_plane));
        memset (&buffer0, 0, sizeof (buffer0));
        buffer0.type = reqbuf0.type;
        buffer0.memory = V4L2_MEMORY_MMAP;
        buffer0.index = i;
        buffer0.m.planes = &plane0[i];
        buffer0.length = 4;

        if (-1 == ioctl (fd0, VIDIOC_QUERYBUF, &buffer0)) {
            perror ("VIDIOC_QUERYBUF");
            return;
        }

        buffers0[i].length = buffer0.m.planes[0].length;

        buffers0[i].start = mmap (NULL, buffer0.m.planes[0].length,
                                 PROT_READ | PROT_WRITE,
                                 MAP_SHARED,
                                 fd0, buffer0.m.planes[0].m.mem_offset);
//        buffers0[i].start = mmap (NULL, buffer0.m.planes->length,
//                                 PROT_READ | PROT_WRITE,
//                                 MAP_SHARED,
//                                 fd0, buffer0.m.planes->m.mem_offset);

        if (MAP_FAILED == buffers0[i].start) {
            perror ("mmap");
            return;
        }
    }

    //将缓存帧放到队列中等待视频流到来
    unsigned int ii;
    for(ii = 0; ii < reqbuf0.count; ii++){
        buffer0.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        buffer0.memory = V4L2_MEMORY_MMAP;
        buffer0.index = ii;
        if (ioctl(fd0,VIDIOC_QBUF,&buffer0)==-1){
            perror("VIDIOC_QBUF failed");
        }
    }

    WIDTH = LPF_GetCurResWidth0();
    HEIGHT = LPF_GetCurResHeight0();
    rgb240 = (unsigned char*)malloc(WIDTH*HEIGHT*3*sizeof(char));
    assert(rgb240 != NULL);
}

void StartVideoPrePare1()
{
    memset(&format, 0, sizeof (format));
    format.fmt.pix.field = V4L2_FIELD_ANY;
//    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    format.fmt.pix.width = WIDTH;
    format.fmt.pix.height = HEIGHT;
    format.fmt.pix.pixelformat = FORMAT_TYPE;//V4L2_PIX_FMT_MJPEG
    int ret = ioctl(fd1, VIDIOC_S_FMT, &format);
    if (ret < 0) {
        perror("change video format failed");
        return;
    }
//    struct v4l2_streamparm streamparm;
//    memset(&streamparm, 0, sizeof(struct v4l2_streamparm));
//    streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

//    ret = ioctl(fd, VIDIOC_G_PARM, &streamparm);/** 判断是否支持帧率设置 **/
//    if (ret < 0) {
//        return;
//    }
//    //printf("streamParm : %d %d \n", streamparm.parm.capture.timeperframe.numerator,
//           //streamparm.parm.capture.timeperframe.denominator);
//    if (V4L2_CAP_TIMEPERFRAME & streamparm.parm.capture.capability) {
//        streamparm.parm.capture.timeperframe.numerator = 1;
//        streamparm.parm.capture.timeperframe.denominator = FPS;
//        if (0 > ioctl(fd, VIDIOC_S_PARM, &streamparm)) {//设置参数
//            fprintf(stderr, "ioctl error: VIDIOC_S_PARM: %s\n", strerror(errno));
//            return;
//        }
//    }

    //申请帧缓存区
    memset (&reqbuf1, 0, sizeof (reqbuf1));
    reqbuf1.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    reqbuf1.memory = V4L2_MEMORY_MMAP;
    reqbuf1.count = 4;

    if (-1 == ioctl (fd1, VIDIOC_REQBUFS, &reqbuf1)) {
        if (errno == EINVAL)
            printf ("Video capturing or mmap-streaming is not supported\n");
        else
            perror ("VIDIOC_REQBUFS");
        return;
    }

    //分配缓存区
    buffers1 = calloc (reqbuf1.count, sizeof (*buffers1));
    if(buffers1 == NULL)
        perror("buffers is NULL");
    else
        assert (buffers1 != NULL);

    //mmap内存映射
    int i;
    for (i = 0; i < (int)reqbuf1.count; i++) {
        memset (&buffer1, 0, sizeof (buffer1));
        buffer1.type = reqbuf1.type;
        buffer1.memory = V4L2_MEMORY_MMAP;
        buffer1.index = i;

        if (-1 == ioctl (fd1, VIDIOC_QUERYBUF, &buffer1)) {
            perror ("VIDIOC_QUERYBUF");
            return;
        }

        buffers1[i].length = buffer1.length;

        buffers1[i].start = mmap (NULL, buffer1.length,
                                 PROT_READ | PROT_WRITE,
                                 MAP_SHARED,
                                 fd1, buffer1.m.offset);

        if (MAP_FAILED == buffers1[i].start) {
            perror ("mmap");
            return;
        }
    }

    //将缓存帧放到队列中等待视频流到来
    unsigned int ii;
    for(ii = 0; ii < reqbuf1.count; ii++){
        buffer1.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        buffer1.memory = V4L2_MEMORY_MMAP;
        buffer1.index = ii;
        if (ioctl(fd1,VIDIOC_QBUF,&buffer1)==-1){
            perror("VIDIOC_QBUF failed");
        }
    }

    WIDTH = LPF_GetCurResWidth1();
    HEIGHT = LPF_GetCurResHeight1();
    rgb241 = (unsigned char*)malloc(WIDTH*HEIGHT*3*sizeof(char));
    assert(rgb241 != NULL);
}

void StartVideoPrePare2()
{
    memset(&format, 0, sizeof (format));
    format.fmt.pix.field = V4L2_FIELD_ANY;
//    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    format.fmt.pix.width = WIDTH;
    format.fmt.pix.height = HEIGHT;
    format.fmt.pix.pixelformat = FORMAT_TYPE;//V4L2_PIX_FMT_MJPEG
    int ret = ioctl(fd2, VIDIOC_S_FMT, &format);
    if (ret < 0) {
        perror("change video format failed");
        return;
    }
//    struct v4l2_streamparm streamparm;
//    memset(&streamparm, 0, sizeof(struct v4l2_streamparm));
//    streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

//    ret = ioctl(fd, VIDIOC_G_PARM, &streamparm);/** 判断是否支持帧率设置 **/
//    if (ret < 0) {
//        return;
//    }
//    //printf("streamParm : %d %d \n", streamparm.parm.capture.timeperframe.numerator,
//           //streamparm.parm.capture.timeperframe.denominator);
//    if (V4L2_CAP_TIMEPERFRAME & streamparm.parm.capture.capability) {
//        streamparm.parm.capture.timeperframe.numerator = 1;
//        streamparm.parm.capture.timeperframe.denominator = FPS;
//        if (0 > ioctl(fd, VIDIOC_S_PARM, &streamparm)) {//设置参数
//            fprintf(stderr, "ioctl error: VIDIOC_S_PARM: %s\n", strerror(errno));
//            return;
//        }
//    }

    //申请帧缓存区
    memset (&reqbuf2, 0, sizeof (reqbuf2));
    reqbuf2.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    reqbuf2.memory = V4L2_MEMORY_MMAP;
    reqbuf2.count = 4;

    if (-1 == ioctl (fd2, VIDIOC_REQBUFS, &reqbuf2)) {
        if (errno == EINVAL)
            printf ("Video capturing or mmap-streaming is not supported\n");
        else
            perror ("VIDIOC_REQBUFS");
        return;
    }

    //分配缓存区
    buffers2 = calloc (reqbuf2.count, sizeof (*buffers2));
    if(buffers2 == NULL)
        perror("buffers is NULL");
    else
        assert (buffers2 != NULL);

    //mmap内存映射
    int i;
    for (i = 0; i < (int)reqbuf2.count; i++) {
        memset (&buffer2, 0, sizeof (buffer2));
        buffer2.type = reqbuf2.type;
        buffer2.memory = V4L2_MEMORY_MMAP;
        buffer2.index = i;

        if (-1 == ioctl (fd2, VIDIOC_QUERYBUF, &buffer2)) {
            perror ("VIDIOC_QUERYBUF");
            return;
        }

        buffers2[i].length = buffer2.length;

        buffers2[i].start = mmap (NULL, buffer2.length,
                                 PROT_READ | PROT_WRITE,
                                 MAP_SHARED,
                                 fd2, buffer2.m.offset);

        if (MAP_FAILED == buffers2[i].start) {
            perror ("mmap");
            return;
        }
    }

    //将缓存帧放到队列中等待视频流到来
    unsigned int ii;
    for(ii = 0; ii < reqbuf2.count; ii++){
        buffer2.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        buffer2.memory = V4L2_MEMORY_MMAP;
        buffer2.index = ii;
        if (ioctl(fd2,VIDIOC_QBUF,&buffer2)==-1){
            perror("VIDIOC_QBUF failed");
        }
    }

    WIDTH = LPF_GetCurResWidth2();
    HEIGHT = LPF_GetCurResHeight2();
    rgb242 = (unsigned char*)malloc(WIDTH*HEIGHT*3*sizeof(char));
    assert(rgb242 != NULL);
}

void StartVideoPrePare3()
{
    memset(&format, 0, sizeof (format));
    format.fmt.pix.field = V4L2_FIELD_ANY;
//    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    format.fmt.pix.width = WIDTH;
    format.fmt.pix.height = HEIGHT;
    format.fmt.pix.pixelformat = FORMAT_TYPE;//V4L2_PIX_FMT_MJPEG
    int ret = ioctl(fd3, VIDIOC_S_FMT, &format);
    if (ret < 0) {
        perror("change video format failed");
        return;
    }
//    struct v4l2_streamparm streamparm;
//    memset(&streamparm, 0, sizeof(struct v4l2_streamparm));
//    streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

//    ret = ioctl(fd, VIDIOC_G_PARM, &streamparm);/** 判断是否支持帧率设置 **/
//    if (ret < 0) {
//        return;
//    }
//    //printf("streamParm : %d %d \n", streamparm.parm.capture.timeperframe.numerator,
//           //streamparm.parm.capture.timeperframe.denominator);
//    if (V4L2_CAP_TIMEPERFRAME & streamparm.parm.capture.capability) {
//        streamparm.parm.capture.timeperframe.numerator = 1;
//        streamparm.parm.capture.timeperframe.denominator = FPS;
//        if (0 > ioctl(fd, VIDIOC_S_PARM, &streamparm)) {//设置参数
//            fprintf(stderr, "ioctl error: VIDIOC_S_PARM: %s\n", strerror(errno));
//            return;
//        }
//    }

    //申请帧缓存区
    memset (&reqbuf3, 0, sizeof (reqbuf3));
    reqbuf3.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    reqbuf3.memory = V4L2_MEMORY_MMAP;
    reqbuf3.count = 4;

    if (-1 == ioctl (fd3, VIDIOC_REQBUFS, &reqbuf3)) {
        if (errno == EINVAL)
            printf ("Video capturing or mmap-streaming is not supported\n");
        else
            perror ("VIDIOC_REQBUFS");
        return;
    }

    //分配缓存区
    buffers3 = calloc (reqbuf3.count, sizeof (*buffers3));
    if(buffers3 == NULL)
        perror("buffers is NULL");
    else
        assert (buffers3 != NULL);

    //mmap内存映射
    int i;
    for (i = 0; i < (int)reqbuf3.count; i++) {
        memset (&buffer3, 0, sizeof (buffer3));
        buffer3.type = reqbuf3.type;
        buffer3.memory = V4L2_MEMORY_MMAP;
        buffer3.index = i;

        if (-1 == ioctl (fd3, VIDIOC_QUERYBUF, &buffer3)) {
            perror ("VIDIOC_QUERYBUF");
            return;
        }

        buffers3[i].length = buffer3.length;

        buffers3[i].start = mmap (NULL, buffer3.length,
                                 PROT_READ | PROT_WRITE,
                                 MAP_SHARED,
                                 fd3, buffer3.m.offset);

        if (MAP_FAILED == buffers3[i].start) {
            perror ("mmap");
            return;
        }
    }

    //将缓存帧放到队列中等待视频流到来
    unsigned int ii;
    for(ii = 0; ii < reqbuf3.count; ii++){
        buffer3.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        buffer3.memory = V4L2_MEMORY_MMAP;
        buffer3.index = ii;
        if (ioctl(fd3,VIDIOC_QBUF,&buffer3)==-1){
            perror("VIDIOC_QBUF failed");
        }
    }

    WIDTH = LPF_GetCurResWidth3();
    HEIGHT = LPF_GetCurResHeight3();
    rgb243 = (unsigned char*)malloc(WIDTH*HEIGHT*3*sizeof(char));
    assert(rgb243 != NULL);
}

void StartVideoPrePare4()
{
    memset(&format, 0, sizeof (format));
    format.fmt.pix.field = V4L2_FIELD_ANY;
//    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    format.fmt.pix.width = WIDTH;
    format.fmt.pix.height = HEIGHT;
    format.fmt.pix.pixelformat = FORMAT_TYPE;//V4L2_PIX_FMT_MJPEG
    int ret = ioctl(fd4, VIDIOC_S_FMT, &format);
    if (ret < 0) {
        perror("change video format failed");
        return;
    }
//    struct v4l2_streamparm streamparm;
//    memset(&streamparm, 0, sizeof(struct v4l2_streamparm));
//    streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

//    ret = ioctl(fd, VIDIOC_G_PARM, &streamparm);/** 判断是否支持帧率设置 **/
//    if (ret < 0) {
//        return;
//    }
//    //printf("streamParm : %d %d \n", streamparm.parm.capture.timeperframe.numerator,
//           //streamparm.parm.capture.timeperframe.denominator);
//    if (V4L2_CAP_TIMEPERFRAME & streamparm.parm.capture.capability) {
//        streamparm.parm.capture.timeperframe.numerator = 1;
//        streamparm.parm.capture.timeperframe.denominator = FPS;
//        if (0 > ioctl(fd, VIDIOC_S_PARM, &streamparm)) {//设置参数
//            fprintf(stderr, "ioctl error: VIDIOC_S_PARM: %s\n", strerror(errno));
//            return;
//        }
//    }

    //申请帧缓存区
    memset (&reqbuf4, 0, sizeof (reqbuf4));
    reqbuf4.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    reqbuf4.memory = V4L2_MEMORY_MMAP;
    reqbuf4.count = 4;

    if (-1 == ioctl (fd4, VIDIOC_REQBUFS, &reqbuf4)) {
        if (errno == EINVAL)
            printf ("Video capturing or mmap-streaming is not supported\n");
        else
            perror ("VIDIOC_REQBUFS");
        return;
    }

    //分配缓存区
    buffers4 = calloc (reqbuf4.count, sizeof (*buffers4));
    if(buffers4 == NULL)
        perror("buffers is NULL");
    else
        assert (buffers4 != NULL);

    //mmap内存映射
    int i;
    for (i = 0; i < (int)reqbuf4.count; i++) {
        memset (&buffer4, 0, sizeof (buffer4));
        buffer4.type = reqbuf4.type;
        buffer4.memory = V4L2_MEMORY_MMAP;
        buffer4.index = i;

        if (-1 == ioctl (fd4, VIDIOC_QUERYBUF, &buffer4)) {
            perror ("VIDIOC_QUERYBUF");
            return;
        }

        buffers4[i].length = buffer4.length;

        buffers4[i].start = mmap (NULL, buffer4.length,
                                 PROT_READ | PROT_WRITE,
                                 MAP_SHARED,
                                 fd4, buffer4.m.offset);

        if (MAP_FAILED == buffers4[i].start) {
            perror ("mmap");
            return;
        }
    }

    //将缓存帧放到队列中等待视频流到来
    unsigned int ii;
    for(ii = 0; ii < reqbuf4.count; ii++){
        buffer4.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        buffer4.memory = V4L2_MEMORY_MMAP;
        buffer4.index = ii;
        if (ioctl(fd4,VIDIOC_QBUF,&buffer4)==-1){
            perror("VIDIOC_QBUF failed");
        }
    }

    WIDTH = LPF_GetCurResWidth4();
    HEIGHT = LPF_GetCurResHeight4();
    rgb244 = (unsigned char*)malloc(WIDTH*HEIGHT*3*sizeof(char));
    assert(rgb244 != NULL);
}

void StartVideoStream0()
{
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    if (ioctl(fd0,VIDIOC_STREAMON,&type) == -1) {
        perror("VIDIOC_STREAMON failed");
    }
}

void StartVideoStream1()
{
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    if (ioctl(fd1,VIDIOC_STREAMON,&type) == -1) {
        perror("VIDIOC_STREAMON failed");
    }
}

void StartVideoStream2()
{
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    if (ioctl(fd2,VIDIOC_STREAMON,&type) == -1) {
        perror("VIDIOC_STREAMON failed");
    }
}

void StartVideoStream3()
{
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    if (ioctl(fd3,VIDIOC_STREAMON,&type) == -1) {
        perror("VIDIOC_STREAMON failed");
    }
}

void StartVideoStream4()
{
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    if (ioctl(fd4,VIDIOC_STREAMON,&type) == -1) {
        perror("VIDIOC_STREAMON failed");
    }
}

void EndVideoStream0()
{
    //关闭视频流
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    if (ioctl(fd0,VIDIOC_STREAMOFF,&type) == -1) {
        perror("VIDIOC_STREAMOFF failed");
    }
}

void EndVideoStream1()
{
    //关闭视频流
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    if (ioctl(fd1,VIDIOC_STREAMOFF,&type) == -1) {
        perror("VIDIOC_STREAMOFF failed");
    }
}

void EndVideoStream2()
{
    //关闭视频流
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    if (ioctl(fd2,VIDIOC_STREAMOFF,&type) == -1) {
        perror("VIDIOC_STREAMOFF failed");
    }
}

void EndVideoStream3()
{
    //关闭视频流
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    if (ioctl(fd3,VIDIOC_STREAMOFF,&type) == -1) {
        perror("VIDIOC_STREAMOFF failed");
    }
}

void EndVideoStream4()
{
    //关闭视频流
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    if (ioctl(fd4,VIDIOC_STREAMOFF,&type) == -1) {
        perror("VIDIOC_STREAMOFF failed");
    }
}

void EndVideoStreamClear0()
{
    //手动释放分配的内存
    int i;
    for (i = 0; i < (int)reqbuf0.count; i++)
        munmap (buffers0[i].start, buffers0[i].length);
    free(rgb240);
    rgb240 = NULL;
}
void EndVideoStreamClear1()
{
    //手动释放分配的内存
    int i;
    for (i = 0; i < (int)reqbuf1.count; i++)
        munmap (buffers1[i].start, buffers1[i].length);
    free(rgb241);
    rgb241 = NULL;
}
void EndVideoStreamClear2()
{
    //手动释放分配的内存
    int i;
    for (i = 0; i < (int)reqbuf2.count; i++)
        munmap (buffers2[i].start, buffers2[i].length);
    free(rgb242);
    rgb242 = NULL;
}
void EndVideoStreamClear3()
{
    //手动释放分配的内存
    int i;
    for (i = 0; i < (int)reqbuf3.count; i++)
        munmap (buffers3[i].start, buffers3[i].length);
    free(rgb243);
    rgb243 = NULL;
}
void EndVideoStreamClear4()
{
    //手动释放分配的内存
    int i;
    for (i = 0; i < (int)reqbuf4.count; i++)
        munmap (buffers4[i].start, buffers4[i].length);
    free(rgb244);
    rgb244 = NULL;
}

void LPF_GetDevControlAll0()
{
    int i = 0;
    for(i = V4L2_CID_BASE; i <= V4L2_CID_LASTP1; i++)
    {
        queryctrl.id = i;
        if(0 == ioctl(fd0, VIDIOC_QUERYCTRL, &queryctrl))
        {
            if(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
                continue;

            if(queryctrl.id == V4L2_CID_BRIGHTNESS)
                brightness_id = i;
            if(queryctrl.id == V4L2_CID_CONTRAST)
                contrast_id = i;
            if(queryctrl.id == V4L2_CID_SATURATION)
                saturation_id = i;
            if(queryctrl.id == V4L2_CID_HUE)
                hue_id = i;
            if(queryctrl.id == V4L2_CID_AUTO_WHITE_BALANCE)
                white_balance_temperature_auto_id = i;
            if(queryctrl.id == V4L2_CID_WHITE_BALANCE_TEMPERATURE)
                white_balance_temperature_id = i;
            if(queryctrl.id == V4L2_CID_GAMMA)
                gamma_id = i;
            if(queryctrl.id == V4L2_CID_POWER_LINE_FREQUENCY)
                power_line_frequency_id = i;
            if(queryctrl.id == V4L2_CID_SHARPNESS)
                sharpness_id = i;
            if(queryctrl.id == V4L2_CID_BACKLIGHT_COMPENSATION)
                backlight_compensation_id = i;
        }
        else
        {
            if(errno == EINVAL)
                continue;
            perror("VIDIOC_QUERYCTRL");
            return;
        }
    }

    queryctrl.id = V4L2_CTRL_CLASS_CAMERA | V4L2_CTRL_FLAG_NEXT_CTRL;
    while (0 == ioctl (fd0, VIDIOC_QUERYCTRL, &queryctrl)) {
        if (V4L2_CTRL_ID2CLASS (queryctrl.id) != V4L2_CTRL_CLASS_CAMERA)
            break;

        if(queryctrl.id == V4L2_CID_EXPOSURE_AUTO)
            exposure_auto_id = queryctrl.id;
        if(queryctrl.id == V4L2_CID_EXPOSURE_ABSOLUTE)
            exposure_absolute_id = queryctrl.id;

        queryctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
    }
}

void LPF_GetDevControlAll1()
{
    int i = 0;
    for(i = V4L2_CID_BASE; i <= V4L2_CID_LASTP1; i++)
    {
        queryctrl.id = i;
        if(0 == ioctl(fd1, VIDIOC_QUERYCTRL, &queryctrl))
        {
            if(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
                continue;

            if(queryctrl.id == V4L2_CID_BRIGHTNESS)
                brightness_id = i;
            if(queryctrl.id == V4L2_CID_CONTRAST)
                contrast_id = i;
            if(queryctrl.id == V4L2_CID_SATURATION)
                saturation_id = i;
            if(queryctrl.id == V4L2_CID_HUE)
                hue_id = i;
            if(queryctrl.id == V4L2_CID_AUTO_WHITE_BALANCE)
                white_balance_temperature_auto_id = i;
            if(queryctrl.id == V4L2_CID_WHITE_BALANCE_TEMPERATURE)
                white_balance_temperature_id = i;
            if(queryctrl.id == V4L2_CID_GAMMA)
                gamma_id = i;
            if(queryctrl.id == V4L2_CID_POWER_LINE_FREQUENCY)
                power_line_frequency_id = i;
            if(queryctrl.id == V4L2_CID_SHARPNESS)
                sharpness_id = i;
            if(queryctrl.id == V4L2_CID_BACKLIGHT_COMPENSATION)
                backlight_compensation_id = i;
        }
        else
        {
            if(errno == EINVAL)
                continue;
            perror("VIDIOC_QUERYCTRL");
            return;
        }
    }

    queryctrl.id = V4L2_CTRL_CLASS_CAMERA | V4L2_CTRL_FLAG_NEXT_CTRL;
    while (0 == ioctl (fd1, VIDIOC_QUERYCTRL, &queryctrl)) {
        if (V4L2_CTRL_ID2CLASS (queryctrl.id) != V4L2_CTRL_CLASS_CAMERA)
            break;

        if(queryctrl.id == V4L2_CID_EXPOSURE_AUTO)
            exposure_auto_id = queryctrl.id;
        if(queryctrl.id == V4L2_CID_EXPOSURE_ABSOLUTE)
            exposure_absolute_id = queryctrl.id;

        queryctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
    }
}

void LPF_GetDevControlAll2()
{
    int i = 0;
    for(i = V4L2_CID_BASE; i <= V4L2_CID_LASTP1; i++)
    {
        queryctrl.id = i;
        if(0 == ioctl(fd2, VIDIOC_QUERYCTRL, &queryctrl))
        {
            if(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
                continue;

            if(queryctrl.id == V4L2_CID_BRIGHTNESS)
                brightness_id = i;
            if(queryctrl.id == V4L2_CID_CONTRAST)
                contrast_id = i;
            if(queryctrl.id == V4L2_CID_SATURATION)
                saturation_id = i;
            if(queryctrl.id == V4L2_CID_HUE)
                hue_id = i;
            if(queryctrl.id == V4L2_CID_AUTO_WHITE_BALANCE)
                white_balance_temperature_auto_id = i;
            if(queryctrl.id == V4L2_CID_WHITE_BALANCE_TEMPERATURE)
                white_balance_temperature_id = i;
            if(queryctrl.id == V4L2_CID_GAMMA)
                gamma_id = i;
            if(queryctrl.id == V4L2_CID_POWER_LINE_FREQUENCY)
                power_line_frequency_id = i;
            if(queryctrl.id == V4L2_CID_SHARPNESS)
                sharpness_id = i;
            if(queryctrl.id == V4L2_CID_BACKLIGHT_COMPENSATION)
                backlight_compensation_id = i;
        }
        else
        {
            if(errno == EINVAL)
                continue;
            perror("VIDIOC_QUERYCTRL");
            return;
        }
    }

    queryctrl.id = V4L2_CTRL_CLASS_CAMERA | V4L2_CTRL_FLAG_NEXT_CTRL;
    while (0 == ioctl (fd2, VIDIOC_QUERYCTRL, &queryctrl)) {
        if (V4L2_CTRL_ID2CLASS (queryctrl.id) != V4L2_CTRL_CLASS_CAMERA)
            break;

        if(queryctrl.id == V4L2_CID_EXPOSURE_AUTO)
            exposure_auto_id = queryctrl.id;
        if(queryctrl.id == V4L2_CID_EXPOSURE_ABSOLUTE)
            exposure_absolute_id = queryctrl.id;

        queryctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
    }
}

void LPF_GetDevControlAll3()
{
    int i = 0;
    for(i = V4L2_CID_BASE; i <= V4L2_CID_LASTP1; i++)
    {
        queryctrl.id = i;
        if(0 == ioctl(fd3, VIDIOC_QUERYCTRL, &queryctrl))
        {
            if(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
                continue;

            if(queryctrl.id == V4L2_CID_BRIGHTNESS)
                brightness_id = i;
            if(queryctrl.id == V4L2_CID_CONTRAST)
                contrast_id = i;
            if(queryctrl.id == V4L2_CID_SATURATION)
                saturation_id = i;
            if(queryctrl.id == V4L2_CID_HUE)
                hue_id = i;
            if(queryctrl.id == V4L2_CID_AUTO_WHITE_BALANCE)
                white_balance_temperature_auto_id = i;
            if(queryctrl.id == V4L2_CID_WHITE_BALANCE_TEMPERATURE)
                white_balance_temperature_id = i;
            if(queryctrl.id == V4L2_CID_GAMMA)
                gamma_id = i;
            if(queryctrl.id == V4L2_CID_POWER_LINE_FREQUENCY)
                power_line_frequency_id = i;
            if(queryctrl.id == V4L2_CID_SHARPNESS)
                sharpness_id = i;
            if(queryctrl.id == V4L2_CID_BACKLIGHT_COMPENSATION)
                backlight_compensation_id = i;
        }
        else
        {
            if(errno == EINVAL)
                continue;
            perror("VIDIOC_QUERYCTRL");
            return;
        }
    }

    queryctrl.id = V4L2_CTRL_CLASS_CAMERA | V4L2_CTRL_FLAG_NEXT_CTRL;
    while (0 == ioctl (fd3, VIDIOC_QUERYCTRL, &queryctrl)) {
        if (V4L2_CTRL_ID2CLASS (queryctrl.id) != V4L2_CTRL_CLASS_CAMERA)
            break;

        if(queryctrl.id == V4L2_CID_EXPOSURE_AUTO)
            exposure_auto_id = queryctrl.id;
        if(queryctrl.id == V4L2_CID_EXPOSURE_ABSOLUTE)
            exposure_absolute_id = queryctrl.id;

        queryctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
    }
}

void LPF_GetDevControlAll4()
{
    int i = 0;
    for(i = V4L2_CID_BASE; i <= V4L2_CID_LASTP1; i++)
    {
        queryctrl.id = i;
        if(0 == ioctl(fd4, VIDIOC_QUERYCTRL, &queryctrl))
        {
            if(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
                continue;

            if(queryctrl.id == V4L2_CID_BRIGHTNESS)
                brightness_id = i;
            if(queryctrl.id == V4L2_CID_CONTRAST)
                contrast_id = i;
            if(queryctrl.id == V4L2_CID_SATURATION)
                saturation_id = i;
            if(queryctrl.id == V4L2_CID_HUE)
                hue_id = i;
            if(queryctrl.id == V4L2_CID_AUTO_WHITE_BALANCE)
                white_balance_temperature_auto_id = i;
            if(queryctrl.id == V4L2_CID_WHITE_BALANCE_TEMPERATURE)
                white_balance_temperature_id = i;
            if(queryctrl.id == V4L2_CID_GAMMA)
                gamma_id = i;
            if(queryctrl.id == V4L2_CID_POWER_LINE_FREQUENCY)
                power_line_frequency_id = i;
            if(queryctrl.id == V4L2_CID_SHARPNESS)
                sharpness_id = i;
            if(queryctrl.id == V4L2_CID_BACKLIGHT_COMPENSATION)
                backlight_compensation_id = i;
        }
        else
        {
            if(errno == EINVAL)
                continue;
            perror("VIDIOC_QUERYCTRL");
            return;
        }
    }

    queryctrl.id = V4L2_CTRL_CLASS_CAMERA | V4L2_CTRL_FLAG_NEXT_CTRL;
    while (0 == ioctl (fd4, VIDIOC_QUERYCTRL, &queryctrl)) {
        if (V4L2_CTRL_ID2CLASS (queryctrl.id) != V4L2_CTRL_CLASS_CAMERA)
            break;

        if(queryctrl.id == V4L2_CID_EXPOSURE_AUTO)
            exposure_auto_id = queryctrl.id;
        if(queryctrl.id == V4L2_CID_EXPOSURE_ABSOLUTE)
            exposure_absolute_id = queryctrl.id;

        queryctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
    }
}

void LPF_SetDevControlParam(int control_id, int value)
{
    if(videoIsRun0 < 0)
    {
        return;
    }

    struct v4l2_control control_s;
    control_s.id = control_id;
    control_s.value = value;

    if(ioctl(fd0,VIDIOC_S_CTRL,&control_s)==-1)
    {
        perror("ioctl LPF_SetDevControlParam");
        return;
        //exit(EXIT_FAILURE);
    }
}

int LPF_SetResFpsFormat(int width, int height, int fps, u_int32_t formatType)
{
    WIDTH = width;
    HEIGHT = height;
    FPS = fps;
    FORMAT_TYPE = formatType;

    return 0;
}

int test_device_exist(char *devName)
{
    struct stat st;
    if (-1 == stat(devName, &st))
        return -1;

    if (!S_ISCHR (st.st_mode))
        return -1;

    return 0;
}
int LPF_GetDeviceCount()
{
    char devname[15] = "";
    int count = 0;
    int i;
    for(i = 0; i < 100; i++)
    {
        sprintf(devname, "%s%d", "/dev/video", i);
        if(test_device_exist(devname) == 0)
            count++;

        memset(devname, 0, sizeof(devname));
    }

    return count;
}

//根据索引获取设备名称
char *LPF_GetDeviceName(int index)
{
    memset(devName0, 0, sizeof(devName0));

    int count = 0;
    char devname[15] = "";
    int i;
    for(i = 0; i < 100; i++)
    {
        sprintf(devname, "%s%d", "/dev/video", i);
        if(test_device_exist(devname) == 0)
        {
            if(count == index)
                break;
            count++;
        }
        else
            memset(devname, 0, sizeof(devname));
    }

    strcpy(devName0, devname);

    return devName0;
}

//根据索引获取摄像头名称
char *LPF_GetCameraName(int index)
{
    if(videoIsRun0 > 0)
        return "";

    memset(camName, 0, sizeof(camName));

    char devname[15] = "";
    strcpy(devname, LPF_GetDeviceName(index));

    int fd0 = open(devname, O_RDWR | O_NONBLOCK, 0);
    if (fd0 < 0) {
        close(fd0);
        return NULL;
    }
    if(ioctl(fd0, VIDIOC_QUERYCAP, &cap) != -1)
    {
        strcpy(camName, (char *)cap.card);
    }
    close(fd0);

    return camName;
}



//运行指定索引的视频
int LPF_StartRun0(int index)
{
    if(videoIsRun0 > 0 || index < 0)
        return -1;

    char *devname0 = LPF_GetDeviceName(index);

    if((fd0 = open(devname0, O_RDWR | O_NONBLOCK, 0)) < 0) {
        perror("open uvc dev err\n");
        close(fd0);
        return -1;
    }

    deviceIsOpen0 = 1;

//    setCurDecode_width_height(WIDTH, HEIGHT);
//    ffmpeg_init_h264_decoder();//init avcodec

//    LPF_GetDevControlAll0();

    StartVideoPrePare0();
    StartVideoStream0();

    strcpy(runningDev0, devname0);

    videoIsRun0 = 1;

    return 0;
}

//运行指定索引的视频
int LPF_StartRun1(int index)
{
    if(videoIsRun1 > 0 || index < 0)
        return -1;

    char *devname1 = LPF_GetDeviceName(index);

    if((fd1 = open(devname1, O_RDWR | O_NONBLOCK, 0)) < 0) {
        perror("open uvc dev err\n");
        close(fd1);
        return -1;
    }

    deviceIsOpen1 = 1;

//    setCurDecode_width_height(WIDTH, HEIGHT);
//    ffmpeg_init_h264_decoder();//init avcodec

//    LPF_GetDevControlAll1();

    StartVideoPrePare1();
    StartVideoStream1();

    strcpy(runningDev1, devname1);

    videoIsRun1 = 1;

    return 0;
}

//运行指定索引的视频
int LPF_StartRun2(int index)
{
    if(videoIsRun2 > 0 || index < 0)
        return -1;

    char *devname2 = LPF_GetDeviceName(index);

    if((fd2 = open(devname2, O_RDWR | O_NONBLOCK, 0)) < 0) {
        perror("open uvc dev err\n");
        close(fd2);
        return -1;
    }

    deviceIsOpen2 = 1;

//    setCurDecode_width_height(WIDTH, HEIGHT);
//    ffmpeg_init_h264_decoder();//init avcodec

//    LPF_GetDevControlAll2();

    StartVideoPrePare2();
    StartVideoStream2();

    strcpy(runningDev2, devname2);

    videoIsRun2 = 1;

    return 0;
}

//运行指定索引的视频
int LPF_StartRun3(int index)
{
    if(videoIsRun3 > 0 || index < 0)
        return -1;

    char *devname3 = LPF_GetDeviceName(index);

    if((fd3 = open(devname3, O_RDWR | O_NONBLOCK, 0)) < 0) {
        perror("open uvc dev err\n");
        close(fd3);
        return -1;
    }

    deviceIsOpen3 = 1;

//    setCurDecode_width_height(WIDTH, HEIGHT);
//    ffmpeg_init_h264_decoder();//init avcodec

//    LPF_GetDevControlAll3();

    StartVideoPrePare3();
    StartVideoStream3();

    strcpy(runningDev3, devname3);

    videoIsRun3 = 1;

    return 0;
}

//运行指定索引的视频
int LPF_StartRun4(int index)
{
    if(videoIsRun4 > 0 || index < 0)
        return -1;

    char *devname4 = LPF_GetDeviceName(index);

    if((fd4 = open(devname4, O_RDWR | O_NONBLOCK, 0)) < 0) {
        perror("open uvc dev err\n");
        close(fd4);
        return -1;
    }

    deviceIsOpen4 = 1;

//    setCurDecode_width_height(WIDTH, HEIGHT);
//    ffmpeg_init_h264_decoder();//init avcodec

//    LPF_GetDevControlAll4();

    StartVideoPrePare4();
    StartVideoStream4();

    strcpy(runningDev4, devname4);

    videoIsRun4 = 1;

    return 0;
}

int LPF_GetFrame0()
{
    if(videoIsRun0> 0)
    {
        fd_set fds;
        struct timeval tv;
        int r;

        FD_ZERO (&fds);
        FD_SET (fd0, &fds);

        /* Timeout. */
        tv.tv_sec = 0;
        tv.tv_usec = 30000;

        r = select (fd0 + 1, &fds, NULL, NULL, &tv);

        if (0 == r)
            return -1;
        else if(-1 == r)
            return errno;

//        memset(plane0, 0, sizeof(struct v4l2_plane)*4);
        memset(&buffer0, 0, sizeof(buffer0));
        buffer0.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        buffer0.memory = V4L2_MEMORY_MMAP;
        buffer0.m.planes = &plane0[0];
        buffer0.length   = 1;

        if (ioctl(fd0, VIDIOC_DQBUF, &buffer0) == -1) {
            perror("GetFrame VIDIOC_DQBUF0 Failed");
            return errno;
        }
        else
        {
            if (FORMAT_TYPE == V4L2_PIX_FMT_YUYV) {
                convert_yuv_to_rgb_buffer((unsigned char*)buffers0[buffer0.index].start, rgb240, WIDTH, HEIGHT);
            } else if (FORMAT_TYPE == V4L2_PIX_FMT_MJPEG) {
//                MJPEG2RGB((unsigned char*)buffers0[buffer0.index].start, rgb240, buffer0.bytesused);
            } else if (FORMAT_TYPE == V4L2_PIX_FMT_H264) {
                h264ToRGB((unsigned char*)buffers0[buffer0.index].start, buffer0.bytesused, rgb240);
            }

            if (ioctl(fd0, VIDIOC_QBUF, &buffer0) < 0) {
                perror("GetFrame VIDIOC_QBUF Failed");
                return errno;
            }

            return 0;
        }
    }

    return 0;
}

int LPF_GetFrame1()
{
    if(videoIsRun1> 0)
    {
        fd_set fds;
        struct timeval tv;
        int r;

        FD_ZERO (&fds);
        FD_SET (fd1, &fds);

        /* Timeout. */
        tv.tv_sec = 0;
        tv.tv_usec = 30000;

        r = select (fd1 + 1, &fds, NULL, NULL, &tv);

        if (0 == r)
            return -1;
        else if(-1 == r)
            return errno;

        memset(&buffer1, 0, sizeof(buffer1));
        buffer1.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        buffer1.memory = V4L2_MEMORY_MMAP;
        buffer1.m.planes = &plane1[0];
        buffer1.length   = 1;
        if (ioctl(fd1, VIDIOC_DQBUF, &buffer1) == -1) {
            perror("GetFrame VIDIOC_DQBUF Failed");
            return errno;
        }
        else
        {
            if (FORMAT_TYPE == V4L2_PIX_FMT_YUYV) {
                convert_yuv_to_rgb_buffer((unsigned char*)buffers1[buffer1.index].start, rgb241, WIDTH, HEIGHT);
            } else if (FORMAT_TYPE == V4L2_PIX_FMT_MJPEG) {
//                MJPEG2RGB((unsigned char*)buffers1[buffer1.index].start, rgb241, buffer1.bytesused);
            } else if (FORMAT_TYPE == V4L2_PIX_FMT_H264) {
                h264ToRGB((unsigned char*)buffers1[buffer1.index].start, buffer1.bytesused, rgb241);
            }

            if (ioctl(fd1, VIDIOC_QBUF, &buffer1) < 0) {
                perror("GetFrame VIDIOC_QBUF Failed");
                return errno;
            }

            return 0;
        }
    }

    return 0;
}
int LPF_GetFrame2()
{
    if(videoIsRun2> 0)
    {
        fd_set fds;
        struct timeval tv;
        int r;

        FD_ZERO (&fds);
        FD_SET (fd2, &fds);

        /* Timeout. */
        tv.tv_sec = 0;
        tv.tv_usec = 30000;

        r = select (fd2 + 1, &fds, NULL, NULL, &tv);

        if (0 == r)
            return -1;
        else if(-1 == r)
            return errno;

        memset(&buffer2, 0, sizeof(buffer2));
        buffer2.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        buffer2.memory = V4L2_MEMORY_MMAP;
        buffer2.m.planes = &plane2[0];
        buffer2.length   = 1;
        if (ioctl(fd2, VIDIOC_DQBUF, &buffer2) == -1) {
            perror("GetFrame VIDIOC_DQBUF Failed");
            return errno;
        }
        else
        {
            if (FORMAT_TYPE == V4L2_PIX_FMT_YUYV) {
                convert_yuv_to_rgb_buffer((unsigned char*)buffers2[buffer2.index].start, rgb242, WIDTH, HEIGHT);
            } else if (FORMAT_TYPE == V4L2_PIX_FMT_MJPEG) {
//                MJPEG2RGB((unsigned char*)buffers2[buffer2.index].start, rgb242, buffer2.bytesused);
            } else if (FORMAT_TYPE == V4L2_PIX_FMT_H264) {
                h264ToRGB((unsigned char*)buffers2[buffer2.index].start, buffer2.bytesused, rgb242);
            }

            if (ioctl(fd2, VIDIOC_QBUF, &buffer2) < 0) {
                perror("GetFrame VIDIOC_QBUF Failed");
                return errno;
            }

            return 0;
        }
    }

    return 0;
}
int LPF_GetFrame3()
{
    if(videoIsRun3> 0)
    {
        fd_set fds;
        struct timeval tv;
        int r;

        FD_ZERO (&fds);
        FD_SET (fd3, &fds);

        /* Timeout. */
        tv.tv_sec = 0;
        tv.tv_usec = 30000;

        r = select (fd3 + 1, &fds, NULL, NULL, &tv);

        if (0 == r)
            return -1;
        else if(-1 == r)
            return errno;

        memset(&buffer3, 0, sizeof(buffer3));
        buffer3.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        buffer3.memory = V4L2_MEMORY_MMAP;
        buffer3.m.planes = &plane3[0];
        buffer3.length   = 1;
        if (ioctl(fd3, VIDIOC_DQBUF, &buffer3) == -1) {
            perror("GetFrame VIDIOC_DQBUF Failed");
            return errno;
        }
        else
        {
            if (FORMAT_TYPE == V4L2_PIX_FMT_YUYV) {
                convert_yuv_to_rgb_buffer((unsigned char*)buffers3[buffer3.index].start, rgb243, WIDTH, HEIGHT);
            } else if (FORMAT_TYPE == V4L2_PIX_FMT_MJPEG) {
//                MJPEG2RGB((unsigned char*)buffers3[buffer3.index].start, rgb243, buffer3.bytesused);
            } else if (FORMAT_TYPE == V4L2_PIX_FMT_H264) {
                h264ToRGB((unsigned char*)buffers3[buffer3.index].start, buffer3.bytesused, rgb243);
            }

            if (ioctl(fd3, VIDIOC_QBUF, &buffer3) < 0) {
                perror("GetFrame VIDIOC_QBUF Failed");
                return errno;
            }

            return 0;
        }
    }

    return 0;
}
int LPF_GetFrame4()
{
    if(videoIsRun4> 0)
    {
        fd_set fds;
        struct timeval tv;
        int r;

        FD_ZERO (&fds);
        FD_SET (fd4, &fds);

        /* Timeout. */
        tv.tv_sec = 0;
        tv.tv_usec = 30000;

        r = select (fd4 + 1, &fds, NULL, NULL, &tv);

        if (0 == r)
            return -1;
        else if(-1 == r)
            return errno;

        memset(&buffer4, 0, sizeof(buffer4));
        buffer4.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        buffer4.memory = V4L2_MEMORY_MMAP;
        buffer4.m.planes = &plane4[0];
        buffer4.length   = 1;
        if (ioctl(fd4, VIDIOC_DQBUF, &buffer4) == -1) {
            perror("GetFrame VIDIOC_DQBUF Failed");
            return errno;
        }
        else
        {
            if (FORMAT_TYPE == V4L2_PIX_FMT_YUYV) {
                convert_yuv_to_rgb_buffer((unsigned char*)buffers4[buffer4.index].start, rgb244, WIDTH, HEIGHT);
            } else if (FORMAT_TYPE == V4L2_PIX_FMT_MJPEG) {
//                MJPEG2RGB((unsigned char*)buffers4[buffer4.index].start, rgb244, buffer4.bytesused);
            } else if (FORMAT_TYPE == V4L2_PIX_FMT_H264) {
                h264ToRGB((unsigned char*)buffers4[buffer4.index].start, buffer4.bytesused, rgb244);
            }

            if (ioctl(fd4, VIDIOC_QBUF, &buffer4) < 0) {
                perror("GetFrame VIDIOC_QBUF Failed");
                return errno;
            }

            return 0;
        }
    }

    return 0;
}

int LPF_StopRun0()
{
    if(videoIsRun0 > 0)
    {
        EndVideoStream0();
        EndVideoStreamClear0();
    }

    memset(runningDev0, 0, sizeof(runningDev0));
    videoIsRun0 = -1;
    deviceIsOpen0 = -1;

    if(close(fd0) != 0)
        return -1;

//    ffmpeg_release_video_decoder();

    return 0;
}


int LPF_StopRun1()
{
    if(videoIsRun1 > 0)
    {
        EndVideoStream1();
        EndVideoStreamClear1();
    }

    memset(runningDev1, 0, sizeof(runningDev1));
    videoIsRun1 = -1;
    deviceIsOpen1 = -1;

    if(close(fd1) != 0)
        return -1;

//    ffmpeg_release_video_decoder();

    return 0;
}


int LPF_StopRun2()
{
    if(videoIsRun2 > 0)
    {
        EndVideoStream2();
        EndVideoStreamClear2();
    }

    memset(runningDev2, 0, sizeof(runningDev2));
    videoIsRun2 = -1;
    deviceIsOpen2 = -1;

    if(close(fd2) != 0)
        return -1;

//    ffmpeg_release_video_decoder();

    return 0;
}

int LPF_StopRun3()
{
    if(videoIsRun3 > 0)
    {
        EndVideoStream3();
        EndVideoStreamClear3();
    }

    memset(runningDev3, 0, sizeof(runningDev3));
    videoIsRun3 = -1;
    deviceIsOpen3 = -1;

    if(close(fd3) != 0)
        return -1;

//    ffmpeg_release_video_decoder();

    return 0;
}

int LPF_StopRun4()
{
    if(videoIsRun4 > 0)
    {
        EndVideoStream4();
        EndVideoStreamClear4();
    }

    memset(runningDev4, 0, sizeof(runningDev4));
    videoIsRun4 = -1;
    deviceIsOpen4 = -1;

    if(close(fd4) != 0)
        return -1;

//    ffmpeg_release_video_decoder();

    return 0;
}




int LPF_DeviceRunState()
{
    return videoIsRun0;
}

char *LPF_GetDevFmtDesc(int index)
{
    memset(devFmtDesc, 0, sizeof(devFmtDesc));

    fmtdesc.index=index;
    fmtdesc.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if(ioctl(fd0, VIDIOC_ENUM_FMT, &fmtdesc) != -1)
    {
        char fmt[5] = "";
        sprintf(fmt, "%c%c%c%c",
                (__u8)(fmtdesc.pixelformat&0XFF),
                (__u8)((fmtdesc.pixelformat>>8)&0XFF),
                (__u8)((fmtdesc.pixelformat>>16)&0XFF),
                (__u8)((fmtdesc.pixelformat>>24)&0XFF));

        strncpy(devFmtDesc, fmt, 4);
    }

    return devFmtDesc;
}

int LPF_GetDevFpsDesc(int i, int width, int height, u_int32_t data)
{
    struct v4l2_frmivalenum frmival;//frmival.index = 0;
    frmival.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//    if (data != V4L2_PIX_FMT_MJPEG) {
//        return -2;
//    }
    frmival.pixel_format = data;//V4L2_PIX_FMT_MJPEG
    frmival.width = width;
    frmival.height = height;
    frmival.index = i;
    while (0 == ioctl(fd0, VIDIOC_ENUM_FRAMEINTERVALS, &frmival)) {
        //printf("Frame interval<%ffps> ", frmival.discrete.denominator / frmival.discrete.numerator);
        return frmival.discrete.denominator / frmival.discrete.numerator;
    }
    return -1;
}

//获取图像的格式属性相关
int LPF_GetDevFmtWidth()
{
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl (fd0, VIDIOC_G_FMT, &format) == -1)
    {
        perror("GetDevFmtWidth:");
        return -1;
    }
    return format.fmt.pix.width;
}

int LPF_GetDevFmtHeight()
{
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl (fd0, VIDIOC_G_FMT, &format) == -1)
    {
        perror("GetDevFmtHeight:");
        return -1;
    }
    return format.fmt.pix.height;
}
int LPF_GetDevFmtSize()
{
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl (fd0, VIDIOC_G_FMT, &format) == -1)
    {
        perror("GetDevFmtSize:");
        return -1;
    }
    return format.fmt.pix.sizeimage;
}
int LPF_GetDevFmtBytesLine()
{
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl (fd0, VIDIOC_G_FMT, &format) == -1)
    {
        perror("GetDevFmtBytesLine:");
        return -1;
    }
    return format.fmt.pix.bytesperline;
}

//设备分辨率相关
int LPF_GetResolutinCount()
{
    fmtdesc.index = 0;
    fmtdesc.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if(ioctl(fd0, VIDIOC_ENUM_FMT, &fmtdesc) == -1)
        return -1;

    frmsizeenum.pixel_format = fmtdesc.pixelformat;
    int i = 0;
    for(i = 0; ; i++)
    {
        frmsizeenum.index = i;
        if(ioctl(fd0, VIDIOC_ENUM_FRAMESIZES, &frmsizeenum) == -1)
            break;
    }
    return i;
}
int LPF_GetResolutionWidth(int index)
{
    fmtdesc.index = 0;
    fmtdesc.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if(ioctl(fd0, VIDIOC_ENUM_FMT, &fmtdesc) == -1)
        return -1;

    frmsizeenum.pixel_format = fmtdesc.pixelformat;

    frmsizeenum.index = index;
    if(ioctl(fd0, VIDIOC_ENUM_FRAMESIZES, &frmsizeenum) != -1)
        return frmsizeenum.discrete.width;
    else
        return -1;
}

int LPF_GetResolutionHeight(int index)
{
    fmtdesc.index = 0;
    fmtdesc.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if(ioctl(fd0, VIDIOC_ENUM_FMT, &fmtdesc) == -1)
        return -1;

    frmsizeenum.pixel_format = fmtdesc.pixelformat;

    frmsizeenum.index = index;
    if(ioctl(fd0, VIDIOC_ENUM_FRAMESIZES, &frmsizeenum) != -1)
        return frmsizeenum.discrete.height;
    else
        return -1;
}
int LPF_GetCurResWidth0()
{
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd0, VIDIOC_G_FMT, &format) == -1)
        return -1;
    return format.fmt.pix.width;
}
int LPF_GetCurResWidth1()
{
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd1, VIDIOC_G_FMT, &format) == -1)
        return -1;
    return format.fmt.pix.width;
}
int LPF_GetCurResWidth2()
{
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd2, VIDIOC_G_FMT, &format) == -1)
        return -1;
    return format.fmt.pix.width;
}
int LPF_GetCurResWidth3()
{
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd3, VIDIOC_G_FMT, &format) == -1)
        return -1;
    return format.fmt.pix.width;
}
int LPF_GetCurResWidth4()
{
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd4, VIDIOC_G_FMT, &format) == -1)
        return -1;
    return format.fmt.pix.width;
}
int LPF_GetCurResHeight0()
{
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd0, VIDIOC_G_FMT, &format) == -1)
        return -1;
    return format.fmt.pix.height;
}
int LPF_GetCurResHeight1()
{
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd1, VIDIOC_G_FMT, &format) == -1)
        return -1;
    return format.fmt.pix.height;
}
int LPF_GetCurResHeight2()
{
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd2, VIDIOC_G_FMT, &format) == -1)
        return -1;
    return format.fmt.pix.height;
}
int LPF_GetCurResHeight3()
{
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd3, VIDIOC_G_FMT, &format) == -1)
        return -1;
    return format.fmt.pix.height;
}
int LPF_GetCurResHeight4()
{
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd4, VIDIOC_G_FMT, &format) == -1)
        return -1;
    return format.fmt.pix.height;
}

char* LPF_GetCurPixformat()
{
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd0, VIDIOC_G_FMT, &format) == -1)
        return NULL;

    memset(devFmtDesc, 0, sizeof(devFmtDesc));

    char fmt[5] = "";
    sprintf(fmt, "%c%c%c%c",
            (__u8)(format.fmt.pix.pixelformat&0XFF),
            (__u8)((format.fmt.pix.pixelformat>>8)&0XFF),
            (__u8)((format.fmt.pix.pixelformat>>16)&0XFF),
            (__u8)((format.fmt.pix.pixelformat>>24)&0XFF));

    strncpy(devFmtDesc, fmt, 4);
    return devFmtDesc;
}

int LPF_GetCurFps()
{
    if(videoIsRun0 < 0)
    {
        return -1;
    }

    struct v4l2_streamparm parm;
    parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd0, VIDIOC_G_PARM, &parm) == -1)
        return -1;
//frmival.discrete.denominator / frmival.discrete.numerator
    return parm.parm.capture.timeperframe.denominator/parm.parm.capture.timeperframe.numerator;
}

int LPF_GetExposureMode()
{
    if(videoIsRun0 < 0)
    {
        return -1;
    }

    struct v4l2_control control_s;
    control_s.id = V4L2_CID_EXPOSURE_AUTO;

    if(ioctl(fd0,VIDIOC_G_CTRL,&control_s)==-1)
    {
        perror("ioctl LPF_GetExposureMode");
        return -1;
        //exit(EXIT_FAILURE);
    }

    return control_s.value;
}


#ifdef __cplusplus
}
#endif



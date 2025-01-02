/************************************
buffer queue 
author:zhukeyu
date:2021-4-20
************************************/
#ifndef __BUFFER_QUEUE_H__
#define __BUFFER_QUEUE_H__

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define QUEUE_CONSUMER_MAX	    10
#define BUFFER_QUEUE_FLAG       0x0FFFFFFF

//数据类型
typedef enum __BUFFER_TYPE_ENUM
{
	BUFFER_TYPE_UNKNOWN		=		0x00000000,
	BUFFER_TYPE_VIDEO		=		0x00000001,
	BUFFER_TYPE_AUDIO,
	BUFFER_TYPE_EVENT,
	BUFFER_TYPE_TEXT
}BUFFER_TYPE_ENUM;

//包头信息
typedef struct __BUFFER_HEADER_T
{
	unsigned int		size;			//自身包头长度
	unsigned int		id;				//队列Id
	BUFFER_TYPE_ENUM	type;			//数据类型(音频、视频、事件、文本)
	unsigned int		flag;			//BUFFER_QUEUE_FLAG		用于检验包完整性
	unsigned int		timestampSec;   //包写入时间
	int		            headerSize;		//帧头大小
	int		            payloadSize;    //帧数据大小
}__attribute__ ((packed)) BUFFER_HEADER_T; //字节对齐


//节点信息
typedef struct __BUFFER_NODE_T
{
	unsigned int	pos;                //偏移位置
	unsigned int	timestampSec;		//帧写入时间
}__attribute__ ((packed)) BUFFER_NODE_T;


//队列头信息
typedef struct __QUEUE_HEADER_T
{
	unsigned int	queueSize;				//整个缓冲队列大小
	unsigned int	writePos;				//写位置偏移量
	unsigned int	totalSize;				//当前总字节数

//------------------------------------------//帧节点信息
	unsigned int	buffNodeNum;			//BufferNode个数
	unsigned int	nodeIndex;				//当前写帧节点
	unsigned int	updateNodeList;			//是否需要更新NodeList
	unsigned int	firstNodeIndex;			//最早的NodeList下标值, 表示最早的一条记录
	unsigned int    firstNodePos;           //最早的节点位置
	unsigned int    firstBufferSize;        //最早到现在总长
//------------------------------------------//以下是内部计数
	unsigned int	videoFrameNum;
	unsigned int	audioFrameNum;
	unsigned int	eventFrameNum;
	unsigned int	textFrameNum;
}__attribute__ ((packed)) QUEUE_HEADER_T;

//消费者结构
typedef struct __QUEUE_CONSUMER_T
{
	unsigned long	consumerId;    //消费者ID
	unsigned int	readPos;       //读地址偏移
	unsigned int	totalSize;     //总大小
	bool            bGetHistory;   //从历史数据获取数据标记
//----------------------------------//以下是内部计数
	unsigned int	videoFrameNum;  //视频帧计数
	unsigned int	audioFrameNum;  //音频帧计数
	unsigned int	eventFrameNum;  //事件帧计数
	unsigned int	textFrameNum;   //文本帧计数
}QUEUE_CONSUMER_T;

//缓冲队列对象结构
typedef struct __BUFFER_QUEUE_OBJ_T
{
	unsigned int 		id;           		//队列ID
	char				name[36];     		//队列名称
	pthread_mutex_t 	hMutex;             //互斥锁       
	QUEUE_HEADER_T	    *pQueHeader;        //队列头部
	char			    *pQueData;          //队列数据
	BUFFER_NODE_T	    *pNodeInfoList;     //队列节点信息表
	QUEUE_CONSUMER_T	*pConsumer;         //消费者信息表
}BUFFER_QUEUE_OBJ_T;

typedef void *BUFFQUEUE_HANDLE;				//生产者队列句柄
typedef void *CONSUMER_HANDLE;				//消费者句柄

#ifdef __cplusplus
extern "C" 
{
#endif

//==================================================================================================

/* 创建生产者队列  */
/* handle 队列句柄 */
/* queueId 队列ID值 */
/* queueName 队列名称，可有可无 */
/* queueSize 队列总大小，不能小于2*BUFFER_HEADER_T，建议大于queueElem可缓存数据大小的2倍 */
/* queueElem 队列总元素，不能小于30 */
int Buffer_Queue_Producer_Create(BUFFQUEUE_HANDLE *handle, unsigned int queueId, char *queueName,unsigned int queueSize,unsigned int queueElem);

/* 销毁生产者队列 */
int Buffer_Queue_Producer_Destroy(BUFFQUEUE_HANDLE *handle);

/* 消费者注册 */
/* handle 队列句柄 */
/* consumerId 消费者唯一ID */
/* getHistoryData  0:获取最新数据 1: 获取历史数据 */
/* return CONSUMER_HANDLE 消费者句柄 */
CONSUMER_HANDLE Buffer_Queue_Consumer_Register(BUFFQUEUE_HANDLE handle, unsigned long consumerId,int getHistoryData);

/* 消费者注销 */
void Buffer_Queue_Consumer_Release(BUFFQUEUE_HANDLE handle, CONSUMER_HANDLE *consumerHandle);

/* 生产者往队列增加数据 */
int Buffer_Queue_AddData(BUFFQUEUE_HANDLE handle, unsigned int bufferId, BUFFER_TYPE_ENUM bufferType, 
							int headerSize, char *headerData, int payloadSize, char *payloadData);

/* 消费者从队列获取数据 */
int Buffer_Queue_GetData(BUFFQUEUE_HANDLE handle, CONSUMER_HANDLE consumerHandle, unsigned int *bufferId, 
						BUFFER_TYPE_ENUM *bufferType, int *headerSize, char *headerData, int *payloadSize, char *payloadData);

//====================================================================================================

#ifdef __cplusplus
}
#endif

#endif 

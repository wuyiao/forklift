/************************************
buffer queue
author:Mr.Lu
date:2022-12-13
************************************/
#include "buffer_queue.h"

//生产队列创建
int Buffer_Queue_Producer_Create(BUFFQUEUE_HANDLE *handle, unsigned int queueId, char *queueName, unsigned int queueSize,unsigned int queueElem)
{
    unsigned int totalSize = 0;
    
    //队列大小不能小于两个BUFFER_HEADER_T
	if (queueSize < sizeof(BUFFER_HEADER_T)*2){
        printf("Buffer_Queue_Producer_Create Err1!\n");
        return -1;
    }
    //队列元素不能小于30个    
    if(queueElem < 30){
        printf("Buffer_Queue_Producer_Create Err2!\n");
        return -1;    
    }    

    /* [BUG3] 修复，用户按queueSize申请内存，但程序却将内部的头信息存储进空间中，导致实际可用空间小于申请空间 */
    totalSize = sizeof(BUFFER_HEADER_T) * queueElem + queueSize;
    
    //创建队列句柄
    BUFFER_QUEUE_OBJ_T *pObj = (BUFFER_QUEUE_OBJ_T *)malloc(sizeof(BUFFER_QUEUE_OBJ_T));
	if (NULL == pObj){
        printf("Buffer_Queue_Producer_Create Err3!\n");
        return -1;
    }
	memset(pObj, 0x00, sizeof(BUFFER_QUEUE_OBJ_T));
	pObj->id = queueId;
	if (NULL != queueName && ((int)strlen(queueName)>0)){
        strcpy(pObj->name, queueName);
    }	
    pthread_mutex_init (&pObj->hMutex,NULL);

    int ret = -1;
    //创建队列
    do{
        //创建队列头部,用于存储帧信息结构
        pObj->pQueHeader = (QUEUE_HEADER_T *)malloc(sizeof(QUEUE_HEADER_T));
        if (NULL == pObj->pQueHeader){
            break;
        }			
        memset(pObj->pQueHeader, 0x00, sizeof(QUEUE_HEADER_T));
        //创建队列节点信息,用于记录帧节点信息
        pObj->pNodeInfoList = (BUFFER_NODE_T *) malloc(sizeof(BUFFER_NODE_T) * queueElem);
        if (NULL == pObj->pNodeInfoList){
            break;
        }		
        memset(&pObj->pNodeInfoList[0], 0x00, sizeof(BUFFER_NODE_T) * queueElem);
        pObj->pQueHeader->buffNodeNum = queueElem;
        //创建队列数据存放区，用于存储帧数据
        pObj->pQueData = (char *)malloc(sizeof(char) * totalSize);
        if (NULL == pObj->pQueData){
            break;
        }				
        pObj->pQueHeader->queueSize = totalSize;
        //创建消费者列表,用于记录消费者消费信息
        pObj->pConsumer = (QUEUE_CONSUMER_T *)malloc(sizeof(QUEUE_CONSUMER_T) * QUEUE_CONSUMER_MAX);
        if (NULL==pObj->pConsumer){
            break;
        }				
        memset(pObj->pConsumer, 0x00, sizeof(QUEUE_CONSUMER_T) * QUEUE_CONSUMER_MAX);
        ret = 0;
    }while(0);
    if(ret < 0){
        //创建队列失败，则销毁，释放已申请的内存
        ret = Buffer_Queue_Producer_Destroy((BUFFER_QUEUE_OBJ_T *)&pObj);
        printf("Buffer_Queue_Producer_Create Err4!\n");
        return -1;
    }
    *handle = pObj;
	return 0;
}

//生产队列销毁
int Buffer_Queue_Producer_Destroy(BUFFQUEUE_HANDLE *handle)
{
    BUFFER_QUEUE_OBJ_T *pObj = (BUFFER_QUEUE_OBJ_T *)*handle;
    if (NULL == pObj){
        printf("Buffer_Queue_Producer_Destroy Err1!\n");
        return -1;
    }
    if (NULL != pObj->pQueHeader)
    {
        free(pObj->pQueHeader);
        pObj->pQueHeader = NULL;
    }
    if (NULL != pObj->pQueData)
    {
        free(pObj->pQueData);
        pObj->pQueData = NULL;
    }	
    if (NULL != pObj->pNodeInfoList)
    {
        free(pObj->pNodeInfoList);
        pObj->pNodeInfoList = NULL;
    }
    if(NULL != pObj->pConsumer)
    {
        free(pObj->pConsumer);
        pObj->pConsumer = NULL;
    }
    pthread_mutex_destroy(&pObj->hMutex);
    free(pObj);
	pObj = NULL;
	*handle = NULL; 
    return 0;   		
}

//头域检查
int	Buffer_Queue_CheckBufferHeader(BUFFER_HEADER_T *pBufferHeader)
{
	if (NULL == pBufferHeader)		return 0;
	
	if (pBufferHeader->size != sizeof(BUFFER_HEADER_T))	    return -1;
	if (pBufferHeader->flag != BUFFER_QUEUE_FLAG)		    return -1;
    
	return 0;
}

//消费者注册
CONSUMER_HANDLE Buffer_Queue_Consumer_Register(BUFFQUEUE_HANDLE handle, unsigned long consumerId,int getHistoryData)
{
    BUFFER_QUEUE_OBJ_T *pObj = (BUFFER_QUEUE_OBJ_T *)handle;
    if (NULL == pObj){
        printf("Buffer_Queue_Consumer_Register Err1!\n");
        return NULL;
    }
    int i = 0,index = 0;
    CONSUMER_HANDLE	consumerHandle = NULL;

    //检查是否已存在
	for (i=0; i < QUEUE_CONSUMER_MAX; i++)
	{
		if (pObj->pConsumer[i].consumerId == consumerId){
			consumerHandle = (CONSUMER_HANDLE)&pObj->pConsumer[i];//消费者句柄地址值
            index = i;
			break;
		}
	}
    //不存在, 则分配
	if (NULL == consumerHandle){
		for (i=0; i < QUEUE_CONSUMER_MAX; i++)
		{
			if (pObj->pConsumer[i].consumerId < 1){
				pObj->pConsumer[i].consumerId = consumerId;
				consumerHandle = (CONSUMER_HANDLE)&pObj->pConsumer[i];
                index = i;
				break;
			}
		}
	}
    
    if (NULL == consumerHandle){
        printf("BUFFQUEUE_HANDLE:0x%x Consumer is FULL.....\n",pObj);//消费者已经满了
        return NULL;
    }
    printf("Consumer Register Success Id:%d \n",consumerId);
    pObj->pConsumer[index].bGetHistory  = getHistoryData > 0 ? true : false;
    if(getHistoryData == 0){ //获取当前最新数据
        pObj->pConsumer[index].readPos = pObj->pQueHeader->writePos;//消费者读位置指定为队列最新帧位置
        pObj->pConsumer[index].totalSize = 0;
		pObj->pConsumer[index].videoFrameNum = 0;
		pObj->pConsumer[index].audioFrameNum = 0;
		pObj->pConsumer[index].eventFrameNum = 0;
		pObj->pConsumer[index].textFrameNum = 0;
        printf("BUFFQUEUE_HANDLE:0x%x index:%d Consumer is Get New Data......\n",pObj,index);
    }else{
        pObj->pConsumer[index].readPos = pObj->pQueHeader->firstNodePos;
        pObj->pConsumer[index].totalSize = pObj->pQueHeader->firstBufferSize;
        printf("BUFFQUEUE_HANDLE:0x%x index:%d Consumer is Get History Data ......\n",pObj,index);
        printf("History Data Pos:%d totalSize:%d \n",pObj->pQueHeader->firstNodePos,pObj->pQueHeader->firstBufferSize);
    }
    return consumerHandle;
}

//消费者注销
void Buffer_Queue_Consumer_Release(BUFFQUEUE_HANDLE handle, CONSUMER_HANDLE *consumerHandle)
{
    int i = 0;
	BUFFER_QUEUE_OBJ_T *pObj = (BUFFER_QUEUE_OBJ_T *)handle;
	if (NULL == pObj || NULL == *consumerHandle){
        printf("Buffer_Queue_Consumer_Release Err1!");
        return;
    }				
	for (i = 0; i < QUEUE_CONSUMER_MAX; i++)
	{
		if (&pObj->pConsumer[i] == *consumerHandle) //查找消费者
		{
            printf("Consumer_Release consumerId:%d \n", pObj->pConsumer[i].consumerId);
			memset(&pObj->pConsumer[i], 0x00, sizeof(QUEUE_CONSUMER_T));//对应的消费者信息删除
			*consumerHandle = NULL;
			break;
		}
	}
}

//环形写队列数据
int Buffer_Queue_WriteRing(BUFFQUEUE_HANDLE handle,int dataSize,char *dataPtr)
{
    BUFFER_QUEUE_OBJ_T *pObj = (BUFFER_QUEUE_OBJ_T *)handle;
    if(NULL == pObj) return -1;
    int spaceLen = pObj->pQueHeader->queueSize - pObj->pQueHeader->writePos;	//剩余空间
    if(spaceLen <= 0){
        pObj->pQueHeader->writePos = 0;
        spaceLen = pObj->pQueHeader->queueSize;

        /* [BUG1]修改处，spaceLen则写指针必定回环 */
        pObj->pQueHeader->updateNodeList = 1;
    }
    if(spaceLen >= dataSize){
        memcpy(pObj->pQueData + pObj->pQueHeader->writePos,dataPtr,dataSize);//填充队列
        pObj->pQueHeader->writePos += dataSize; //队列写位置更新
    }else{
        memcpy(pObj->pQueData + pObj->pQueHeader->writePos,dataPtr,spaceLen);//填充队尾
        memcpy(pObj->pQueData,dataPtr + spaceLen,dataSize - spaceLen);//填充队头
        pObj->pQueHeader->writePos = dataSize - spaceLen;//队列写位置更新

        /* [BUG1]删除，如果queueSize与写入dataSize刚好成倍数关系，会导致写指针回头了但不会执行该语句，导致缺陷 */
/*      pObj->pQueHeader->updateNodeList = 1; */
    }
    return 0;
}
//写队列数据
int Buffer_Queue_WriteData(BUFFQUEUE_HANDLE handle,BUFFER_HEADER_T *headerInfo,BUFFER_NODE_T *nodeInfo,
                            int headerSize,char *headerData, int payloadSize, char *payloadData)
{
    BUFFER_QUEUE_OBJ_T *pObj = (BUFFER_QUEUE_OBJ_T *)handle;
    if(NULL == pObj) return -1;
    int i = 0,offset = 0;//偏移位置
    int dataLen = sizeof(BUFFER_HEADER_T) + headerSize + payloadSize;
    if(dataLen >= pObj->pQueHeader->queueSize){ //当前要进的数据过大
        printf("Buffer_Queue_WriteData Err1!\n");
        return -1;
    }
    //依次写数据到队列
    Buffer_Queue_WriteRing(handle,sizeof(BUFFER_HEADER_T),headerInfo);
    Buffer_Queue_WriteRing(handle,headerSize,headerData);
    Buffer_Queue_WriteRing(handle,payloadSize,payloadData);
    //printf("write headerSize:%d payloadSize:%d \n",headerSize,payloadSize);

    //更新队列帧节点
    if (NULL != pObj->pNodeInfoList){
		int nodeIndex = pObj->pQueHeader->nodeIndex; //当前节点下标
		memcpy(&pObj->pNodeInfoList[nodeIndex], nodeInfo, sizeof(BUFFER_NODE_T));//记录位置偏移量pos
		pObj->pQueHeader->nodeIndex ++;

		if (pObj->pQueHeader->nodeIndex >= pObj->pQueHeader->buffNodeNum){ //大于总帧数
			pObj->pQueHeader->nodeIndex = 0;
		}
		if (pObj->pQueHeader->updateNodeList == 0x01){//需要更新第一帧
            for(i = 0;i < (int)pObj->pQueHeader->buffNodeNum - 1;i++)
            {
                if(pObj->pNodeInfoList[i].pos < pObj->pQueHeader->writePos && pObj->pNodeInfoList[i+1].pos >= pObj->pQueHeader->writePos)
                {
                    pObj->pQueHeader->firstNodeIndex = i + 1;//寻找未被覆盖的最早帧节点 
                    pObj->pQueHeader->firstNodePos = pObj->pNodeInfoList[i+1].pos;
                    //统计实际数据长度
                    pObj->pQueHeader->firstBufferSize = pObj->pQueHeader->queueSize - pObj->pNodeInfoList[i+1].pos + pObj->pQueHeader->writePos;
                    break;          
                }
            }
		}else{
            pObj->pQueHeader->firstNodePos = 0;//队列从未覆盖的时候是队列头地址
        }
	}
    return 0;
}
//队列加入帧数据
int Buffer_Queue_AddData(BUFFQUEUE_HANDLE handle, unsigned int bufferId, BUFFER_TYPE_ENUM bufferType, 
						int headerSize, char *headerData, int payloadSize, char *payloadData)
{
    BUFFER_QUEUE_OBJ_T *pObj = (BUFFER_QUEUE_OBJ_T *)handle;
    if(NULL == pObj || NULL == headerData || NULL == payloadData){
        printf("Buffer_Queue_AddData Err1!\n");
        return -1;
    }
    if(NULL == pObj->pQueHeader || NULL == pObj->pQueData){
        printf("Buffer_Queue_AddData Err2!\n");
        return -1;
    }			
    if (headerSize < 1 || payloadSize <= 0){
        printf("Buffer_Queue_AddData Err3!\n");
		return -1;
	}
    int ret = -1,i = 0;
    BUFFER_HEADER_T		bufferHeader;//头信息
	BUFFER_NODE_T		bufferNode;//帧节点
    //队头赋值
	memset(&bufferHeader, 0x00, sizeof(BUFFER_HEADER_T));
	bufferHeader.size = sizeof(BUFFER_HEADER_T);
	bufferHeader.id = bufferId;
	bufferHeader.type = bufferType;
	bufferHeader.flag = BUFFER_QUEUE_FLAG;
	bufferHeader.headerSize = headerSize;
	bufferHeader.payloadSize = payloadSize;
    //节点赋值
	memset(&bufferNode, 0x00, sizeof(BUFFER_NODE_T));
	bufferNode.pos = pObj->pQueHeader->writePos;
    int dataLen = sizeof(BUFFER_HEADER_T) + headerSize + payloadSize;
    ret = Buffer_Queue_WriteData(handle,&bufferHeader,&bufferNode,headerSize,headerData,payloadSize,payloadData);//写队列数据
    if(ret < 0) 
    {
        printf("Buffer_Queue_AddData Err4!\n");
        return -1;
    }

    //帧计数++
	if (bufferType==BUFFER_TYPE_VIDEO)				pObj->pQueHeader->videoFrameNum ++;
	else if (bufferType==BUFFER_TYPE_AUDIO)			pObj->pQueHeader->audioFrameNum ++;
	else if (bufferType==BUFFER_TYPE_EVENT)			pObj->pQueHeader->eventFrameNum ++;
	else if (bufferType==BUFFER_TYPE_TEXT)			pObj->pQueHeader->textFrameNum ++;

    pthread_mutex_lock(&pObj->hMutex);
    for (i = 0; i < QUEUE_CONSUMER_MAX; i++) //同步所有消费者队列总数据
    {
        if (pObj->pConsumer[i].consumerId > 0){
            pObj->pConsumer[i].totalSize += dataLen;
            if (bufferType==BUFFER_TYPE_VIDEO)				pObj->pConsumer[i].videoFrameNum ++;
            else if (bufferType==BUFFER_TYPE_AUDIO)			pObj->pConsumer[i].audioFrameNum ++;
            else if (bufferType==BUFFER_TYPE_EVENT)			pObj->pConsumer[i].eventFrameNum ++;
            else if (bufferType==BUFFER_TYPE_TEXT)			pObj->pConsumer[i].textFrameNum ++;
        }
    }
    pthread_mutex_unlock(&pObj->hMutex);
    return 0;
}

//更新读位置
int	Buffer_Queue_UpdateReadPos(BUFFER_QUEUE_OBJ_T *pObj, CONSUMER_HANDLE handle)
{
	QUEUE_CONSUMER_T	*pConsumer = (QUEUE_CONSUMER_T *)handle;
	if (NULL == pObj)				return -1;
	if (NULL == pObj->pQueHeader)	return -1;
	if (NULL == pConsumer)			return -1;

    /* [BUG2] 修改内容 */
    if (pConsumer->bGetHistory == true)
    {
        pConsumer->readPos = pObj->pQueHeader->firstNodePos;//队列生产当前写帧位置
        pConsumer->totalSize = pObj->pQueHeader->firstBufferSize;
    }
    /* [BUG2] 直接更新消费者读指针会导致读历史数据的消费者丢失历史数据 */
    else
    {
        pConsumer->readPos = pObj->pQueHeader->writePos;//队列生产当前写帧位置
        pConsumer->totalSize = 0;
        pConsumer->videoFrameNum = 0;
        pConsumer->audioFrameNum = 0;
        pConsumer->eventFrameNum = 0;
        pConsumer->textFrameNum = 0;
    }

	return 0;
}

//环形读队列数据
int Buffer_Queue_ReadRing(BUFFQUEUE_HANDLE handle, CONSUMER_HANDLE consumerHandle,int dataSize,char *DataPtr)
{
    BUFFER_QUEUE_OBJ_T  *pObj = (BUFFER_QUEUE_OBJ_T *)handle;
	QUEUE_CONSUMER_T	*pConsumer = (QUEUE_CONSUMER_T *)consumerHandle;
    if(dataSize >= (int)pObj->pQueHeader->queueSize) return -1;//读取数据过大
    
    int spaceLen = pObj->pQueHeader->queueSize - pConsumer->readPos;
    if(spaceLen <= 0){
        pConsumer->readPos = 0;
        spaceLen = pObj->pQueHeader->queueSize;
    }
    if(spaceLen >= dataSize){
        memcpy(DataPtr,pObj->pQueData + pConsumer->readPos,dataSize);
        pConsumer->readPos += dataSize;
    }else{
        memcpy(DataPtr,pObj->pQueData + pConsumer->readPos,spaceLen);
        memcpy(DataPtr + spaceLen,pObj->pQueData,dataSize - spaceLen);
        pConsumer->readPos = dataSize - spaceLen;
    }
    return 0;
}
//消费者获取数据
int Buffer_Queue_GetData(BUFFQUEUE_HANDLE handle, CONSUMER_HANDLE consumerHandle, 
                        unsigned int *bufferId, BUFFER_TYPE_ENUM *bufferType, 
						int *headerSize, char *headerData, int *payloadSize, char *payloadData)
{
    int ret = -1;
    BUFFER_QUEUE_OBJ_T  *pObj = (BUFFER_QUEUE_OBJ_T *)handle;
	QUEUE_CONSUMER_T	*pConsumer = (QUEUE_CONSUMER_T *)consumerHandle;
    if(NULL == pObj || NULL == pConsumer){
        printf("Buffer_Queue_GetData Err1!\n");
        return -1;
    }
    if (pConsumer->totalSize < sizeof(BUFFER_HEADER_T)){ //队列无数据则返回
        return -1;
    }
    if (pConsumer->readPos >= pObj->pQueHeader->queueSize){ //已读到尾部
		pConsumer->readPos = 0;
	}
    BUFFER_HEADER_T bufferHeader;
    memset(&bufferHeader,0x00,sizeof(BUFFER_HEADER_T));
    //从队列读取数据
    if(pConsumer->readPos < pObj->pQueHeader->queueSize){
        Buffer_Queue_ReadRing(handle,pConsumer,sizeof(BUFFER_HEADER_T),(char *)&bufferHeader);
        ret = Buffer_Queue_CheckBufferHeader(&bufferHeader);
        if(ret == -1){
            if (pConsumer->readPos != pObj->pQueHeader->writePos){
                Buffer_Queue_UpdateReadPos(pObj, consumerHandle);
                printf("[%d] Buffer_Queue_UpdateReadPos.....\n",__LINE__);
            }
            printf("Buffer_Queue_GetData Err2!\n");
            return -1;
        }
        if(pConsumer->totalSize < sizeof(BUFFER_HEADER_T) + bufferHeader.headerSize + bufferHeader.payloadSize){
            printf("Buffer_Queue_GetData Err3!\n");
            return -1;
        } 
        //printf("read headerSize:%d payloadSize:%d \n",bufferHeader.headerSize,bufferHeader.payloadSize);
        Buffer_Queue_ReadRing(handle,pConsumer,bufferHeader.headerSize,headerData);
        Buffer_Queue_ReadRing(handle,pConsumer,bufferHeader.payloadSize,payloadData);
    }else{
        Buffer_Queue_UpdateReadPos(pObj, consumerHandle);
        printf("[%d] Buffer_Queue_UpdateReadPos.....\n",__LINE__);
        return -1;
    }
    if (NULL != bufferId)		*bufferId = bufferHeader.id;
    if (NULL != bufferType)		*bufferType = bufferHeader.type;
    if (NULL != headerSize)		*headerSize = bufferHeader.headerSize;
    if (NULL != payloadSize)	*payloadSize = bufferHeader.payloadSize;
    if (bufferHeader.type == BUFFER_TYPE_VIDEO && pConsumer->videoFrameNum>0)		pConsumer->videoFrameNum --;
    else if (bufferHeader.type == BUFFER_TYPE_AUDIO && pConsumer->audioFrameNum>0)	pConsumer->audioFrameNum --;
    else if (bufferHeader.type == BUFFER_TYPE_EVENT && pConsumer->eventFrameNum>0)	pConsumer->eventFrameNum --;
    else if (bufferHeader.type == BUFFER_TYPE_TEXT && pConsumer->textFrameNum>0)		pConsumer->textFrameNum --;
    
    pthread_mutex_lock(&pObj->hMutex);
    pConsumer->totalSize -= ((int)sizeof(BUFFER_HEADER_T) + bufferHeader.headerSize + bufferHeader.payloadSize);
    pthread_mutex_unlock(&pObj->hMutex);
    return 0;
}

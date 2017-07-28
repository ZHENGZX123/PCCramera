//
//  KYLLocalMP4FilePlayer.cpp
//  P2PCamera
//
//
//

#include "KYLLocalMP4FilePlayer.h"
#import "pthread.h"
#import <sys/time.h>
#import "SE_AudioCodec.h"


KYLLocalMP4FilePlayer::KYLLocalMP4FilePlayer()
{
    m_pAudioPlayer = nil;
    m_bAudioRunning = 0;
    m_pAudioPlayer = [[KYLOpenALPlayer alloc] init];
    m_pMp4videoRecordToolHandle = NULL;
    m_playbackDelegate = nil;
    m_PlaybackThreadID = NULL;
    m_bPlaybackThreadRuning = 0;
    m_bPause = NO;
    sumTime=0;
    m_playbackLock = [[NSCondition alloc] init];
    isPlayOver=NO;
    int nRet = SEMP4Read_Create(&m_pMp4videoRecordToolHandle);
    if(nRet < 0)
    {
        SEMP4Read_Destroy(&m_pMp4videoRecordToolHandle);
        m_pMp4videoRecordToolHandle = NULL;
    }
}

KYLLocalMP4FilePlayer::~KYLLocalMP4FilePlayer()
{
    StopPlayback();
    if (m_playbackLock != nil) {
        [m_playbackLock release];
        m_playbackLock = nil;
    }
    sumTime=0;
    if(m_pMp4videoRecordToolHandle)
    {
        SEMP4Read_Destroy(&m_pMp4videoRecordToolHandle);
        m_pMp4videoRecordToolHandle = NULL;
    }
    
    if (m_pAudioPlayer) {
        [m_pAudioPlayer stopSound];
        [m_pAudioPlayer cleanUpOpenAL];
        [m_pAudioPlayer release];
        m_pAudioPlayer = nil;
    }
}

void* KYLLocalMP4FilePlayer::PlaybackThread(void *param)
{
    KYLLocalMP4FilePlayer *pPlayback = (KYLLocalMP4FilePlayer*)param;
    NSAutoreleasePool *apool = [[NSAutoreleasePool alloc] init];
    pPlayback->PlaybackProcess();
    [apool release];
    
    // NSLog(@"PlaybackThread end");
    return NULL;
}

void KYLLocalMP4FilePlayer::StopPlayback()
{
    m_bPlaybackThreadRuning = 0;
    if (m_PlaybackThreadID != NULL) {
        pthread_join(m_PlaybackThreadID, NULL);
        m_PlaybackThreadID = NULL;
    }
    
    SEMP4Read_CloseMp4File(m_pMp4videoRecordToolHandle);
}

BOOL KYLLocalMP4FilePlayer::CustomSleep(int uNum)
{    
    sumTime+=1;
    int i = 0;
    for (i = 0; i < uNum; i++) {
        sumTime+=1;
        if (!m_bPlaybackThreadRuning) {
            return NO;
        }
        
        usleep(1000);
    }
    return YES;
}


void KYLLocalMP4FilePlayer::PlaybackProcess()
{
    unsigned int oldtimestamp = 0;
    
    //unsigned int startTimestamp = 0;
    UCHAR *m_pVideoHandleH264 = NULL;
    int nRet = SEVideo_Create(VIDEO_CODE_TYPE_H264, &m_pVideoHandleH264);
    if (nRet <= 0) {
        return;
    }
    
    UCHAR *m_pAudioHandleG726 = NULL;
    UCHAR *m_pAudioHandleAdpcm = NULL;
    UCHAR *m_pAudioHandleG711 = NULL;
    UCHAR *m_pAudioHandleAAC = NULL;
    
    
    RTSP_ENUM_AV_CODECID eAudioAVCodecID = m_stFileInfo.eAudioAVCodecID;
    if (eAudioAVCodecID == RTSP_AV_CODECID_AUDIO_ADPCM) {
        SEAudio_Create(AUDIO_CODE_TYPE_ADPCM, &m_pAudioHandleAdpcm);
    }
    else if (eAudioAVCodecID == RTSP_AV_CODECID_AUDIO_G726) {
        SEAudio_Create(AUDIO_CODE_TYPE_G726, &m_pAudioHandleG726);
    }
    else if (eAudioAVCodecID == RTSP_AV_CODECID_AUDIO_G711A) {
        SEAudio_Create(AUDIO_CODE_TYPE_G711A, &m_pAudioHandleG711);
    }
    else if (eAudioAVCodecID == RTSP_AV_CODECID_AUDIO_AAC) {
        SEAudio_Create(AUDIO_CODE_TYPE_AAC, &m_pAudioHandleAAC);
    }
    
    INT32   nAudioSamplerate = m_stFileInfo.nAudioSamplerate;
//    INT32   nAudioDatabit = m_stFileInfo.nAudioDatabit;
//    INT32   nAudioChannels = m_stFileInfo.nAudioChannels;
//    
//    INT32   nDurationInSecond = m_stFileInfo.nDurationInSecond;
//    INT64   nFileSizeInByte = m_stFileInfo.nFileSizeInByte;
    
    int nSamplingRate = nAudioSamplerate;
    int nAudioFormat = AL_FORMAT_MONO16;
    if (m_pAudioPlayer) {
        [m_pAudioPlayer initOpenAL:nAudioFormat :nSamplingRate];
    }
    
    
    char *pBuf=(char *)malloc(KYL_MAX_SIZE_YUV);
    char *outYUV420=(char *)malloc(KYL_MAX_SIZE_YUV);
    while (m_bPlaybackThreadRuning) {
        if (m_bPause) {
            usleep(10000);
            continue;
        }
        //read data head
        RTSP_STREAM_HEAD *datahead;
        //memset(&datahead, 0, sizeof(datahead));
        
        nRet = SEMP4Read_ReadOneFrame(m_pMp4videoRecordToolHandle, (UCHAR *)pBuf, KYL_MAX_SIZE_YUV);
        if (nRet < 0) {
            NSLog(@"SEMP4Read_ReadOneFrame is error = %d", nRet);
            [m_playbackLock lock];
            isPlayOver=YES;
            [m_playbackDelegate didLocalMP4FilePlayerStoped];
            [m_playbackLock unlock];
            //KYL_SAFE_DELETE_ARR(outYUV420);
            free(pBuf);
            pBuf = NULL;
            free(outYUV420);
            outYUV420 = NULL;
            return;
        }
        datahead = (RTSP_STREAM_HEAD *) pBuf;
        UINT32 nAVCodecID = datahead->nAVCodecID;		 // refer to RTSP_ENUM_AV_CODECID
        //nParameter:
        //		Video: refer to RTSP_ENUM_VIDEO_FRAME.
        //		Audio:(samplerate << 2) | (databits << 1) | (channel), samplerate refer to MP4_ENUM_AUDIO_SAMPLERATE; databits refer to MP4_ENUM_AUDIO_DATABITS; channel refer to MP4_ENUM_AUDIO_CHANNEL
        //CHAR   nParameter = datahead->nParameter;
        
        
        UINT32 nStreamDataLen = datahead->nStreamDataLen;	// Stream data size after following struct 'RTSP_STREAM_HEAD'
        UINT32 nTimestamp = datahead->nTimestamp;		// Timestamp of the frame, in milliseconds
        //char *pRawData = (char *)malloc(nStreamDataLen);
        //memset(pRawData, 0, nStreamDataLen);
        //memcpy(pRawData, pBuf + sizeof(RTSP_STREAM_HEAD), nStreamDataLen);
        
        
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        //NSLog(@"播放本地录像线程，读取一帧mp4, nAVCodeID=%d",nAVCodecID);
        char *pRawData = (char *)(pBuf + sizeof(RTSP_STREAM_HEAD));
        
        UINT32 Length = nStreamDataLen;
        UINT32 nOutLen = 20*Length;
        UCHAR *pHandle = NULL;
        UCHAR *inBuf = (UCHAR *)pRawData;
        UINT32 inLen = (UINT32)Length;
        
        if (nAVCodecID == RTSP_AV_CODECID_VIDEO_MJPEG) {
            [m_playbackLock lock];
            [m_playbackDelegate didLocalMP4FilePlayerReceivedPlaybackMJPEGData:pRawData length:nStreamDataLen timestamp:nTimestamp];
            [m_playbackLock unlock];
        }
        else if (nAVCodecID == RTSP_AV_CODECID_VIDEO_H264) {
            int  nRet=0, nWidth=0, nHeigh=0;
            ULONG in_outLen=KYL_MAX_SIZE_YUV;
            
            
            nRet=SEVideo_Decode2YUV(m_pVideoHandleH264, (UCHAR *)pRawData, nStreamDataLen, (UCHAR *)outYUV420, &in_outLen, &nWidth,&nHeigh);
            //NSLog(@"----------------Record Decode result = %d",nRet);
            [m_playbackLock lock];
            //NSLog(@"LocalPlayback....1帧");
            if(nRet > 0)
            {
                [m_playbackDelegate didLocalMP4FilePlayerReceivedPlaybackYUVData:outYUV420 length:(int)in_outLen width:nWidth height:nHeigh timestamp:nTimestamp];
            }
            
            [m_playbackLock unlock];
            
        }
        else if (nAVCodecID == RTSP_AV_CODECID_AUDIO_ADPCM) {
            //no support adpcm format audio
            if (false)
            {
                if (m_pAudioPlayer && m_bAudioRunning)
                {
                    nOutLen = 4*Length;
                    unsigned char *pOutBuf = new unsigned char[nOutLen];
                    pHandle = m_pAudioHandleAdpcm;
                    if(pHandle && pOutBuf)
                    {
                        
                        int nRet = SEAudio_Decode(pHandle, inBuf,  inLen,  (UCHAR*)pOutBuf, &nOutLen);
                        //NSLog(@"AV_CODECID_AUDIO_AAC SEAudio_Decode()22 ret=%d inlen=%d,outlen=%d",nRet,inLen,nOutLen);
                        if(nRet > 0 && nOutLen > 0)
                        {
                            NSData *data = [[NSData alloc] initWithBytes:pOutBuf length:nOutLen];
                            [m_pAudioPlayer openAudioFromQueue:data];
                            [data release];
                        }
                    }
                    delete [] pOutBuf;
                    pOutBuf = NULL;
                }
            }
            
        }
        else if (nAVCodecID == RTSP_AV_CODECID_AUDIO_G726) {
            if (m_pAudioPlayer && m_bAudioRunning)
            {
                nOutLen = 8*Length;
                unsigned char *pOutBuf = new unsigned char[nOutLen];
                pHandle = m_pAudioHandleG726;
                if(pHandle && pOutBuf)
                {
                    
                    int nRet = SEAudio_Decode(pHandle, inBuf,  inLen,  (UCHAR*)pOutBuf, &nOutLen);
                    //NSLog(@"AV_CODECID_AUDIO_AAC SEAudio_Decode()22 ret=%d inlen=%d,outlen=%d",nRet,inLen,nOutLen);
                    if(nRet > 0 && nOutLen > 0)
                    {
                        NSData *data = [[NSData alloc] initWithBytes:pOutBuf length:nOutLen];
                        [m_pAudioPlayer openAudioFromQueue:data];
                        [data release];
                    }
                }
                delete [] pOutBuf;
                pOutBuf = NULL;
            }
            
        }
        else if (nAVCodecID == RTSP_AV_CODECID_AUDIO_G711A) {
            
            if (m_pAudioPlayer && m_bAudioRunning)
            {
                nOutLen = 2*Length;
                unsigned char *pOutBuf = new unsigned char[nOutLen];
                pHandle = m_pAudioHandleG711;
                if(pHandle && pOutBuf)
                {
                    
                    int nRet = SEAudio_Decode(pHandle, inBuf,  inLen,  (UCHAR*)pOutBuf, &nOutLen);
                    //NSLog(@"AV_CODECID_AUDIO_AAC SEAudio_Decode()22 ret=%d inlen=%d,outlen=%d",nRet,inLen,nOutLen);
                    if(nRet > 0 && nOutLen > 0)
                    {
                        NSData *data = [[NSData alloc] initWithBytes:pOutBuf length:nOutLen];
                        [m_pAudioPlayer openAudioFromQueue:data];
                        [data release];
                        //[self addOneAudioFrameIntoLocalRecordFile:(char*)pOutBuf dataSize:nOutLen deviceTimeTamp:nTimestamp];
                    }
                }
                delete [] pOutBuf;
                pOutBuf = NULL;
            }
        }
        else if (nAVCodecID == RTSP_AV_CODECID_AUDIO_AAC) {
            if (m_pAudioPlayer && m_bAudioRunning)
            {
                nOutLen = 100*Length;
                if (nOutLen < KYL_AUDIO_DECODE_BUFFER_SIZE+1) {
                    nOutLen = KYL_AUDIO_DECODE_BUFFER_SIZE+1;
                }
                unsigned char *pOutBuf = new unsigned char[nOutLen];
                pHandle = m_pAudioHandleAAC;
                
                if(pHandle && pOutBuf)
                {
                    
                    int nRet = SEAudio_Decode(pHandle, inBuf,  inLen,  (UCHAR*)pOutBuf, &nOutLen);
                    //NSLog(@"AV_CODECID_AUDIO_AAC SEAudio_Decode()22 ret=%d inlen=%d,outlen=%d",nRet,inLen,nOutLen);
                    if(nRet > 0 && nOutLen > 0)
                    {
                        NSData *data = [[NSData alloc] initWithBytes:pOutBuf length:nOutLen];
                        [m_pAudioPlayer openAudioFromQueue:data];
                        [data release];
                        //[self addOneAudioFrameIntoLocalRecordFile:(char*)pOutBuf dataSize:nOutLen deviceTimeTamp:nTimestamp];
                    }
                }
                delete [] pOutBuf;
                pOutBuf = NULL;
            }
        }
        else if (nAVCodecID == RTSP_AV_CODECID_DATA_ALARM) {
            
        }
        
        if (nAVCodecID == RTSP_AV_CODECID_VIDEO_H264 || nAVCodecID == RTSP_AV_CODECID_VIDEO_MJPEG) {
            //sleep a little time
            if (oldtimestamp == 0) {
                oldtimestamp = nTimestamp;
            }else {
                unsigned int timestamp1 = nTimestamp;
                int timeoff = timestamp1 - oldtimestamp;
                if (timeoff > 20000 || timeoff <= 0) {
                    timeoff = 10;
                }
                //NSLog(@"timeoff=%d",timeoff);
                oldtimestamp = timestamp1;
                if (!CustomSleep(timeoff)) {
                    [pool drain];
                    if (pBuf) {
                        free(pBuf);
                        pBuf = NULL;
                    }
                    if (outYUV420) {
                        free(outYUV420);
                        outYUV420 = NULL;
                    }
                    
                    
                    if(m_pVideoHandleH264) {
                        SEVideo_Destroy(&m_pVideoHandleH264);
                        m_pVideoHandleH264=NULL;
                    }
                    
                    if(m_pAudioHandleG726) {
                        SEAudio_Destroy(&m_pAudioHandleG726);
                        m_pAudioHandleG726=NULL;
                    }
                    
                    if (m_pAudioHandleG711) {
                        SEAudio_Destroy(&m_pAudioHandleG711);
                        m_pAudioHandleG711 = NULL;
                    }
                    if(m_pAudioHandleAdpcm){
                        SEAudio_Destroy(&m_pAudioHandleAdpcm);
                        m_pAudioHandleAdpcm=NULL;
                    }
                    //{{--kongyulu at 20141118
                    if(m_pAudioHandleAAC)
                    {
                        SEAudio_Destroy(&m_pAudioHandleAAC);
                        m_pAudioHandleAAC = NULL;
                    }
                    if (m_pAudioPlayer != nil)
                    {
                        [m_pAudioPlayer stopSound];
                        [m_pAudioPlayer cleanUpOpenAL];
                        [m_pAudioPlayer release];
                        m_pAudioPlayer = nil;
                    }
                    return;
                }
            }
            
        }
        [pool drain];
    }
    
    if (pBuf) {
        free(pBuf);
        pBuf = NULL;
    }
    if (outYUV420) {
        free(outYUV420);
        outYUV420 = NULL;
    }
    
    
    if(m_pVideoHandleH264) {
        SEVideo_Destroy(&m_pVideoHandleH264);
        m_pVideoHandleH264=NULL;
    }
    
    if(m_pAudioHandleG726) {
        SEAudio_Destroy(&m_pAudioHandleG726);
        m_pAudioHandleG726=NULL;
    }
    
    if (m_pAudioHandleG711) {
        SEAudio_Destroy(&m_pAudioHandleG711);
        m_pAudioHandleG711 = NULL;
    }
    if(m_pAudioHandleAdpcm){
        SEAudio_Destroy(&m_pAudioHandleAdpcm);
        m_pAudioHandleAdpcm=NULL;
    }
    //{{--kongyulu at 20141118
    if(m_pAudioHandleAAC)
    {
        SEAudio_Destroy(&m_pAudioHandleAAC);
        m_pAudioHandleAAC = NULL;
    }
    if (m_pAudioPlayer != nil)
    {
        [m_pAudioPlayer stopSound];
        [m_pAudioPlayer cleanUpOpenAL];
        [m_pAudioPlayer release];
        m_pAudioPlayer = nil;
    }
    
}

void KYLLocalMP4FilePlayer::Pause(BOOL bPause)
{
    m_bPause = bPause;
}

BOOL KYLLocalMP4FilePlayer::SeekToPos(int nSecond)
{
    if (NULL == m_pMp4videoRecordToolHandle) {
        return false;
    }
    int nRet = SEMP4Read_SetBeginPos(m_pMp4videoRecordToolHandle,nSecond);
    return nRet;
}

FILE_INFO * KYLLocalMP4FilePlayer::GetTheFileInfo(char *szFilePath)
{
    int nRet = 0;
    if(NULL == m_pMp4videoRecordToolHandle) return false;
    
    //FILE_INFO m_stFileInfo;
    memset(&m_stFileInfo, 0, sizeof(FILE_INFO));
    nRet = SEMP4Read_OpenMp4File(m_pMp4videoRecordToolHandle, szFilePath,&m_stFileInfo);
    if (nRet < 0) {
        if (nRet  == -1) {
            printf("open local mp4 file failed -1: pFilename==NULL");
        }
        else if (nRet  == -1) {
            printf("open local mp4 file failed -2: open file fails");
        }
        else if (nRet  == -1) {
            printf("open local mp4 file -3: there isn't audio and video track");
        }
        SEMP4Read_CloseMp4File(m_pMp4videoRecordToolHandle);
        return NULL;
    }
    else
    {
        //打开文件成功
//        RTSP_ENUM_AV_CODECID eVideoAVCodecID = m_stFileInfo.eVideoAVCodecID;
//        INT32   nVideoWidth = m_stFileInfo.nVideoWidth;
//        INT32   nVideoHeight = m_stFileInfo.nVideoHeight;
//        
//        RTSP_ENUM_AV_CODECID eAudioAVCodecID = m_stFileInfo.eAudioAVCodecID;
//        INT32   nAudioSamplerate = m_stFileInfo.nAudioSamplerate;
//        INT32   nAudioDatabit = m_stFileInfo.nAudioDatabit;
//        INT32   nAudioChannels = m_stFileInfo.nAudioChannels;
//        
//        INT32   nDurationInSecond = m_stFileInfo.nDurationInSecond;
//        INT64   nFileSizeInByte = m_stFileInfo.nFileSizeInByte;
        
//        printf("open local mp4 file ok , eVideoAVCodecID=%d,nVideoWidth=%d,nVideoHeight=%d,eAudioAVCodecID=%d,nAudioSamplerate=%d,nAudioDatabit=%d, nAudioChannels=%d,nDurationInSecond=%d, nFileSizeInByte=%d,",eVideoAVCodecID,nVideoWidth,nVideoHeight,eAudioAVCodecID,nAudioSamplerate,nAudioDatabit,nAudioChannels,nDurationInSecond,nFileSizeInByte);
    }
    SEMP4Read_CloseMp4File(m_pMp4videoRecordToolHandle);
    return &m_stFileInfo;
}

BOOL KYLLocalMP4FilePlayer::StartAudio(BOOL bOpen)
{
    if (bOpen) {
        m_bAudioRunning = 1;
    }
    else{
        m_bAudioRunning = 0;
    }
    return true;
}

BOOL KYLLocalMP4FilePlayer::StartPlayback(char *szFilePath)
{
    int nRet = 0;
    if(NULL == m_pMp4videoRecordToolHandle) return false;
    
    //FILE_INFO m_stFileInfo;
    memset(&m_stFileInfo, 0, sizeof(FILE_INFO));
    nRet = SEMP4Read_OpenMp4File(m_pMp4videoRecordToolHandle, szFilePath,&m_stFileInfo);
    if (nRet < 0) {
        if (nRet  == -1) {
            printf("open local mp4 file failed -1: pFilename==NULL");
        }
        else if (nRet  == -1) {
            printf("open local mp4 file failed -2: open file fails");
        }
        else if (nRet  == -1) {
            printf("open local mp4 file -3: there isn't audio and video track");
        }
        return NO;
    }
    else
    {
        //打开文件成功
        RTSP_ENUM_AV_CODECID eVideoAVCodecID = m_stFileInfo.eVideoAVCodecID;
        INT32   nVideoWidth = m_stFileInfo.nVideoWidth;
        INT32   nVideoHeight = m_stFileInfo.nVideoHeight;
        
        RTSP_ENUM_AV_CODECID eAudioAVCodecID = m_stFileInfo.eAudioAVCodecID;
        INT32   nAudioSamplerate = m_stFileInfo.nAudioSamplerate;
        INT32   nAudioDatabit = m_stFileInfo.nAudioDatabit;
        INT32   nAudioChannels = m_stFileInfo.nAudioChannels;
        
        INT32   nDurationInSecond = m_stFileInfo.nDurationInSecond;
        INT64   nFileSizeInByte = m_stFileInfo.nFileSizeInByte;
        
        if (m_playbackDelegate && [m_playbackDelegate respondsToSelector:@selector(didLocalMP4FilePlayerGetTotalTimeLength:)]) {
            [m_playbackDelegate didLocalMP4FilePlayerGetTotalTimeLength:nDurationInSecond];
        }
        
        printf("open local mp4 file ok , eVideoAVCodecID=%d,nVideoWidth=%d,nVideoHeight=%d,eAudioAVCodecID=%d,nAudioSamplerate=%d,nAudioDatabit=%d, nAudioChannels=%d,nDurationInSecond=%d, nFileSizeInByte=%d,",eVideoAVCodecID,nVideoWidth,nVideoHeight,eAudioAVCodecID,nAudioSamplerate,nAudioDatabit,nAudioChannels,nDurationInSecond,(int)nFileSizeInByte);
    }
    
    m_bPlaybackThreadRuning = 1;
    isPlayOver=NO;
    pthread_create(&m_PlaybackThreadID, NULL, PlaybackThread, this);
    //pthread_create(&m_UpdateTimeThreadID, NULL, UpdateTimeThread, this);
    
    //默认开启音频
    StartAudio(true);
    
    return YES;
}

void KYLLocalMP4FilePlayer::SetPlaybackDelegate(id<KYLLocalMP4FilePlayerProtocol> playbackDelegate)
{
    [m_playbackLock lock];
    m_playbackDelegate = playbackDelegate;
    [m_playbackLock unlock];
}

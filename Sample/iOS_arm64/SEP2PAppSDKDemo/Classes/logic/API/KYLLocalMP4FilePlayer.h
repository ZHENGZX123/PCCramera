//
//  KYLLocalMP4FilePlayer.h
//  P2PCamera
//
//
//

#ifndef __P2PCamera__KYLLocalMP4FilePlayer__
#define __P2PCamera__KYLLocalMP4FilePlayer__

#include <stdio.h>

#import "KYLLocalMP4FilePlayerProtocol.h"

#import "SE_VideoCodec.h"
#import "KYLDefine.h"
#import "mp4.h"
#import "KYLOpenALPlayer.h"


class KYLLocalMP4FilePlayer
{
public:
    KYLLocalMP4FilePlayer();
    ~KYLLocalMP4FilePlayer();
    
    void SetPlaybackDelegate(id<KYLLocalMP4FilePlayerProtocol> playbackDelegate);
    BOOL StartPlayback(char *szFilePath);
    BOOL SeekToPos(int nSecond);
    void Pause(BOOL bPause);
    FILE_INFO *GetFileInfo(){ return &m_stFileInfo;}
    FILE_INFO *GetTheFileInfo(char *szFilePath);
    BOOL StartAudio(BOOL bOpen);
    
protected:
    static void* PlaybackThread(void* param);
    void PlaybackProcess();
    
    void StopPlayback();
    BOOL CustomSleep(int uNum);
    BOOL isPlayOver;
    //static void* UpdateTimeThread(void* param);
    //void updateTimeProcess();
private:
    pthread_t m_UpdateTimeThreadID;
    pthread_t m_PlaybackThreadID;
    int m_bPlaybackThreadRuning;
    
    id<KYLLocalMP4FilePlayerProtocol> m_playbackDelegate;
    unsigned int m_nTotalTime;
    unsigned int m_nTotalKeyFrame;
    UCHAR *m_pMp4videoRecordToolHandle;
    
    BOOL m_bPause;
    
    NSCondition *m_playbackLock;
    int sumTime;
    FILE_INFO m_stFileInfo;
    //UCHAR *m_pVideoHandleH264;
    KYLOpenALPlayer *m_pAudioPlayer;
    int m_bAudioRunning;
    
    
};

#endif /* defined(__P2PCamera__KYLLocalMP4FilePlayer__) */

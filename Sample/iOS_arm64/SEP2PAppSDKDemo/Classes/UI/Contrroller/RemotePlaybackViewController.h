//
//  PlaybackViewController.h
//  P2PCamera
//
//  Created by mac on 12-11-20.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "KYLCamera.h"

@protocol RemotePlaybackViewControllerProtocol<NSObject>

@optional
-  (void) didTheRmotePlaybackFinished:(NSString *) did currentIndex:(NSInteger) nIndex;

@end

@interface RemotePlaybackViewController : UIViewController<UINavigationBarDelegate, KYLRemoteRecordPlayImageProtocol, KYLDeviceStatusChangedProtocol>{
    IBOutlet UIImageView *imageView;
    IBOutlet UINavigationBar *navigationBar;
    IBOutlet UIActivityIndicatorView *progressView;
    IBOutlet UILabel *LblProgress;
    NSString *strDID;
    UIView *bottomView;
    UIButton *playButton;
     UIButton *btnPre;
     UIButton *btnNext;
    UIButton *btnViewCenterPlay;
    UILabel *startLabel;
    UILabel *endLabel;
    UISlider *slider;
    UIImage *imagePlayNormal;
    UIImage *imagePlayPressed;
    UIImage *imagePauseNormal;
    UIImage *imagePausePressed;
    int m_nTotalTime;
    //BOOL m_bPlayPause;
    BOOL m_bCenterPlayBtnStatus;
    BOOL m_bHideToolBar;
    NSString *strDate;
    
    int m_nScreenHeight;
    int m_nScreenWidth;
    NSCondition *m_playbackstoplock;
    NSString *m_strFileName;
    NSString *m_strName;
    int m_nTimeStamp;
    NSInteger m_nCurrentTimeStampSec;
    UILabel *timestampLabel;
    
    KYLRecordFileInfo *m_pRecordFileInfo;
    KYLCamera *m_pCameraObj;
    NSMutableArray *m_AllRecordFileList;
    NSInteger m_nCurrentSelectedVideoListIndex;
    NSString *m_strRecordFilePath;
    int m_nRecordFileTotalTimeLength;
    int m_nRecordFileCurrentPlayTimePos;
    int m_nPlaybackBeginPosSeconds;
    int m_nPlaybackPlaybackID;
    int m_nPlaybackTimeLen;
    UILabel * m_pCustomToastLabel;
    UINavigationItem *btnBarTopTitleItem;
    
    id<RemotePlaybackViewControllerProtocol> m_delegate;
    
    BOOL m_bPlaying;
    BOOL m_bIsPlayAfterPause;
    BOOL m_bIsReplayAfterFinished;
}

@property (nonatomic, retain) UIActivityIndicatorView *progressView;
@property (nonatomic, retain) UILabel *LblProgress;
@property (nonatomic, retain) UINavigationBar *navigationBar;
@property (nonatomic, retain) UIImageView *imageView;
@property (nonatomic, copy) NSString *strDID;
@property (nonatomic, retain) NSString *m_strFileName;
@property (nonatomic, copy) NSString *m_strName;
@property (nonatomic, retain) UILabel *timestampLabel;
@property (nonatomic, retain) KYLRecordFileInfo *m_pRecordFileInfo;
@property (nonatomic, retain) KYLCamera *m_pCameraObj;
@property (nonatomic, copy) NSString *strDate;
@property (nonatomic, retain) UIImage *imagePlayNormal;
@property (nonatomic, retain) UIImage *imagePlayPressed;
@property (nonatomic, retain) UIImage *imagePauseNormal;
@property (nonatomic, retain) UIImage *imagePausePressed;
@property (nonatomic, retain) UIButton *btnPre;
@property (nonatomic, retain) UIButton *btnNext;
@property (nonatomic, retain) UIButton *btnViewCenterPlay;
@property (nonatomic, retain) UILabel * m_pCustomToastLabel;
@property (nonatomic, retain) NSMutableArray *m_AllRecordFileList;
@property (nonatomic, retain)  NSString *m_strRecordFilePath;
@property (nonatomic, retain)  NSString *m_strCurrentRecordFileName;
@property (nonatomic, assign) NSInteger m_nCurrentSelectedVideoListIndex;
@property (nonatomic, assign) int m_nRecordFileTotalTimeLength;
@property (nonatomic, assign) int m_nRecordFileCurrentPlayTimePos;
@property (nonatomic, retain) UINavigationItem *btnBarTopTitleItem;
@property (nonatomic, assign)  id<RemotePlaybackViewControllerProtocol> m_delegate;


- (void) StopPlayback;


@end

//
//  PlaybackViewController.m
//  P2PCamera
//
//  Created by mac on 12-11-20.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "RemotePlaybackViewController.h"
#import "APICommon.h"


@interface RemotePlaybackViewController ()

@end

@implementation RemotePlaybackViewController

@synthesize navigationBar;
@synthesize imageView;
@synthesize strDID;
@synthesize m_strFileName;
@synthesize m_strName;
@synthesize progressView;
@synthesize LblProgress;
@synthesize timestampLabel;
@synthesize m_pCameraObj;
@synthesize m_pRecordFileInfo;
@synthesize strDate;
@synthesize imagePauseNormal;
@synthesize imagePausePressed;
@synthesize imagePlayNormal;
@synthesize imagePlayPressed;
@synthesize btnPre;
@synthesize btnNext;
@synthesize btnViewCenterPlay;
@synthesize m_AllRecordFileList;
@synthesize m_nCurrentSelectedVideoListIndex;
@synthesize m_strRecordFilePath;
@synthesize m_nRecordFileCurrentPlayTimePos;
@synthesize m_nRecordFileTotalTimeLength;
@synthesize m_pCustomToastLabel;
@synthesize m_strCurrentRecordFileName;
@synthesize btnBarTopTitleItem;
@synthesize m_delegate;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
        self.m_pRecordFileInfo = nil;
        self.m_pCameraObj = nil;
        self.m_strRecordFilePath = nil;
        self.m_AllRecordFileList = nil;
        m_nRecordFileTotalTimeLength = 0;
        m_nRecordFileCurrentPlayTimePos = 0;
         m_nPlaybackBeginPosSeconds = 0;
         m_nPlaybackPlaybackID = 0;
         m_nPlaybackTimeLen = 0;
        self.m_pCustomToastLabel = nil;
        m_strCurrentRecordFileName = @"";
        m_bIsReplayAfterFinished = NO;
    }
    return self;
}


- (void) StopPlayback
{
    if (m_delegate && [m_delegate respondsToSelector:@selector(didTheRmotePlaybackFinished:currentIndex:)])
    {
        [self.m_delegate didTheRmotePlaybackFinished:self.m_strRecordFilePath currentIndex:m_nCurrentSelectedVideoListIndex];
    }

    [self stopPlayTheRemoteVideo];
    [self stopPlayThread];
    [m_playbackstoplock lock];
    [self btnBarBackClicked];
    [self.navigationController popViewControllerAnimated:YES];
    [m_playbackstoplock unlock];
}

#pragma mark - camera control actions

- (int) startPlayTheRemoteVideo
{
    int nRet = 0;
    if (self.m_pCameraObj) {
        
        self.m_pCameraObj.m_pRemoteRecordPlaybackImageDelegate = self;
        nRet = [self.m_pCameraObj startPlayRemoteRecordFile:self.m_strRecordFilePath];
        self.m_strCurrentRecordFileName = [self.m_strRecordFilePath lastPathComponent];
    }
    return nRet;
}

- (int) restartPlayTheRemoteVideoWhenPause
{
    int nRet = 0;
    if (self.m_pCameraObj) {
        self.m_pCameraObj.m_pRemoteRecordPlaybackImageDelegate = self;
        nRet = [self.m_pCameraObj startPlayRemoteRecordFile:self.m_strRecordFilePath atPos:(int)m_nCurrentTimeStampSec/1000];
        self.m_strCurrentRecordFileName = [self.m_strRecordFilePath lastPathComponent];
    }
    return nRet;
}

- (int) stopPlayTheRemoteVideo
{
    int nRet = 0;
    if (self.m_pCameraObj) {
        nRet = [self.m_pCameraObj stopPlayRemoteRecordFile:m_strRecordFilePath];
    }
    
    return nRet;
}

- (int) startPlayTheRemoteVideoAtPos:(int) nPosSec
{
    int nRet = 0;
    if (self.m_pCameraObj) {
        NSLog(@"播放的位置为：%d",nPosSec);
        self.m_pCameraObj.m_pRemoteRecordPlaybackImageDelegate = self;
        nRet = [self.m_pCameraObj startPlayRemoteRecordFile:self.m_strRecordFilePath atPos:nPosSec];
        self.m_strCurrentRecordFileName = [self.m_strRecordFilePath lastPathComponent];
    }
    return nRet;
}

- (int) startPlayThread
{
    int nRet = 0;
    if (self.m_pCameraObj) {
        self.m_pCameraObj.m_pRemoteRecordPlaybackImageDelegate = self;
        nRet = [self.m_pCameraObj startPlayTheRemoteRecordFileWhenSucceedReceiveVideoStream];
    }
    
    return nRet;
}

- (int) stopPlayThread
{
    int nRet = 0;
    if (self.m_pCameraObj) {
        nRet = [self.m_pCameraObj stopPlayTheRemoteRecordFileWhenSucceedReceiveVideoStream];
    }
    return nRet;
}

- (void) btnPlayControl: (id) sender
{
    if (nil == self.m_pCameraObj) return;
    
    if ([self.m_pCameraObj getRemoteSDCardVideoStatus])
    {
        [self stopPlayTheRemoteVideo];
        [self stopPlayThread];
        //m_bIsPlayAfterPause = YES;
        [playButton setImage:imagePlayNormal forState:UIControlStateNormal] ;
        [playButton setImage:imagePlayPressed forState:UIControlStateHighlighted];
    }
    else
    {
        [self showTheWaitProgress];
        [self restartPlayTheRemoteVideoWhenPause];
        
        [playButton setImage:imagePauseNormal forState:UIControlStateNormal] ;
        [playButton setImage:imagePausePressed forState:UIControlStateHighlighted];
        m_bIsPlayAfterPause = YES;
    }
    
}

- (void) showTheVideoPlayFinishedStatus
{
    [self stopPlayThread];
    //[mytoast showWithText:NSLocalizedString(@"Video Finished", nil) ];
    [self showWithText:NSLocalizedString(@"Video Finished!", nil) superView:self.view bLandScap:YES];
    

    [btnViewCenterPlay setImage:imagePlayNormal forState:UIControlStateNormal] ;
    [btnViewCenterPlay setImage:imagePlayPressed forState:UIControlStateHighlighted];
    [btnViewCenterPlay setHidden:NO];
    [playButton setImage:imagePlayNormal forState:UIControlStateNormal] ;
    [playButton setImage:imagePlayPressed forState:UIControlStateHighlighted];
    
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    m_bPlaying = NO;
    self.m_pCustomToastLabel = nil;
    self.btnNext = nil;
    self.btnPre = nil;
    self.btnViewCenterPlay = nil;
    
    m_bCenterPlayBtnStatus = NO;
    m_bHideToolBar = NO;
    
    m_nTimeStamp = 0;
    m_nCurrentTimeStampSec =0;
    m_bIsPlayAfterPause = NO;
    self.imageView.backgroundColor = [UIColor blackColor];
    m_playbackstoplock = [[NSCondition alloc] init];
    CGRect getFrame = [[UIScreen mainScreen]applicationFrame];
    m_nScreenHeight = getFrame.size.width;
    m_nScreenWidth = getFrame.size.height;
    navigationBar.barStyle = UIBarStyleBlackTranslucent;
    navigationBar.delegate = self;

    self.navigationBar.alpha = 0.6f;
    UINavigationItem *back = [[UINavigationItem alloc] initWithTitle:NSLocalizedString(@"Back", nil)];
    UINavigationItem *item = [[UINavigationItem alloc] initWithTitle:m_strFileName];
    self.btnBarTopTitleItem = item;
    
    NSArray *array = [NSArray arrayWithObjects:back, item, nil];
    [navigationBar setItems:array];
    float version = [[[UIDevice currentDevice] systemVersion] floatValue];
    if (version >= 7.0)
    {
        self.navigationBar.tintColor = [UIColor colorWithRed:KYL_BTN_NORMAL_RED_IOS7/255.0f green:KYL_BTN_NORMAL_GREEN_IOS7/255.0f blue:KYL_BTN_NORMAL_BLUE_IOS7/255.0f alpha:1];
        NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:
                                    [UIColor colorWithRed:KYL_BTN_NORMAL_RED_IOS7/255.0f green:KYL_BTN_NORMAL_GREEN_IOS7/255.0f blue:KYL_BTN_NORMAL_BLUE_IOS7/255.0f alpha:1],
                                    NSForegroundColorAttributeName, nil];
        [self.navigationBar setTitleTextAttributes:attributes];
    }
    else
    {
        self.navigationBar.tintColor = [UIColor colorWithRed:KYL_BTN_NORMAL_RED/255.0f green:KYL_BTN_NORMAL_GREEN/255.0f blue:KYL_BTN_NORMAL_BLUE/255.0f alpha:1];
    }

    [item release];
    [back release];
    imageView.userInteractionEnabled = YES;
    UITapGestureRecognizer *imageGR=[[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(imageTouched:)];
    [imageGR setNumberOfTapsRequired:1];
    [imageView addGestureRecognizer:imageGR];
    [imageGR release];
    //load image
    self.imagePlayNormal = [UIImage imageNamed:@"video_play_pause_normal.png"];
    self.imagePlayPressed = [UIImage imageNamed:@"video_play_pause_pressed.png"] ;
    self.imagePauseNormal = [UIImage imageNamed:@"video_play_play_normal.png"];
    self.imagePausePressed = [UIImage imageNamed:@"video_play_play_pressed.png"];
    
    //-------------bottomView--------------------------------------
    CGRect screenRect = [[UIScreen mainScreen] applicationFrame] ;
    float bottomViewHeight = 80 ;
    float bottomViewX = 0;
    float bottomViewY = screenRect.size.width - bottomViewHeight ;
    float bottomViewWidth = screenRect.size.height;
    bottomView = [[UIView alloc] initWithFrame:CGRectMake(bottomViewX, bottomViewY, bottomViewWidth, bottomViewHeight)];
    bottomView.backgroundColor = [UIColor colorWithRed:80/255.0f green:80/255.0f blue:80/255.0f alpha:1.0f];
    bottomView.alpha = 0.6f ;
    float sliderX = 10 ;
    float sliderY = 5 ;
    float sliderWidth = bottomView.frame.size.width - 2 * sliderX ;
    float sliderHeight = 30 ;
    slider = [[UISlider alloc] initWithFrame:CGRectMake(sliderX, sliderY, sliderWidth, sliderHeight)];
    slider.userInteractionEnabled = YES;
    //{{--- kongyulu at 20140812
    [slider addTarget:self action:@selector(didTheSliderValueChanged:) forControlEvents:UIControlEventValueChanged];
    [slider addTarget:self action:@selector(didTheSliderTouchDown:) forControlEvents:UIControlEventTouchDown];
    [slider addTarget:self action:@selector(didTheSliderTouchUp:) forControlEvents:UIControlEventTouchUpInside];
    //}}--  kongyulu at 20140812
    [bottomView addSubview:slider] ;
    
    float startLabelX = 10 ;
    float startLabelY = sliderY + sliderHeight + 5 ;
    float startLabelWidth = 100;
    float startLabelHeight = 30 ;
    startLabel = [[UILabel alloc] initWithFrame:CGRectMake(startLabelX, startLabelY, startLabelWidth, startLabelHeight)] ;
    startLabel.text = @"";
    startLabel.textAlignment = NSTextAlignmentLeft ;
    startLabel.backgroundColor = [UIColor clearColor];
    startLabel.textColor = [UIColor whiteColor];
    [bottomView addSubview:startLabel];
    
    float endLabelWidth = 100;
    float endLabelX = bottomView.frame.size.width - 10 - 100 ;
    float endLabelY = sliderY + sliderHeight + 5 ;
    float endLabelHeight = 30 ;
    endLabel = [[UILabel alloc] initWithFrame:CGRectMake(endLabelX, endLabelY, endLabelWidth, endLabelHeight)] ;
    endLabel.text = @"";
    endLabel.textAlignment = NSTextAlignmentRight;
    endLabel.textColor = [UIColor whiteColor];
    endLabel.backgroundColor = [UIColor clearColor];
    [bottomView addSubview:endLabel];
    
    float centerX = bottomView.frame.size.width / 2;
    float centerY = self.view.frame.size.height / 2;
    float playButtonX = centerX - 20 ;
    float playButtonY = sliderY + sliderHeight;
    float playButtonWidth = 40;
    float playButtonHeight = 40;
//    CGRect rect = self.view.frame;
//    int nWidthSuper = rect.size.width;
//    int nHeightSuper = rect.size.height;
    self.LblProgress.text = NSLocalizedString(@"Connecting",nil);
    self.LblProgress.textColor = [UIColor whiteColor];
    [self.progressView startAnimating];
    
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone)//iphone
    {
    }
    else//ipad
    {
         centerX = 1024 / 2;
         centerY = 768 / 2;
        self.LblProgress.center = CGPointMake(centerX, centerY);
    }
    playButton = [[UIButton alloc] initWithFrame:CGRectMake(playButtonX, playButtonY, playButtonWidth, playButtonHeight)];
    [playButton addTarget:self action:@selector(btnPlayControl:) forControlEvents:UIControlEventTouchUpInside];
    [playButton setImage:imagePauseNormal forState:UIControlStateNormal] ;
    [playButton setImage:imagePausePressed forState:UIControlStateHighlighted];
    [bottomView addSubview:playButton];
    
    btnPre = [[UIButton alloc] initWithFrame:CGRectMake(playButtonX - playButtonWidth - 5, playButtonY, playButtonWidth, playButtonHeight)];
    [btnPre addTarget:self action:@selector(btnPreClicked:) forControlEvents:UIControlEventTouchUpInside];
    [btnPre setImage:[UIImage imageNamed:@"btn_video_retreat.png"] forState:UIControlStateNormal] ;
    [btnPre setImage:[UIImage imageNamed:@"btn_video_retreat.png"] forState:UIControlStateHighlighted];
    [bottomView addSubview:btnPre];
    
    btnPre = [[UIButton alloc] initWithFrame:CGRectMake(playButtonX+playButtonWidth+5, playButtonY, playButtonWidth, playButtonHeight)];
    [btnPre addTarget:self action:@selector(btnNextClicked:) forControlEvents:UIControlEventTouchUpInside];
    [btnPre setImage:[UIImage imageNamed:@"btn_video_speed.png"] forState:UIControlStateNormal] ;
    [btnPre setImage:[UIImage imageNamed:@"btn_video_speed.png"] forState:UIControlStateHighlighted];
    [bottomView addSubview:btnPre];
    
    [self.view addSubview:bottomView];
    
    //add timestamp label
    timestampLabel = [[UILabel alloc] initWithFrame:CGRectMake(getFrame.size.height - 300, 10, 290, 30)];
    [timestampLabel setTextAlignment:NSTextAlignmentRight];
    [timestampLabel setBackgroundColor:[UIColor clearColor]];
    [timestampLabel setFont:[UIFont boldSystemFontOfSize:20]];
    [timestampLabel setTextColor:[UIColor redColor]];
    [self.view addSubview:timestampLabel];
    [timestampLabel setHidden:YES];

    btnViewCenterPlay = [[UIButton alloc] initWithFrame:CGRectMake(centerX-30, centerY-30, 60, 60)];
    [btnViewCenterPlay addTarget:self action:@selector(btnCenterViewPlayClicked:) forControlEvents:UIControlEventTouchUpInside];
    [btnViewCenterPlay setImage:imagePauseNormal forState:UIControlStateNormal] ;
    [btnViewCenterPlay setImage:imagePausePressed forState:UIControlStateHighlighted];
    [self.view addSubview:btnViewCenterPlay];
    [btnViewCenterPlay setHidden:YES];
    
    [self startPlayTheRemoteVideo];
    [self showTheWaitProgress];

    self.m_pCameraObj.m_pDeviceStatusChangedDelegate = self;
    
    [self registerTheNotifications];
}

- (void) imageTouched: (UITapGestureRecognizer*)sender
{
    m_bHideToolBar = !m_bHideToolBar ;
    [navigationBar setHidden:m_bHideToolBar];
    [bottomView setHidden:m_bHideToolBar];
    [timestampLabel setHidden:!m_bHideToolBar];
}

- (void) didTheSliderValueChanged:(id) sender
{
    float fValue = slider.value;
    NSLog(@"滑动条拖动：%f",fValue);
    
    
}

- (void) didTheSliderTouchDown:(id) sender
{
    //float fValue = slider.value;
    [self stopPlayTheRemoteVideo];
    [self stopPlayThread];
}

- (void) didTheSliderTouchUp:(id) sender
{
    float fValue = slider.value;
    int nTempSec = fValue ;
    NSLog(@"滑动条拖动后手指松开，拖动的位置为：%d 秒",nTempSec);
    [self showTheWaitProgress];
    [self startPlayTheRemoteVideoAtPos:nTempSec];
}

- (void) btnCenterViewPlayClicked:(id) sender
{
    if (m_pCameraObj && ![self.m_pCameraObj getRemoteSDCardVideoStatus])
    {
        [self showTheWaitProgress];
        [self startPlayTheRemoteVideo];
        [btnViewCenterPlay setHidden:YES];
        [playButton setImage:imagePauseNormal forState:UIControlStateNormal] ;
        [playButton setImage:imagePausePressed forState:UIControlStateHighlighted];
    }
}

- (void) showTheWaitProgress
{
    m_bPlaying = NO;
    [self.progressView startAnimating];
    [self.progressView setHidden:NO];
    [self.LblProgress setHidden:NO];
}

- (void) hideTheWaitProgress
{
    [self.progressView stopAnimating];
    [self.progressView setHidden:YES];
    [self.LblProgress setHidden:YES];
}

- (void) btnPreClicked:(id) sender
{
    if (nil == self.m_pCameraObj) {
        return;
    }
    if (m_nCurrentSelectedVideoListIndex >= [self.m_AllRecordFileList count] || m_nCurrentSelectedVideoListIndex <= 0)
    {
        [self showTheCurrentFileIsTheFirstOneInfo];
        return;
    }
    
    if ([self.m_pCameraObj getRemoteSDCardVideoStatus])
    {
        [self stopPlayTheRemoteVideo];
        [self stopPlayThread];
    }
    
    
    self.m_nCurrentSelectedVideoListIndex -= 1;
    KYLRecordFileInfo *oneRecordInfo = [self.m_AllRecordFileList objectAtIndex:m_nCurrentSelectedVideoListIndex];
    self.m_strRecordFilePath = oneRecordInfo.m_sFilePath;
    self.m_strCurrentRecordFileName = [oneRecordInfo.m_sFilePath lastPathComponent];
    NSLog(@"clicked the play pre file,the filepath=%@",self.m_strRecordFilePath);
    self.m_nRecordFileTotalTimeLength = oneRecordInfo.m_nTimeLength_Sec;
    self.m_nRecordFileCurrentPlayTimePos = 0;
    
    [self showTheCurrentPlayingFileInfo];
    [self showTheWaitProgress];
    [self startPlayTheRemoteVideo];
}

- (void) btnNextClicked:(id) sender
{
    if (nil == self.m_pCameraObj) {
        return;
    }
    if (m_nCurrentSelectedVideoListIndex+1 >= [self.m_AllRecordFileList count])
    {
        [self showTheCurrentFileIsTheLastOneInfo];
        return;
    }
    if ([self.m_pCameraObj getRemoteSDCardVideoStatus])
    {
        [self stopPlayTheRemoteVideo];
        [self stopPlayThread];
    }
    m_nCurrentSelectedVideoListIndex = m_nCurrentSelectedVideoListIndex+1;
    KYLRecordFileInfo *oneRecordInfo = [self.m_AllRecordFileList objectAtIndex:m_nCurrentSelectedVideoListIndex];
    self.m_strRecordFilePath = oneRecordInfo.m_sFilePath;
    self.m_strCurrentRecordFileName = [oneRecordInfo.m_sFilePath lastPathComponent];
    NSLog(@"clicked the play next file,the filepath=%@",self.m_strRecordFilePath);
    self.m_nRecordFileTotalTimeLength = oneRecordInfo.m_nTimeLength_Sec;
    self.m_nRecordFileCurrentPlayTimePos = 0;
    
    [self showTheCurrentPlayingFileInfo];
    [self showTheWaitProgress];
    [self startPlayTheRemoteVideo];
}

- (void) replayTheCurrentFile
{
    if (nil == self.m_pCameraObj) {
        return;
    }
    NSInteger nTempIndex = self.m_nCurrentSelectedVideoListIndex;
    if (nTempIndex >= [self.m_AllRecordFileList count] || nTempIndex < 0)
    {
        return;
    }
    if ([self.m_pCameraObj getRemoteSDCardVideoStatus])
    {
        [self stopPlayTheRemoteVideo];
        [self stopPlayThread];
    }
    
    [self showTheWaitProgress];
    KYLRecordFileInfo *oneRecordInfo = [self.m_AllRecordFileList objectAtIndex:nTempIndex];
    self.m_strRecordFilePath = oneRecordInfo.m_sFilePath;
    self.m_strCurrentRecordFileName = [oneRecordInfo.m_sFilePath lastPathComponent];
    NSLog(@"clicked the play next file,the filepath=%@",self.m_strRecordFilePath);
    self.m_nRecordFileTotalTimeLength = oneRecordInfo.m_nTimeLength_Sec;
    self.m_nRecordFileCurrentPlayTimePos = 0;
    
    [self showTheCurrentPlayingFileInfo];
    [self startPlayTheRemoteVideo];
}


- (void) viewWillAppear:(BOOL)animated
{
    float version = [[[UIDevice currentDevice] systemVersion] floatValue];
    if (version >= 6.0) {//ios6
        [self.view setTransform:CGAffineTransformMakeRotation(M_PI / 2)];
        CGRect rectScreen = [[UIScreen mainScreen] applicationFrame];
        self.view.frame = rectScreen;//CGRectMake(0,0,480,320);
    }
    [super viewWillAppear:animated];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
    if (bottomView != nil) {
        [bottomView release];
        bottomView = nil;
    }
    if (playButton != nil) {
        [playButton release];
        playButton = nil;
    }
}



- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationLandscapeRight);
}

- (void) btnBarBackClicked
{
    self.m_pRecordFileInfo = nil;
    self.m_pCameraObj.m_pDeviceStatusChangedDelegate = nil;
    self.m_pCameraObj.m_pRemoteRecordPlaybackImageDelegate = nil;
    self.m_pCameraObj = nil;
}

- (void) dealloc
{
    self.m_delegate = nil;
    [self removeTheNotifications];
    self.m_strCurrentRecordFileName = nil;
    self.m_pCustomToastLabel = nil;
    self.btnViewCenterPlay = nil;
    self.m_strRecordFilePath = nil;
    self.m_AllRecordFileList = nil;
    self.btnNext = nil;
    self.btnPre = nil;
    
    self.m_pRecordFileInfo = nil;
    self.m_pCameraObj.m_pDeviceStatusChangedDelegate = nil;
    self.m_pCameraObj.m_pRemoteRecordPlaybackImageDelegate = nil;
    self.m_pCameraObj = nil;
    
    self.navigationBar = nil;
    self.imageView = nil;
    self.strDID = nil;
    self.m_strFileName = nil;
    self.m_strName = nil;
    self.progressView = nil;
    self.LblProgress = nil;
    self.strDate = nil;
    if (playButton != nil) {
        [playButton release];
        playButton = nil;
    }
    if (startLabel != nil) {
        [startLabel release];
        startLabel = nil;
    }
    if (endLabel != nil) {
        [endLabel release];
        endLabel = nil;
    }
    if (slider != nil) {
        [slider release];
        slider = nil;
    }
    self.imagePlayNormal = nil;
    self.imagePlayPressed = nil;
    self.imagePauseNormal = nil;
    self.imagePausePressed = nil;
    if (bottomView != nil) {
        [bottomView release];
        bottomView = nil;
    }
    
    if (m_playbackstoplock != nil) {
        [m_playbackstoplock release];
        m_playbackstoplock = nil;
    }
    
    self.btnBarTopTitleItem = nil;
    [super dealloc];
}

#pragma mark -
#pragma mark performOnMainThread
- (void) updateTime
{
    NSString *curDateStr = [self secTimeToString:m_nTimeStamp];
    timestampLabel.text = curDateStr;
    [self updateCurrTime];
}

- (void) updateImage: (UIImage*) image
{
    imageView.image = image;
    //[image release];
    [self updateTime];
}

- (void) hideView
{
    [self hideTheWaitProgress];
}

//- (void) showTheProgressView
//{
//    m_bPlaying = NO;
//    [self.progressView setHidden:NO];
//    [self.LblProgress setHidden:NO];
//}

- (void) updateTotalTime
{
    if (m_bIsPlayAfterPause)
    {
        [self hidTheCenterPlayButton];
        int nTempSec = (int)m_nCurrentTimeStampSec/1000;
        [slider setValue:nTempSec animated:YES];
    }
    else
    {
        endLabel.text = [self secTimeToString:m_nRecordFileTotalTimeLength];
        [slider setMinimumValue:0];
        [slider setMaximumValue:m_nRecordFileTotalTimeLength];
        [self hidTheCenterPlayButton];
    }

}

- (void) hideTheHUDWhenReceiveVideoFrame
{
    
}

- (void) updateCurrTime
{
    startLabel.text = [self secTimeToString:m_nTimeStamp];
    [slider setValue:m_nTimeStamp];
}

- (void) showTheCenterPlayButton
{
    [btnViewCenterPlay setHidden:NO];
}

- (void) hidTheCenterPlayButton
{
    [btnViewCenterPlay setHidden:YES];
}

#pragma mark -
#pragma mark other
- (NSString*) secTimeToString: (NSInteger) secTime
{
    int hour = (int)secTime / 60 / 60;
    int minute = (int)(secTime - hour * 3600 ) / 60;
    int sec = (int)(secTime - hour * 3600 - minute * 60) ;
    NSString *strTime = [NSString stringWithFormat:@"%02d:%02d:%02d", hour, minute, sec];
    return strTime;
}



#pragma mark -
#pragma mark navigationBarDelegate

- (BOOL)navigationBar:(UINavigationBar *)navigationBar shouldPopItem:(UINavigationItem *)item
{
    [self StopPlayback];
    return NO;
}


#pragma mark- video stream  delegate


- (void) KYLRemoteRecordPlayImageProtocol_didReceiveTheStartPlayRemoteRecordFile:(NSString *) strDID
                                                                          result:(int) nResult
                                                                        filePath:(NSString *) sFilePath
                                                                      audipParam:(int) nAudioParam
                                                                        beginPos:(int) nBeginPosSec
                                                                      timeLenSec:(int) nTimeLenSec
                                                                        fileSize:(int) nFileSizeKB
                                                                      playbackID:(int) nPlayBackID
                                                                        reserve1:(NSString *) sReserve1
                                                                        reserve2:(NSString *) sReserve2
{
    NSLog(@"远程回放回调：nRet=%d,filePath=%@,audioParam=%d,beginPos=%d,timeLenSec=%d,fileSize=%d,playbackID=%d",nResult,sFilePath,nAudioParam,nBeginPosSec,nTimeLenSec,nFileSizeKB,nPlayBackID);
    m_nPlaybackBeginPosSeconds = nBeginPosSec;
    m_nPlaybackPlaybackID = nPlayBackID;
    m_nPlaybackTimeLen = nTimeLenSec;
    if (nResult == 0) {// start request sdcard record video stream succeed
        [self performSelectorOnMainThread:@selector(startPlayThread) withObject:nil waitUntilDone:NO];
         [self performSelectorOnMainThread:@selector(updateTotalTime) withObject:nil waitUntilDone:NO];
        [self performSelectorOnMainThread:@selector(showTheCurrentPlayingFileInfo) withObject:nil waitUntilDone:NO];
    }
}

- (void) KYLRemoteRecordPlayImageProtocol_didReceiveTheStopPlayRemoteRecordFile:(NSString *) strDID result:(int) nResult filePath:(NSString *) sFilePath reserve1:(NSString *) sReserve1
{
    if (nResult == 0) {
        //[self performSelectorOnMainThread:@selector(stopPlayThread) withObject:nil waitUntilDone:NO];
    }
}

- (void) KYLRemoteRecordPlayImageProtocol_didReceiveRemoteRecordPlaybackMJPEGImageNotify: (UIImage *)image timestamp: (NSInteger)timestamp  timeProgressSec:(int) nTimeProgressSec DID:(NSString *)did user:(void *) pUser
{
    if (m_bPlaying == NO)
    {
        m_bPlaying = YES;
        [self performSelectorOnMainThread:@selector(hideView) withObject:nil waitUntilDone:NO];
    }
    m_nTimeStamp = (int)timestamp/1000;
    m_nCurrentTimeStampSec =timestamp;
    [image retain];
    [self performSelectorOnMainThread:@selector(updateImage:) withObject:image waitUntilDone:NO];
    [image release];

}

- (void) KYLRemoteRecordPlayImageProtocol_didReceiveRemoteRecordPlaybackOneVideoFrameYUVNotify: (char*) yuvdata length:(int)length width: (int) width height:(int)height timestamp:(unsigned int)timestamp  timeProgressSec:(int) nTimeProgressSec DID:(NSString *)did user:(void *) pUser
{
    if (m_bPlaying == NO)
    {
        m_bPlaying = YES;
        [self performSelectorOnMainThread:@selector(hideView) withObject:nil waitUntilDone:NO];
    }
    m_nTimeStamp = (int)timestamp/1000;
    m_nCurrentTimeStampSec =timestamp;


    UIImage *image=[APICommon YUV420ToImage:(Byte*) yuvdata inSize:length width:width height:height];
    [image retain];
    [self performSelectorOnMainThread:@selector(updateImage:) withObject:image waitUntilDone:NO];
    [image release];
    //[self performSelectorOnMainThread:@selector(hideView) withObject:nil waitUntilDone:NO];
}

- (void) KYLRemoteRecordPlayImageProtocol_didReceiveRemoteRecordPlaybackOneVideoFrameRGB24Notify: (char*) rgb24data length:(int)length width: (int) width height:(int)height timestamp:(unsigned int)timestamp  timeProgressSec:(int) nTimeProgressSec DID:(NSString *)did user:(void *) pUser
{
    
}

- (void) KYLRemoteRecordPlayImageProtocol_didReceiveRemoteRecordPlaybackOneH264VideoFrameWithH264Data: (char*) h264Framedata length: (int) length type: (int) type timestamp: (NSInteger) timestamp  timeProgressSec:(int) nTimeProgressSec DID:(NSString *)did user:(void *) pUser
{
    
}

//STOP
- (void) KYLRemoteRecordPlayImageProtocol_didReceiveRemoteRecordPlaybackDataFinishedWithTimestamp: (NSInteger) timestamp   timeProgressSec:(int) nTimeProgressSec DID:(NSString *)did user:(void *) pUser
{
    [self performSelectorOnMainThread:@selector(showTheVideoPlayFinishedStatus) withObject:nil waitUntilDone:NO];
}




#pragma mark show information lable

- (void)__show : (UIView*) aView {
    [UIView beginAnimations:@"show" context:aView];
    [UIView setAnimationDelegate:self];
    [UIView setAnimationDuration:0.2f];
    [UIView setAnimationDidStopSelector:@selector(__animationDidStop:__finished:__context:)];
    aView.alpha = 1.0f;
    [UIView commitAnimations];
}

- (void)__hide : (UIView*) aView {
    [self performSelectorOnMainThread:@selector(__hideThread:) withObject:aView waitUntilDone:NO];
    
}

- (void) hideCustomToast
{
    if (m_pCustomToastLabel) {
        [m_pCustomToastLabel removeFromSuperview];
    }
}

- (void)__hideThread : (UIView*) aView {
    [UIView beginAnimations:@"hide" context:aView];
    [UIView setAnimationDelegate:self];
    [UIView setAnimationDuration:0.8f];
    [UIView setAnimationDidStopSelector:@selector(__animationDidStop:__finished:__context:)];
    aView.alpha = 0.0f;
    [UIView commitAnimations];
}

- (void)__animationDidStop:(NSString *)animationID __finished:(NSNumber *)finished __context:(void *)context {
    UIView *aView = (UIView*)context;
    if ([animationID isEqualToString:@"hide"]) {
        //NSLog(@"hide....");
        [aView removeFromSuperview];
        //[aView release];
    }
    else if ([animationID isEqualToString:@"show"]) {
        // NSLog(@"show...");
        //[NSTimer scheduledTimerWithTimeInterval:1 target:self selector:@selector(__hide:) userInfo:aView repeats:NO];
        [self performSelector:@selector(__hide:) withObject:aView afterDelay:0.5];
    }
}

- (void) showWithText: (NSString*) strText superView: (UIView*) superView bLandScap:(BOOL) bLandScap
{
    float screenWidth, screenHeight;
    if (!bLandScap) {
        screenWidth = [UIScreen mainScreen].bounds.size.width;
        screenHeight = [UIScreen mainScreen].bounds.size.height;
    }else {
        screenWidth = [UIScreen mainScreen].bounds.size.height;
        screenHeight = [UIScreen mainScreen].bounds.size.width;
    }
    
    UILabel * textLabel = [[UILabel alloc] initWithFrame:CGRectMake(0,0,0,0)];
    self.m_pCustomToastLabel = textLabel;
    [textLabel release];
    
    [textLabel setNumberOfLines:0];
    UIFont *font = [UIFont systemFontOfSize:15];
    //CGSize size = CGSizeMake(170,100);
    //textLabel.lineBreakMode = UILineBreakModeWordWrap;
    //CGSize labelsize = [strText sizeWithFont:font constrainedToSize:size lineBreakMode:UILineBreakModeWordWrap];
    CGSize labelsize = [strText sizeWithFont:font];
    
    float width = labelsize.width + 10;
    float height = labelsize.height;
    float x = floor((screenWidth - width) / 2.0f);
    float y = floor(screenHeight - height - 50.0f);
    
    if (!bLandScap) {
        y -= 10.0f;
    }
    
    [textLabel setFrame: CGRectMake(x, y, width, height)];
    textLabel.text = strText;
    textLabel.textAlignment = NSTextAlignmentCenter;
    textLabel.textColor = [UIColor whiteColor];
    textLabel.font = font;
    textLabel.layer.masksToBounds = YES;
    textLabel.layer.cornerRadius = 5.0;
    textLabel.backgroundColor = [UIColor blackColor];
    textLabel.alpha = 1.0f;
    [superView addSubview:textLabel];
    
    [self __show:textLabel];
    
}

- (void) showWithText: (NSString*) strText bLandScap:(BOOL) bLandScap
{
    float screenWidth, screenHeight;
    if (!bLandScap) {
        screenWidth = [UIScreen mainScreen].bounds.size.width;
        screenHeight = [UIScreen mainScreen].bounds.size.height;
    }else {
        screenWidth = [UIScreen mainScreen].bounds.size.height;
        screenHeight = [UIScreen mainScreen].bounds.size.width;
    }
    
    UILabel * textLabel = [[UILabel alloc] initWithFrame:CGRectMake(0,0,0,0)];
    self.m_pCustomToastLabel = textLabel;
    [textLabel release];
    [textLabel setNumberOfLines:0];
    UIFont *font = [UIFont systemFontOfSize:15];
    //CGSize size = CGSizeMake(170,100);
    //textLabel.lineBreakMode = UILineBreakModeWordWrap;
    //CGSize labelsize = [strText sizeWithFont:font constrainedToSize:size lineBreakMode:UILineBreakModeWordWrap];
    CGSize labelsize = [strText sizeWithFont:font];
    
    float width = labelsize.width + 10;
    float height = labelsize.height;
    float x = floor((screenWidth - width) / 2.0f);
    float y = floor(screenHeight - height - 50.0f);
    
    if (!bLandScap) {
        y -= 10.0f;
    }
    
    [textLabel setFrame: CGRectMake(x, y, width, height)];
    textLabel.text = strText;
    textLabel.textAlignment = NSTextAlignmentCenter;
    textLabel.textColor = [UIColor whiteColor];
    textLabel.font = font;
    textLabel.layer.masksToBounds = YES;
    textLabel.layer.cornerRadius = 5.0;
    textLabel.backgroundColor = [UIColor blackColor];
    textLabel.alpha = 1.0f;
    [self.view addSubview:textLabel];
    
    [self __show:textLabel];
    
    
}

- (void) showTheCurrentFileIsTheFirstOneInfo
{
    [self showWithText:NSLocalizedString(@"Current record is the first file, please try play the next one!", nil)
             superView:self.view
             bLandScap:YES];
}

- (void) showTheCurrentFileIsTheLastOneInfo
{
    [self showWithText:NSLocalizedString(@"Current record is the last file, please try play the previous one!", nil)
             superView:self.view
             bLandScap:YES];
}



- (void) showTheDeviceDisconnectInfo
{
    [self showWithText:NSLocalizedString(@"Device Disconnected! Please check the network!", nil)
             superView:self.view
             bLandScap:YES];
}

- (void) showTheDeviceReconnectInfo
{
    [self showWithText:NSLocalizedString(@"Device Connect succeed!", nil)
             superView:self.view
             bLandScap:YES];
}

- (void) showTheCurrentPlayingFileInfo
{
    
    [self.btnBarTopTitleItem setTitle:m_strCurrentRecordFileName];
    NSString *strTempFileName = m_strCurrentRecordFileName;
    NSString *strMsg = [NSString stringWithFormat:@"%@:%@",strTempFileName,NSLocalizedString(@"playing", nil)];
    [self showWithText:strMsg
             superView:self.view
             bLandScap:YES];
}

- (void) showThePlayingFinishedInfo
{
    NSString *strTempFileName = m_strCurrentRecordFileName;
    NSString *strMsg = [NSString stringWithFormat:@"%@:%@",strTempFileName,NSLocalizedString(@"play finished", nil)];
    [self showWithText:strMsg
             superView:self.view
             bLandScap:YES];
   
}

//- (void) showTheHUD
//{
//    MBProgressHUD *hud = [MBProgressHUD showHUDAddedTo:self.view animated:YES];
//    hud.labelText =  NSLocalizedString(@"", @"");
//    hud.detailsLabelText =  NSLocalizedString(@"Loading...", @"");
//    hud.minSize = CGSizeMake(120, 120);
//    hud.backgroundColor = [UIColor clearColor];
//    hud.opacity = 0.2;
//    hud.opaque = YES;
//    
//}
//
//
//- (void) hideTheHUD
//{
//    [MBProgressHUD hideHUDForView:self.view animated:YES];
//}




#pragma mark - device status  changed delegate

-  (void) didReceiveCameraStatus_KYLDeviceStatusChangedProtocol:(NSString *) did status:(int) _status reserve:(NSString *) reserve1 user:(void *) pUser
{
    @synchronized(self)
    {
        if (![did isEqualToString:self.m_pCameraObj.m_sDID]) {
            return;
        }
        int nTempStatus = _status;
        switch (nTempStatus) {
            case CONNECT_STATUS_CONNECTING:
            {
            }
                break;
            case CONNECT_STATUS_INITIALING:
            {
            }
                break;
            case CONNECT_STATUS_ONLINE:
            {
            }
                break;
            case CONNECT_STATUS_CONNECT_FAILED:
            case CONNECT_STATUS_DISCONNECT:
            case CONNECT_STATUS_INVALID_ID:
            case CONNECT_STATUS_DEVICE_NOT_ONLINE:
            case CONNECT_STATUS_CONNECT_TIMEOUT:
            case CONNECT_STATUS_WRONG_USER_PWD:
            case CONNECT_STATUS_INVALID_REQ:
            case CONNECT_STATUS_EXCEED_MAX_USER:
            {
                [self performSelectorOnMainThread:@selector(showTheDeviceDisconnectInfo) withObject:nil waitUntilDone:NO];
            }
                break;
            case CONNECT_STATUS_CONNECTED:
            {
                [self performSelectorOnMainThread:@selector(showTheDeviceReconnectInfo) withObject:nil waitUntilDone:NO];
            }
                break;
            case CONNECT_STATUS_UNKNOWN:
            {
            }
                break;
                
            default:
                break;
        }
    }
    
}

#pragma mark Notifications

- (void) registerTheNotifications
{
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(didReceiveAppWillInBackgroundNotification:)
                                                 name: UIApplicationDidEnterBackgroundNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(didReceiveAppWillInForgroundNotification:)
                                                 name: UIApplicationWillEnterForegroundNotification object:nil];
}

- (void) removeTheNotifications
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UIApplicationDidEnterBackgroundNotification object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UIApplicationWillEnterForegroundNotification object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}


//进入后台通知
- (void) didReceiveAppWillInBackgroundNotification:(NSNotification *) notification
{
    
//    [self stopPlayTheRemoteVideo];
//    [self stopPlayThread];
//    [playButton setImage:imagePlayNormal forState:UIControlStateNormal] ;
//    [playButton setImage:imagePlayPressed forState:UIControlStateHighlighted];
//    m_bPlayPause = true;
    
    //返回远程录像列表
    [self StopPlayback];

}

//进入前台通知
- (void) didReceiveAppWillInForgroundNotification:(NSNotification *) notification
{
    
    
//    [self startPlayTheRemoteVideo];
//    [playButton setImage:imagePauseNormal forState:UIControlStateNormal] ;
//    [playButton setImage:imagePausePressed forState:UIControlStateHighlighted];
//    m_bPlayPause = false ;
    
}



@end

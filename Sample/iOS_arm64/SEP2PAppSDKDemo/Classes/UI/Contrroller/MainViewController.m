//
//  MainViewController.m
//  LeftRightSlider
//
//  Created by Zhao Yiqi on 13-11-27.
//  Copyright (c) 2013年 Zhao Yiqi. All rights reserved.
//

#import "MainViewController.h"
#import "SliderViewController.h"

#import "TSLocateView.h"
#import "MBProgressHUD.h"
#import "KYLDeviceInfo.h"
#import "SEP2P_Define.h"
#import "KYLWifiObject.h"
#import "KYLAppDelegate.h"
#import "MLTableAlert.h"
#import "RemoteRecordFileListViewController.h"

#define KYL_CAMERA_DID @"KYL_CAMERA_DID"

@interface MainViewController ()<UIGestureRecognizerDelegate, UIAlertViewDelegate>
{
    CGRect oldMonitorRect;
}

@end

@implementation MainViewController
@synthesize m_pscrollView;
@synthesize  btnConnect;
@synthesize  btnStartVideo;
@synthesize  btnStartAudio;
@synthesize  btnStartTalk;
@synthesize  btnSendCommand;
@synthesize btnAddCamera;

@synthesize  textFieldDID;
@synthesize  textFieldAccount;
@synthesize  textFieldPassword;

@synthesize  textViewShowLog;
@synthesize  lableShowStatus;

@synthesize  imgViewShowVideo;
@synthesize m_pCamera;
@synthesize m_listAllSearchDevice;
@synthesize btnDown;
@synthesize btnLeft;
@synthesize btnRight;
@synthesize btnUp;
@synthesize selectedWifiObject;


- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void) clearResource
{
    [self removeTheNotifications];
    self.selectedWifiObject = nil;
    self.listSGPoPSelectData = nil;
    self.listWifiInfo = nil;
    self.popView = nil;
    self.m_listAllSearchDevice = nil;
    self.btnUp = nil;
    self.btnDown = nil;
    self.btnLeft = nil;
    self.btnRight = nil;
    self.btnAddCamera = nil;
    self.m_pscrollView = nil;
    self.btnConnect = nil;
    
    self.btnStartVideo = nil;
    self.btnStartRecord = nil;
    
    self.btnStartAudio = nil;
    
    self.btnStartTalk = nil;
    self.btnSendCommand = nil;
    self.textFieldDID = nil;
    self.textFieldAccount = nil;
    self.textFieldPassword = nil;
    self.textViewShowLog = nil;
    self.lableShowStatus = nil;
    self.imgViewShowVideo = nil;
    [m_listAllCGICommand release];
    m_listAllCGICommand = nil;
    self.m_pCamera = nil;
}

- (void) dealloc
{
    [self clearResource];
    [super dealloc];
}

- (void) setStrDID:(NSString *)strDID
{
    [[NSUserDefaults standardUserDefaults] setObject:strDID forKey:KYL_CAMERA_DID];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (NSString *) strDID
{
    return [[NSUserDefaults standardUserDefaults] valueForKey:KYL_CAMERA_DID];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
    
    [self initTheData];
    [self initTheUI];
    [self registTheNotifications];
    
}

- (void) viewDidUnload
{
    [super viewDidUnload];
    [self clearResource];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}




#pragma mark ui initial

- (void) initTheData
{
    m_nSelectWiflListRow = 0;
    self.selectedWifiObject = nil;
    self.m_pCamera = nil;
    if (nil == m_pCamera) {
        self.m_pCamera = [[[KYLCamera alloc] init] autorelease];
        self.m_pCamera.delegate = self;
    }
    
    self.listSGPoPSelectData = nil;
    self.listWifiInfo = nil;
    //self.popView = [[[SGPopSelectView alloc] init] autorelease];
    
    m_listAllCGICommand = [[NSArray alloc] initWithObjects:@"getCameraParams", @"getCameraWifi",@"setWifiPassword", @"getCameraWifiList", @"getCameraUserInfo", @"getCameraDatetimeInfo", @"getCameraFTPInfo", @"getCameraEmailInfo", @"getCameraAlarmInfo", @"getCameraSDCardScheduleInfo", @"getCameraSoftVersionInfo",@"getRecFileListBy", nil];
    self.listWifiInfo = [[[NSMutableArray alloc] initWithCapacity:20] autorelease];
    self.listSGPoPSelectData = [[[NSMutableArray alloc] initWithCapacity:20] autorelease];
    
}

- (void) initTheUI
{
    [self initTheButtons];
    self.imgViewShowVideo.backgroundColor = [UIColor blackColor];
    
    [self addGesture];
    //self.imgViewShowVideo.image = [UIImage imageNamed:@"video_play1.png"];
    //CGRect rect = self.imgViewShowVideo.frame;
    //self.imgViewShowVideo.frame = CGRectMake(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height + 100);
    self.textViewShowLog.editable = NO;
    self.lableShowStatus.textColor = [UIColor redColor];

    self.textFieldDID.text = self.strDID;
    self.textFieldAccount.text = @"admin";
    self.textFieldPassword.text = @"123456";
    
    [self.btnConnect setTitle:@"Connect" forState:UIControlStateNormal];
    
    [self inittheButtonTitles];
    
    [self enableAllButtons:NO];
    [self initTheCameraMonitor];
    
}

- (void) initTheCameraMonitor
{
    self.pCameraMonitor = [[[KYLCameraMonitor alloc] initWithFrame:imgViewShowVideo.frame] autorelease];
    self.pCameraMonitor.m_pCameraObj = self.m_pCamera;
    oldMonitorRect = self.pCameraMonitor.frame;
    [self.view addSubview:_pCameraMonitor];
    self.pCameraMonitor.delegate = self;
    self.pCameraMonitor.userInteractionEnabled = YES;
    
}

- (void) inittheButtonTitles
{
    
    [self.btnStartVideo setTitle:@"StartVideo" forState:UIControlStateNormal];
    [self.btnStartAudio setTitle:@"StartAudio" forState:UIControlStateNormal];
    [self.btnStartTalk setTitle:@"StartTalk" forState:UIControlStateNormal];
    [self.btnSendCommand setTitle:@"SendCommand" forState:UIControlStateNormal];
}

- (void) addGesture
{
    UITapGestureRecognizer* singleTapRecognizer;
    singleTapRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleSingleTapFrom:)];
    singleTapRecognizer.numberOfTapsRequired = 1; // 单击
    [self.view addGestureRecognizer:singleTapRecognizer];
    
    // 双击的 Recognizer
    UITapGestureRecognizer* doubleTapRecognizer;
    doubleTapRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleDoubleTapFrom:)];
    doubleTapRecognizer.numberOfTapsRequired = 2; // 双击
    [self.view addGestureRecognizer:doubleTapRecognizer];
    
    // 关键在这一行，如果双击确定偵測失败才會触发单击
    [singleTapRecognizer requireGestureRecognizerToFail:doubleTapRecognizer];
    
    // 向上擦碰
    UISwipeGestureRecognizer *oneFingerSwipeUp =
    [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerSwipeUp:)] ;
    [oneFingerSwipeUp setDirection:UISwipeGestureRecognizerDirectionUp];
    [self.view  addGestureRecognizer:oneFingerSwipeUp];
    
    
    // 向下擦碰
    UISwipeGestureRecognizer *oneFingerSwipeDown =
    [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerSwipeDown:)] ;
    [oneFingerSwipeDown setDirection:UISwipeGestureRecognizerDirectionDown];
    [self.view  addGestureRecognizer:oneFingerSwipeDown];
    
    // 向左擦碰
    UISwipeGestureRecognizer *oneFingerSwipeLeft =
    [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerSwipeLeft:)] ;
    [oneFingerSwipeLeft setDirection:UISwipeGestureRecognizerDirectionLeft];
    [self.view  addGestureRecognizer:oneFingerSwipeLeft];
    
    
    // 向右擦碰
    UISwipeGestureRecognizer *oneFingerSwipeRight =
    [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerSwipeRight:)]   ;
    [oneFingerSwipeRight setDirection:UISwipeGestureRecognizerDirectionRight];
    [self.view  addGestureRecognizer:oneFingerSwipeRight];
    
    //长按手势
    UILongPressGestureRecognizer *longPressGesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(longPressGestureDid:)];
    longPressGesture.minimumPressDuration = 2;
    [self.view  addGestureRecognizer:longPressGesture];
    
    
    //free memory
    
    [singleTapRecognizer release];
    [doubleTapRecognizer release];
    [oneFingerSwipeUp release];
    [oneFingerSwipeDown release];
    [oneFingerSwipeLeft release];
    [oneFingerSwipeRight release];
    [longPressGesture release];
}

// 长按手势
- (void)longPressGestureDid:(UILongPressGestureRecognizer*)recognizer {
    // 触发手勢事件后，在这里作些事情
    if (recognizer.state == UIGestureRecognizerStateEnded) {
        
        
    }
    
    
    //[self.popupMenu showInView:self.view atPoint:CGPointMake(100, 50)];
    
}

// 单击
- (void)handleSingleTapFrom:(UITapGestureRecognizer*)recognizer {
    
    if (recognizer.state == UIGestureRecognizerStateEnded) {
        
        [self hideTheKeyboard];
    }
    
}

// 双击
- (void)handleDoubleTapFrom:(UITapGestureRecognizer*)recognizer {
    if (recognizer.state == UIGestureRecognizerStateEnded) {
        
        
    }
    
}

//向上滑动
- (void)oneFingerSwipeUp:(UISwipeGestureRecognizer *)recognizer
{
    if (recognizer.state == UIGestureRecognizerStateEnded) {
        if (m_pCamera) {
            [m_pCamera up];
        }
    }
}

// 向下滑动
- (void)oneFingerSwipeDown:(UISwipeGestureRecognizer *)recognizer
{
    if (recognizer.state == UIGestureRecognizerStateEnded) {
        if (m_pCamera) {
            [m_pCamera down];
        }
    }
}

//向左滑动
- (void)oneFingerSwipeLeft:(UISwipeGestureRecognizer *)recognizer
{
    if (recognizer.state == UIGestureRecognizerStateEnded) {
        if (m_pCamera) {
            [m_pCamera left];
        }
    }
}

// 向右滑动
- (void)oneFingerSwipeRight:(UISwipeGestureRecognizer *)recognizer
{
    if (recognizer.state == UIGestureRecognizerStateEnded) {
        if (m_pCamera) {
            [m_pCamera right];
        }
    }
}

- (void) showTheHUD
{
    MBProgressHUD *hud = [MBProgressHUD showHUDAddedTo:self.imgViewShowVideo animated:YES];
	hud.labelText =  NSLocalizedString(@"loading...", @"");
    //CGRect rect = self.view.frame;
    //hud.minSize = rect.size;
    hud.backgroundColor = [UIColor clearColor];
    hud.opacity = 0.2;
    hud.opaque = NO;
    hud.minSize = CGSizeMake(135.f, 135.f);
}


- (void) hideTheHUD
{
    [MBProgressHUD hideHUDForView:self.imgViewShowVideo animated:YES];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation
{
    return (toInterfaceOrientation == UIInterfaceOrientationPortrait);
}

- (BOOL)shouldAutorotate
{
    return NO;
}

- (NSUInteger)supportedInterfaceOrientations
{
    return UIInterfaceOrientationMaskPortrait;//只支持这一个方向(正常的方向)
}

#pragma mark
#pragma rotate UI change begin
- (void)adjustEditModeToolBarUi {
    UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;
    if (UIInterfaceOrientationLandscapeLeft == orientation||UIInterfaceOrientationLandscapeRight == orientation) {//横屏模式
        [self adjustUIForLandscapeMode];
    }else {//竖屏模式
        [self adjustUIForPortaitMode];
    }
}

- (void) adjustUIForLandscapeMode//横屏模式,UI调整
{
    CGRect bounds = [[UIScreen mainScreen] bounds];
    self.pCameraMonitor.frame = CGRectMake(0, 0, self.view.frame.size.height, self.view.frame.size.width);
}

- (void) adjustUIForPortaitMode//竖屏模式,UI调整
{
    self.pCameraMonitor.frame = self.imgViewShowVideo.frame;
}

#pragma rotate UI change End


#pragma mark action
- (IBAction)btnConnectClicked:(id)sender
{
    [self hideTheKeyboard];
    int nRet = -1;
    if([self.btnConnect.titleLabel.text isEqualToString:@"Connect"])
    {
        NSString *strDID = self.textFieldDID.text;
        NSString *strUserName = self.textFieldAccount.text;
        NSString *strPwd = self.textFieldPassword.text;
        
        if (nil == strDID
            || [strDID length] < 10
            || nil == strUserName
            || [strUserName length] < 1
            || nil == strPwd
            || [strPwd length]< 1
            ) {
            NSLog(@"The userinfo error!");
            UIAlertView *alertView =  [[UIAlertView alloc] initWithTitle:@"Tips" message:@"Please input the valid Information" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:@"Cancel", nil];
            [alertView show];
            [alertView release];
            return;
        }
        
        self.m_pCamera.m_sDID = strDID;
        self.strDID = strDID;
        self.m_pCamera.m_sUsername = strUserName;
        self.m_pCamera.m_sPassword = strPwd;
        
        NSLog(@"begin connecting!");
        int ret = [self.m_pCamera connect];
        if (ret == 0) {
            NSLog(@"connected ok!");
            [self.btnConnect setTitle:@"Disconnect" forState:UIControlStateNormal];
            //[self enableAllButtons:YES];
            [self enableTheStartVideoButton:YES];
            [self enableTheSendCommandButton:YES];
        }
    }else{
        if (self.m_pCamera) {
            [self.m_pCamera stopAudio];
            [self.m_pCamera stopTalk];
            [self.m_pCamera stopVideo];

            nRet = [self.m_pCamera disconnect];
            if (nRet == 0) {
                [self.btnConnect setTitle:@"Connect" forState:UIControlStateNormal];
                [self inittheButtonTitles];
                [self enableAllButtons:NO];
                [self enableTheSendCommandButton:NO];
            }
        }
    }
    
}


- (IBAction)btnStartVideoClicked:(id)sender
{
    if (nil == self.m_pCamera) {
        return;
    }
    int nRet = -1;
    if ([self.btnStartVideo.titleLabel.text isEqualToString:@"StartVideo"])
    {
        if ([self.m_pCamera getCameraStatus] != CONNECT_STATUS_CONNECTED) {
            UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"Tips" message:@"You should make sure connected to device first!" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
            [alertView show];
            [alertView release];
            return;
        }
        
        nRet = [self.pCameraMonitor startVideo];
        if(nRet == 0)
        {
            [self.btnStartVideo setTitle:@"StopVideo" forState:UIControlStateNormal];
            [self enableAllButtons:YES];
        }
    }else{
        nRet = [self.pCameraMonitor stopVideo];
        if(nRet == 0)
        {
            [self.btnStartVideo setTitle:@"StartVideo" forState:UIControlStateNormal];
        }
    }
}

- (IBAction)btnStartRecordClicked:(id)sender
{
    int nRet = -1;
    if ([self.btnStartRecord.titleLabel.text isEqualToString:@"StartRecord"])
    {
        if ([self.m_pCamera getCameraStatus] != CONNECT_STATUS_CONNECTED) {
            UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"Tips" message:@"You should make sure connected to device first!" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
            [alertView show];
            [alertView release];
            return;
        }
        nRet = [self.m_pCamera startLocalRecord];
        if (nRet == 0)
        {
            [self.btnStartRecord setTitle:@"StopRecord" forState:UIControlStateNormal];
        }
    }
    else
    {
        nRet = [self.m_pCamera stopLocalRecord];
        if (nRet == 0)
        {
            [self.btnStartRecord setTitle:@"StartRecord" forState:UIControlStateNormal];
        }
    }
}

- (IBAction)btnStartAudioClicked:(id)sender
{
    if (nil == self.m_pCamera) {
        return;
    }
    int nRet = -1;
    if ([self.btnStartAudio.titleLabel.text isEqualToString:@"StartAudio"])
    {
        if ([self.m_pCamera getCameraStatus] != CONNECT_STATUS_CONNECTED) {
            UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"Tips" message:@"You should make sure connected to device and start video first!" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
            [alertView show];
            [alertView release];
            return;
        }
        
        nRet = [self.m_pCamera startAudio];
        if (nRet == 0)
        {
            [self.btnStartAudio setTitle:@"StopAudio" forState:UIControlStateNormal];
        }
    }
    else
    {
        nRet = [self.m_pCamera stopAudio];
        if (nRet == 1)
        {
            [self.btnStartAudio setTitle:@"StartAudio" forState:UIControlStateNormal];
        }
    }
}


- (IBAction)btnStartTalkClicked:(id)sender
{
    if (nil == self.m_pCamera) {
        return;
    }
    int nRet = -1;
    if ([self.btnStartTalk.titleLabel.text isEqualToString:@"StartTalk"])
    {
        if ([self.m_pCamera getCameraStatus] != CONNECT_STATUS_CONNECTED) {
            UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"Tips" message:@"You should make sure connected to device and start video first!" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
            [alertView show];
            [alertView release];
            return;
        }
        
        nRet = [self.m_pCamera startTalk];
        if (nRet == 0)
        {
            [self.btnStartTalk setTitle:@"StopTalk" forState:UIControlStateNormal];
        }
    }
    else
    {
        nRet = [self.m_pCamera stopTalk];
        if (nRet == 0)
        {
            [self.btnStartTalk setTitle:@"StartTalk" forState:UIControlStateNormal];
        }
    }
}



- (IBAction)btnSendCommandClicked:(id)sender
{
    if ([self.m_pCamera getCameraStatus] != CONNECT_STATUS_CONNECTED) {
        UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"Tips" message:@"You should make sure connected to device first!" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
        [alertView show];
        [alertView release];
        return;
    }
    
    TSLocateView *pickerView = [[TSLocateView alloc] initWithTitle:@"Choose Command" delegate:self] ;
    pickerView.m_listCGICommand = m_listAllCGICommand;
    [pickerView showInView:self.view];
}

- (IBAction)btnAddCameraClicked:(id)sender
{
    
}

- (IBAction)btnUpClicked:(id)sender
{
    if (m_pCamera) {
        [m_pCamera up];
    }
}

- (IBAction)btnDownClicked:(id)sender
{
    if (m_pCamera) {
        [m_pCamera down];
    }
}

- (IBAction)btnLeftClicked:(id)sender
{
    if (m_pCamera) {
        [m_pCamera left];
    }
}

- (IBAction)btnRightClicked:(id)sender
{
    if (m_pCamera) {
        [m_pCamera right];
    }
}

- (void) enableAllButtons:(BOOL) _isEnable
{
    //[self.btnConnect setEnabled:_isEnable];
    [self.btnStartAudio setEnabled:_isEnable];
    [self.btnStartVideo setEnabled:_isEnable];
    [self.btnStartTalk setEnabled:_isEnable];
    [self.btnStartRecord setEnabled:_isEnable];
    [self.btnSendCommand setEnabled:_isEnable];
    [self.btnDown setEnabled:_isEnable];
    [self.btnUp setEnabled:_isEnable];
    [self.btnLeft setEnabled:_isEnable];
    [self.btnRight setEnabled:_isEnable];
    
    if (_isEnable)
    {
        self.btnStartAudio.layer.borderColor=[[UIColor whiteColor] CGColor];
        self.btnStartVideo.layer.borderColor=[[UIColor whiteColor] CGColor];
        self.btnStartTalk.layer.borderColor=[[UIColor whiteColor] CGColor];
        self.btnStartRecord.layer.borderColor=[[UIColor whiteColor] CGColor];
        self.btnSendCommand.layer.borderColor=[[UIColor whiteColor] CGColor];
        self.btnDown.layer.borderColor=[[UIColor whiteColor] CGColor];
        self.btnUp.layer.borderColor=[[UIColor whiteColor] CGColor];
        self.btnLeft.layer.borderColor=[[UIColor whiteColor] CGColor];
        self.btnRight.layer.borderColor=[[UIColor whiteColor] CGColor];
        
        [self.btnStartAudio setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        [self.btnStartVideo setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        [self.btnStartTalk setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        [self.btnStartRecord setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        [self.btnSendCommand setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        [self.btnDown setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        [self.btnUp setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        [self.btnLeft setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
        [self.btnRight setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    }
    else
    {
        self.btnStartAudio.layer.borderColor=[[UIColor grayColor] CGColor];
        self.btnStartVideo.layer.borderColor=[[UIColor grayColor] CGColor];
        self.btnStartTalk.layer.borderColor=[[UIColor grayColor] CGColor];
        self.btnStartRecord.layer.borderColor=[[UIColor grayColor] CGColor];
        self.btnSendCommand.layer.borderColor=[[UIColor grayColor] CGColor];
        self.btnDown.layer.borderColor=[[UIColor grayColor] CGColor];
        self.btnUp.layer.borderColor=[[UIColor grayColor] CGColor];
        self.btnLeft.layer.borderColor=[[UIColor grayColor] CGColor];
        self.btnRight.layer.borderColor=[[UIColor grayColor] CGColor];
        
        [self.btnStartAudio setTitleColor:[UIColor grayColor] forState:UIControlStateNormal];
        [self.btnStartVideo setTitleColor:[UIColor grayColor] forState:UIControlStateNormal];
        [self.btnStartTalk setTitleColor:[UIColor grayColor] forState:UIControlStateNormal];
        [self.btnStartRecord setTitleColor:[UIColor grayColor] forState:UIControlStateNormal];
        [self.btnSendCommand setTitleColor:[UIColor grayColor] forState:UIControlStateNormal];
        [self.btnDown setTitleColor:[UIColor grayColor] forState:UIControlStateNormal];
        [self.btnUp setTitleColor:[UIColor grayColor] forState:UIControlStateNormal];
        [self.btnLeft setTitleColor:[UIColor grayColor] forState:UIControlStateNormal];
        [self.btnRight setTitleColor:[UIColor grayColor] forState:UIControlStateNormal];
    }
}

- (void) enableTheStartVideoButton:(BOOL) _isEnable
{
    [self.btnStartVideo setEnabled:_isEnable];
    if (_isEnable)
    {
        self.btnStartVideo.layer.borderColor=[[UIColor whiteColor] CGColor];
        [self.btnStartVideo setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    }
    else
    {
        self.btnStartVideo.layer.borderColor=[[UIColor grayColor] CGColor];
        [self.btnStartVideo setTitleColor:[UIColor grayColor] forState:UIControlStateNormal];
    }
}



- (void) enableTheSendCommandButton:(BOOL) _isEnable
{
    [self.btnSendCommand setEnabled:_isEnable];
    if (_isEnable)
    {
        self.btnSendCommand.layer.borderColor=[[UIColor whiteColor] CGColor];
        [self.btnSendCommand setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    }
    else
    {
        self.btnSendCommand.layer.borderColor=[[UIColor grayColor] CGColor];
        [self.btnSendCommand setTitleColor:[UIColor grayColor] forState:UIControlStateNormal];
    }
}

#pragma mark logic


- (void) hideTheKeyboard
{
    [self.textFieldAccount resignFirstResponder];
    [self.textFieldDID resignFirstResponder];
    [self.textFieldPassword resignFirstResponder];
}




- (UIImage *) getRGB24FrameToImage:(char *) frameData width:(int) width height:(int) height
{
    CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault;
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CFDataRef data = CFDataCreateWithBytesNoCopy(kCFAllocatorDefault, (const unsigned char *) frameData, width * height * 3,kCFAllocatorNull);
    CGDataProviderRef provider = CGDataProviderCreateWithCFData(data);
    
    CGImageRef imgRef = CGImageCreate(width, height, 8, 24, width * 3, colorSpace, bitmapInfo, provider, NULL, YES, kCGRenderingIntentDefault);
    UIImage *img = [[[UIImage alloc] initWithCGImage:imgRef] autorelease];
    
    CGImageRelease(imgRef);
    CGColorSpaceRelease(colorSpace);
    CGDataProviderRelease(provider);
    CFRelease(data);
    return img;
}

- (void) clearTheText
{
    self.textViewShowLog.text = nil;
}

- (void) doTestSetWifiPassword
{
    [self performSelector:@selector(showTheWifiListPopView) withObject:nil afterDelay:0.3];
}

- (void) showTheWifiListPopView
{
    /*
    self.popView = [[[SGPopSelectView alloc] init] autorelease];
    self.popView.selections = self.listSGPoPSelectData;
    __weak typeof(self) weakSelf = self;
    self.popView.selectedHandle = ^(NSInteger selectedIndex){
        NSLog(@"selected index %ld, content is %@", selectedIndex, weakSelf.listSGPoPSelectData[selectedIndex]);
        m_nSelectWiflListRow = (int)selectedIndex;
        
        [self hideTheWifiListPopView];
        
        self.selectedWifiObject = [self.listWifiInfo objectAtIndex:selectedIndex];
        [self showTheAlertViewForEditPassword];
    };
    //self.popView.selections = self.listSGPoPSelectData;
    //KYLAppDelegate *gAppDelegate = (KYLAppDelegate *)[UIApplication sharedApplication].delegate;
    [self.popView showFromView:self.view atPoint:self.view.center animated:YES];
    // create the alert
    */
}

- (void) hideTheWifiListPopView
{
    [self.popView hide:YES];
}

- (BOOL) showTheAlertViewForEditPassword
{
    // show change password dialog
    NSString *strTitle = NSLocalizedString(@"Tips", nil);
    NSString *str1 = NSLocalizedString(@"WiFi SSID", nil);
    NSString *str2 = NSLocalizedString(@"Please Input Wifi password", nil);
    
    NSString *strMsg = [NSString stringWithFormat:@"%@: %@\n %@:\n",str1,self.selectedWifiObject.sSSID,str2];
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:strTitle message:strMsg delegate:self cancelButtonTitle:NSLocalizedString(@"Cancel", nil) otherButtonTitles:NSLocalizedString(@"OK", nil), nil];
    [alert setAlertViewStyle:UIAlertViewStyleSecureTextInput];
    [alert show];
    alert.tag = 1002;
    alert.delegate = self;
    [alert release];
    return YES;
}





#pragma mark - UIAlertView delegate call back
- (void) alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    NSInteger tag = alertView.tag;
    if (tag == 1002) {//设置用户名密码
        if (1 == buttonIndex) {//点击确定
            UITextField *textFieldPassword = [alertView textFieldAtIndex:0];
            NSString *strPassword = textFieldPassword.text;
            NSLog(@"填写的WiFi密码:%@",strPassword);
            
            //开始设置WiFi密码
        }
        else if (0 == buttonIndex) {//点击取消按钮
            
        }
    }
}


- (BOOL)gestureRecognizerShouldBegin:(UIGestureRecognizer *)gestureRecognizer
{
    CGPoint p = [gestureRecognizer locationInView:self.view];
    if (self.popView.visible && CGRectContainsPoint(self.popView.frame, p)) {
        return NO;
    }
    return YES;
}

- (void) showTheChooseWifiListAlertView
{
    // create the alert
    MLTableAlert *alertOneKeyExpirence = [MLTableAlert tableAlertWithTitle:NSLocalizedString(@"Choose a WiFi", @"") cancelButtonTitle:NSLocalizedString(@"Cancel", @"") numberOfRows:^NSInteger (NSInteger section)
                                          {
                                              return [self.listWifiInfo count];
                                          }
                                          
                                                                  andCells:^UITableViewCell* (MLTableAlert *anAlert, NSIndexPath *indexPath)
                                          {
                                              static NSString *CellIdentifier = @"CellIdentifier";
                                              UITableViewCell *cell = [anAlert.table dequeueReusableCellWithIdentifier:CellIdentifier];
                                              if (cell == nil)
                                                  cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier];
                                              
                                              NSInteger section = indexPath.section;
                                              NSInteger row = indexPath.row;
                                              NSString *strCameraName = @"test";
                                              if (section == 0) {
                                                  if (row < [self.listWifiInfo count]) {
                                                      KYLWifiObject *oneWifiInfo = (KYLWifiObject *)[self.listWifiInfo objectAtIndex:row];
                                                      strCameraName = oneWifiInfo.sSSID;
                                                  }
                                              }
                                              else
                                              {
                                                  
                                              }
                                              
                                              
                                              cell.textLabel.text = strCameraName;
                                              
                                              return cell;
                                          }];
    
    // Setting custom alert height
    alertOneKeyExpirence.height = 380;
    
    
    // configure actions to perform
    [alertOneKeyExpirence configureSelectionBlock:^(NSIndexPath *selectedIndex){
        NSInteger section = selectedIndex.section;
        NSInteger row = selectedIndex.row;
        if (section == 0) {
            if (row < [self.listWifiInfo count]) {
                KYLWifiObject *oneWifiInfo = (KYLWifiObject *)[self.listWifiInfo objectAtIndex:row];
                NSString *ssid = oneWifiInfo.sSSID;
                NSLog(@"选中了行row=%d, ssid=%@",(int) row, ssid);
            }
        }
        else
        {
            
        }
        
        NSLog(@"11111111111");
    } andCompletionBlock:^{
        //self.resultLabel.text = @"Cancel Button Pressed\nNo Cells Selected";
        NSLog(@"22222222222");
    }];
    
    // show the alert
    [alertOneKeyExpirence show];
    
}


- (void) doGetAllNearByWifiList
{
    if (m_pCamera) {
        [self.listWifiInfo removeAllObjects];
        [m_pCamera getCameraWifiList];
    }
}

- (int) setTheCameraWifi:(NSString *) sWifiPassword
{
    NSLog(@"setTheCameraWifi - begin");
    int nRet = -1;
    
    int nAuthType = selectedWifiObject.nAuthType;
    if (self.m_pCamera) {
        
        //self.m_pCameraObj.m_pSetWifiDelegate = self;
        
        NSString *strPKey = nil;
        NSString *strPwpa_psk = nil;
        //        char *pkey = NULL;
        //        char *pwpa_psk = NULL;
        
        //        pkey = (char*)"";
        //        pwpa_psk = (char*)"";
        //        strPKey = @"";
        //        strPwpa_psk = @"";
        
        switch (nAuthType) {
            case 0: //none
                //                pkey = (char*)"";
                //                pwpa_psk = (char*)"";
                strPKey = @"";
                strPwpa_psk = @"";
                break;
            case 1: //wep
                //                pkey = (char*)[m_strPwd UTF8String];
                //                pwpa_psk = (char*)"";
                strPKey = sWifiPassword;
                strPwpa_psk = @"";
                break;
            case 2: //wpa-psk(AES)
            case 3://wpa-psk(TKIP)
            case 4://wpa2-psk(AES)
            case 5://wpa3-psk(TKIP)
                //                pkey = (char*)"";
                //                pwpa_psk = (char*)[m_strPwd UTF8String];
                strPKey = @"";
                strPwpa_psk = sWifiPassword;
                break;
            default:
            {
                //                pkey = (char*)"";
                //                pwpa_psk = (char*)"";
                strPKey = @"";
                strPwpa_psk = @"";
            }
                break;
        }
        //判读是否包含特殊字符
        if ([self isStringContainSpecailChar2:strPwpa_psk]) {
            //
            NSString *strMsg = [NSString stringWithFormat:@"密码包含了不支持的特殊字符"];
            UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"告警" message:strMsg delegate:self cancelButtonTitle:NSLocalizedString(@"Cancel", nil) otherButtonTitles:NSLocalizedString(@"OK", nil), nil];
            [alert show];
            alert.tag = 1003;
            //alert.delegate = self;
            [alert release];
            return -2;
        }
        nRet = [self.m_pCamera setTheCameraWifi:self.m_pCamera.m_sDID ssid:selectedWifiObject.sSSID password:strPwpa_psk isEnable:1 channel:selectedWifiObject.reserve mode:0 authType:nAuthType wepEncrypt:0 wepKeyFormat:0 wepDefaultKey:0 wepKey1:strPKey wepKey2:@"" wepKey3:@"" wepKey4:@"" wepKey1Bit:0 wepKey2Bit:0 wepKey3Bit:0 wepKey4Bit:0 reqTestResult:1];
    }
    return nRet;
}

- (BOOL) isStringContainSpecailChar2:(NSString *) str
{
    BOOL bResult = NO;
    NSString *strSpecail1 = @"&";
    //NSString *strSpecail2= @"\\";
    NSString *strSpecail3 = @"\'";
    //NSString *strSpecail4 = @"\"";
    if ([str rangeOfString:strSpecail1].location !=NSNotFound
        //||[str rangeOfString:strSpecail2].location !=NSNotFound
        ||[str rangeOfString:strSpecail3].location !=NSNotFound
        //||[str rangeOfString:strSpecail4].location !=NSNotFound
        ) {
        bResult = YES;
    }
    return bResult;
}

- (void) showTheSetWiFiResult:(NSString *) sResult
{
    NSString *strMsg = [NSString stringWithFormat:@"设置WiFi %@",sResult];
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"提示" message:strMsg delegate:self cancelButtonTitle:NSLocalizedString(@"Cancel", nil) otherButtonTitles:NSLocalizedString(@"OK", nil), nil];
    [alert show];
    alert.tag = 1004;
    //alert.delegate = self;
    [alert release];
}



#pragma mark callback delegete

- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
    TSLocateView *locateView = (TSLocateView *)actionSheet;
    int selectRow = locateView.m_iSelectRow;
    //You can uses location to your application.
    NSString *strSelect = nil;
    if (m_listAllCGICommand && [m_listAllCGICommand count] > selectRow) {
        strSelect = [m_listAllCGICommand objectAtIndex:selectRow];
    }
    NSLog(@"the select item is %@",strSelect );
    if(buttonIndex == 0)
    {
        NSLog(@"Cancel");
    }
    else
    {
        [self clearTheText];
        NSLog(@"Select");
        switch (selectRow) {
            case 0://getCameraParams
            {
                if (m_pCamera) {
                    [m_pCamera getCameraParams];
                }
            }
                break;
            case 1://getCameraWifi
            {
                if (m_pCamera) {
                    [m_pCamera getCameraWifi];
                }
            }
                break;
            case 2://set wifi password
            {
                [self doTestSetWifiPassword];
            }
                break;
            case 3://getCameraWifiList
            {
                [self doGetAllNearByWifiList];
            }
                break;
            case 4://getCameraUserInfo
            {
                if (m_pCamera) {
                    [m_pCamera getCameraUserInfo];
                }
            }
                break;
            case 5://getCameraDatetimeInfo
            {
                if (m_pCamera) {
                    [m_pCamera getCameraDatetimeInfo];
                }
            }
                break;
            case 6://getCameraFTPInfo
            {
                if (m_pCamera) {
                    [m_pCamera getCameraFTPInfo];
                }
            }
                break;
            case 7://getCameraEmailInfo
            {
                if (m_pCamera) {
                    [m_pCamera getCameraEmailInfo];
                }
            }
                break;
            case 8://getCameraAlarmInfo
            {
                if (m_pCamera) {
                    [m_pCamera getCameraAlarmInfo];
                }
            }
                break;
            case 9://getCameraSDCardScheduleInfo
            {
                if (m_pCamera) {
                    [m_pCamera getCameraSDCardScheduleInfo];
                }
            }
                break;
            case 10://getCameraSoftVersionInfo
            {
                if (m_pCamera) {
                    [m_pCamera getCameraSoftVersionInfo];
                }
            }
                break;
            case 11: //getRecFileListBy
            {
                //if(m_pCamera) [m_pCamera getRecFileListBy:20150902 withRecType:3];
                [self doGotoTheGetSDCardRecordPage];
            }
                break;
                
                
            default:
                break;
        }
    }
    
}

- (void) doGotoTheGetSDCardRecordPage
{
    KYLAppDelegate *gAppDelegate= [KYLAppDelegate shareInstance];
        RemoteRecordFileListViewController *controller = [[RemoteRecordFileListViewController alloc] init];
        controller.m_pCameraObj = self.m_pCamera;
        //[gAppDelegate.rootNavController pushViewController:controller animated:YES];
    [[SliderViewController sharedSliderController].navigationController pushViewController:controller animated:YES];
        [controller release];
}




- (void) showImageOnMainThread:(id) obj
{
    [self hideTheHUD];
    UIImage *img = (UIImage *) obj;
    self.imgViewShowVideo.image = img;
}

- (void) didSucceedGetCameraHardVerson:(NSString *)strDID p2papi_ver:(NSString *)p2papi_ver fwp2p_app_ver:(NSString *)fwp2p_app_ver fwp2p_app_buildtime:(NSString *)fwp2p_app_buildtime fwddns_app_ver:(NSString *)fwddns_app_ver fwhard_ver:(NSString *)fwhard_ver vendor:(NSString *)vendor product:(NSString *)product reserve:(NSString *)reserve
{
    
}


- (void) updateTheDeviceLogText:(id) obj
{
    NSDictionary *dic = (NSDictionary *)obj;
    NSString *strStatus = [dic objectForKey:@"value"];
    
    float version = [[[UIDevice currentDevice] systemVersion] floatValue];
    if (version < 7.0) {
        self.textViewShowLog.text = strStatus;
    }
    [self.textViewShowLog insertText:strStatus];
}

- (void) initTheButtons
{
    CGRect rectMain = self.view.frame;
    
    UIImageView *imgV=[[UIImageView alloc] initWithFrame:self.view.bounds];
    [imgV setImage:[UIImage imageNamed:@"Default.png"]];
    [self.view addSubview:imgV];
    [imgV release];
    
    //self.view.backgroundColor = [UIColor grayColor];
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone)//iphone
    {
        UIView *navBar=[[UIView alloc] initWithFrame:CGRectMake(0, 0, self.view.frame.size.width, 20+[UIApplication sharedApplication].statusBarFrame.size.height)];
        navBar.backgroundColor=[UIColor whiteColor];
        navBar.alpha=0.8;
        [self.view addSubview:navBar];
        [navBar release];
        
        UIButton *leftBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        leftBtn.frame = CGRectMake(0, [UIApplication sharedApplication].statusBarFrame.size.height - 20, 44, 44);
        [leftBtn setTitle:@"+" forState:UIControlStateNormal];
        leftBtn.titleLabel.font = [UIFont boldSystemFontOfSize:16];
        [leftBtn setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
        [leftBtn addTarget:self action:@selector(leftItemClick) forControlEvents:UIControlEventTouchUpInside];
        [navBar addSubview:leftBtn];
        
        UIButton *rightBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        rightBtn.frame = CGRectMake(self.view.frame.size.width-44, [UIApplication sharedApplication].statusBarFrame.size.height - 20, 44, 44);
        [rightBtn setTitle:@"" forState:UIControlStateNormal];
        [rightBtn setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
        [rightBtn addTarget:self action:@selector(rightItemClick) forControlEvents:UIControlEventTouchUpInside];
        [navBar addSubview:rightBtn];


        //The DID textfield
        UILabel *labelDID = [[UILabel alloc] initWithFrame:CGRectMake(24, 41, 43, 21)];
        labelDID.backgroundColor = [UIColor clearColor];
        labelDID.font = [UIFont systemFontOfSize:10];
        labelDID.textColor = [UIColor whiteColor];
        [self.view addSubview:labelDID];
        labelDID.text = @"DID:";
        [labelDID release];
        
        UITextField *textFieldDID1 = [[UITextField alloc] initWithFrame:CGRectMake(66, 41, 150, 21)];
        self.textFieldDID = textFieldDID1;
        textFieldDID1.font = [UIFont systemFontOfSize:10];
        textFieldDID1.textColor = [UIColor whiteColor];
        textFieldDID1.layer.borderColor=[[UIColor whiteColor] CGColor];
        textFieldDID1.layer.borderWidth=1;
        textFieldDID1.clearButtonMode = UITextFieldViewModeWhileEditing;
        [self.view addSubview:textFieldDID1];
        [textFieldDID1 release];
        
        //The Account textfield
        UILabel *labelAccount = [[UILabel alloc] initWithFrame:CGRectMake(10, 70, 60, 21)];
        labelAccount.text = @"Account:";
        labelAccount.backgroundColor = [UIColor clearColor];
        labelAccount.textColor = [UIColor whiteColor];
        labelAccount.font = [UIFont systemFontOfSize:10];
        [self.view addSubview:labelAccount];
        [labelAccount release];
        
        UITextField *textFieldAccount1 = [[UITextField alloc] initWithFrame:CGRectMake(66, 70, 150, 21)];
        self.textFieldAccount = textFieldAccount1;
         textFieldAccount1.font = [UIFont systemFontOfSize:10];
        textFieldAccount1.textColor = [UIColor whiteColor];
        textFieldAccount1.layer.borderColor=[[UIColor whiteColor] CGColor];
        textFieldAccount1.layer.borderWidth=1;
        textFieldAccount1.clearButtonMode = UITextFieldViewModeWhileEditing;
        [self.view addSubview:textFieldAccount1];
        
        //The Password textfield
        UILabel *labelPassword = [[UILabel alloc] initWithFrame:CGRectMake(5, 95, 70, 21)];
        labelPassword.text = @"Password:";
        labelPassword.textColor = [UIColor whiteColor];
        labelPassword.backgroundColor = [UIColor clearColor];
        labelPassword.font = [UIFont systemFontOfSize:10];

        [self.view addSubview:labelPassword];
        [labelPassword release];
        
        UITextField *textFieldPassword1 = [[UITextField alloc] initWithFrame:CGRectMake(66, 95, 140, 21)];
        self.textFieldPassword = textFieldPassword1;
         textFieldPassword1.font = [UIFont systemFontOfSize:10];
        textFieldPassword1.textColor = [UIColor whiteColor];
        textFieldPassword1.layer.borderColor=[[UIColor whiteColor] CGColor];
        textFieldPassword1.layer.borderWidth=1;
        textFieldPassword1.clearButtonMode = UITextFieldViewModeWhileEditing;
        [self.view addSubview:textFieldPassword1];
        
        //btnConnect
        UIButton *btnConnect1=[UIButton buttonWithType:UIButtonTypeCustom];
        self.btnConnect = btnConnect1;
        btnConnect1.layer.borderColor=[[UIColor whiteColor] CGColor];
        btnConnect1.layer.borderWidth=2;
        [btnConnect1 setTitle:@"Connect" forState:UIControlStateNormal];
        btnConnect1.titleLabel.font = [UIFont systemFontOfSize:10];
        [btnConnect1 setFrame:CGRectMake(229, 49, 88, 50)];
        [btnConnect1 addTarget:self action:@selector(btnConnectClicked:) forControlEvents:UIControlEventTouchUpInside];
        [self.view addSubview:btnConnect1];
        
        // show device status label
        UILabel *labelDeviceStatus = [[UILabel alloc] initWithFrame:CGRectMake(13, 120, 290, 21)];
        self.lableShowStatus = labelDeviceStatus;
        labelDeviceStatus.backgroundColor = [UIColor clearColor];
        labelDeviceStatus.font = [UIFont systemFontOfSize:10];
        [self.view addSubview:labelDeviceStatus];
        [labelDeviceStatus release];
        
        // show the video image
        UIImageView *imgVideo=[[UIImageView alloc] initWithFrame:CGRectMake(1, 138, 318, 150)];
        imgVideo.backgroundColor = [UIColor blackColor];
        self.imgViewShowVideo = imgVideo;
        [self.view addSubview:imgVideo];
        
        //btnStartVideo
        UIButton *btnStartVideo1=[UIButton buttonWithType:UIButtonTypeCustom];
        self.btnStartVideo = btnStartVideo1;
        btnStartVideo1.titleLabel.font = [UIFont systemFontOfSize:10];
        btnStartVideo1.layer.borderColor=[[UIColor whiteColor] CGColor];
        btnStartVideo1.layer.borderWidth=2;
        [btnStartVideo1 setTitle:@"StartVideo" forState:UIControlStateNormal];
        [btnStartVideo1 setFrame:CGRectMake(20, 288, 60, 30)];
        [btnStartVideo1 addTarget:self action:@selector(btnStartVideoClicked:) forControlEvents:UIControlEventTouchUpInside];
        [self.view addSubview:btnStartVideo1];
        
        //btnUpClicked
        UIButton *btnUp1=[UIButton buttonWithType:UIButtonTypeCustom];
        self.btnUp = btnUp1;
        btnUp1.layer.borderColor=[[UIColor whiteColor] CGColor];
        btnUp1.layer.borderWidth=2;
        btnUp1.titleLabel.font = [UIFont systemFontOfSize:10];
        [btnUp1 setTitle:@"up" forState:UIControlStateNormal];
        [btnUp1 setFrame:CGRectMake(85, 288, 40, 30)];
        [btnUp1 addTarget:self action:@selector(btnUpClicked:) forControlEvents:UIControlEventTouchUpInside];
        [self.view addSubview:btnUp1];
        
        //btnDownClicked
        UIButton *btnDown1=[UIButton buttonWithType:UIButtonTypeCustom];
        self.btnDown = btnDown1;
        btnDown1.layer.borderColor=[[UIColor whiteColor] CGColor];
        btnDown1.layer.borderWidth=2;
        btnDown1.titleLabel.font = [UIFont systemFontOfSize:10];
        [btnDown1 setTitle:@"down" forState:UIControlStateNormal];
        [btnDown1 setFrame:CGRectMake(130, 288, 40, 30)];
        [btnDown1 addTarget:self action:@selector(btnDownClicked:) forControlEvents:UIControlEventTouchUpInside];
        [self.view addSubview:btnDown1];
        
        //btnLeftClicked
        UIButton *btnLeft1=[UIButton buttonWithType:UIButtonTypeCustom];
        self.btnLeft = btnLeft1;
        btnLeft1.layer.borderColor=[[UIColor whiteColor] CGColor];
        btnLeft1.layer.borderWidth=2;
        btnLeft1.titleLabel.font = [UIFont systemFontOfSize:10];
        [btnLeft1 setTitle:@"left" forState:UIControlStateNormal];
        [btnLeft1 setFrame:CGRectMake(175, 288, 40, 30)];
        [btnLeft1 addTarget:self action:@selector(btnLeftClicked:) forControlEvents:UIControlEventTouchUpInside];
        [self.view addSubview:btnLeft1];
        
        
        //btnRightClicked
        UIButton *btnRight1=[UIButton buttonWithType:UIButtonTypeCustom];
        self.btnRight = btnRight1;
        btnRight1.layer.borderColor=[[UIColor whiteColor] CGColor];
        btnRight1.layer.borderWidth=2;
        btnRight1.titleLabel.font = [UIFont systemFontOfSize:10];
        [btnRight1 setTitle:@"right" forState:UIControlStateNormal];
        [btnRight1 setFrame:CGRectMake(220, 288, 40, 30)];
        [btnRight1 addTarget:self action:@selector(btnRightClicked:) forControlEvents:UIControlEventTouchUpInside];
        [self.view addSubview:btnRight1];
        
        //btnStartRecord
        UIButton *btnStartRecord1=[UIButton buttonWithType:UIButtonTypeCustom];
        self.btnStartRecord = btnStartRecord1;
        btnStartRecord1.titleLabel.font = [UIFont systemFontOfSize:10];
        btnStartRecord1.layer.borderColor=[[UIColor whiteColor] CGColor];
        btnStartRecord1.layer.borderWidth=2;
        [btnStartRecord1 setTitle:@"StartRecord" forState:UIControlStateNormal];
        [btnStartRecord1 setFrame:CGRectMake(19, 328, 57, 36)];
        [btnStartRecord1 addTarget:self action:@selector(btnStartRecordClicked:) forControlEvents:UIControlEventTouchUpInside];
        [self.view addSubview:btnStartRecord1];
        
        //btnStartAudio
        UIButton *btnStartAudio1=[UIButton buttonWithType:UIButtonTypeCustom];
        self.btnStartAudio = btnStartAudio1;
        btnStartAudio1.titleLabel.font = [UIFont systemFontOfSize:10];
        btnStartAudio1.layer.borderColor=[[UIColor whiteColor] CGColor];
        btnStartAudio1.layer.borderWidth=2;
        [btnStartAudio1 setTitle:@"StartVideo" forState:UIControlStateNormal];
        [btnStartAudio1 setFrame:CGRectMake(84, 328, 63, 36)];
        [btnStartAudio1 addTarget:self action:@selector(btnStartAudioClicked:) forControlEvents:UIControlEventTouchUpInside];
        [self.view addSubview:btnStartAudio1];

        //btnStartTalk
        UIButton *btnStartTalk1=[UIButton buttonWithType:UIButtonTypeCustom];
        self.btnStartTalk = btnStartTalk1;
        btnStartTalk1.titleLabel.font = [UIFont systemFontOfSize:10];
        btnStartTalk1.layer.borderColor=[[UIColor whiteColor] CGColor];
        btnStartTalk1.layer.borderWidth=2;
        [btnStartTalk1 setTitle:@"StartVideo" forState:UIControlStateNormal];
        [btnStartTalk1 setFrame:CGRectMake(160, 328, 61, 36)];
        
        [btnStartTalk1 addTarget:self action:@selector(btnStartTalkClicked:) forControlEvents:UIControlEventTouchUpInside];
        [self.view addSubview:btnStartTalk1];

        
        //btnSendCommand
        UIButton *btnSendCommand1=[UIButton buttonWithType:UIButtonTypeCustom];
        self.btnSendCommand = btnSendCommand1;
        btnSendCommand1.titleLabel.font = [UIFont systemFontOfSize:10];
        btnSendCommand1.layer.borderColor=[[UIColor whiteColor] CGColor];
        btnSendCommand1.layer.borderWidth=2;
        [btnSendCommand1 setTitle:@"StartVideo" forState:UIControlStateNormal];
        [btnSendCommand1 setFrame:CGRectMake(229, 328, 80, 36)];
        
        [btnSendCommand1 addTarget:self action:@selector(btnSendCommandClicked:) forControlEvents:UIControlEventTouchUpInside];
        [self.view addSubview:btnSendCommand1];
        
        // show the log information textview
        UITextView *tvShowLog = [[UITextView alloc] initWithFrame:CGRectMake(10, 370, 300, 170)];
        self.textViewShowLog = tvShowLog;
        [self.view addSubview:tvShowLog];
        [tvShowLog release];
        
    }else//ipad
    {
        [imgV setImage:[UIImage imageNamed:@"Default-Portrait"]];

        UIView *navBar=[[UIView alloc] initWithFrame:CGRectMake(0, 0, self.view.frame.size.width, 44+[UIApplication sharedApplication].statusBarFrame.size.height)];
        navBar.backgroundColor=[UIColor whiteColor];
        navBar.alpha=0.8;
        [self.view addSubview:navBar];
        
        UIButton *leftBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        leftBtn.frame = CGRectMake(0, [UIApplication sharedApplication].statusBarFrame.size.height, 44, 44);
        [leftBtn setTitle:@"+" forState:UIControlStateNormal];
        leftBtn.titleLabel.font = [UIFont boldSystemFontOfSize:16];
        [leftBtn setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
        [leftBtn addTarget:self action:@selector(leftItemClick) forControlEvents:UIControlEventTouchUpInside];
        [navBar addSubview:leftBtn];
        
        UIButton *rightBtn = [UIButton buttonWithType:UIButtonTypeCustom];
        rightBtn.frame = CGRectMake(self.view.frame.size.width-44, [UIApplication sharedApplication].statusBarFrame.size.height, 44, 44);
        [rightBtn setTitle:@" " forState:UIControlStateNormal];
        [rightBtn setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
        [rightBtn addTarget:self action:@selector(rightItemClick) forControlEvents:UIControlEventTouchUpInside];
        [navBar addSubview:rightBtn];

        
        //btnNext
//        UIButton *btnNext=[UIButton buttonWithType:UIButtonTypeCustom];
//        btnNext.layer.borderColor=[[UIColor whiteColor] CGColor];
//        btnNext.layer.borderWidth=2;
//        [btnNext setTitle:@"Next" forState:UIControlStateNormal];
//        [btnNext setFrame:CGRectMake(rectMain.size.width - 120, rectMain.size.height - 55, 100, 50)];
//        //btnNext.center=self.view.center;
//        [btnNext addTarget:self action:@selector(btnNextClick:) forControlEvents:UIControlEventTouchUpInside];
//        [self.view addSubview:btnNext];
        
        //The DID textfield
        UILabel *labelDID = [[UILabel alloc] initWithFrame:CGRectMake(120, 75, 80, 21)];
        [self.view addSubview:labelDID];
        labelDID.text = @"DID:";
        [labelDID release];
        
        UITextField *textFieldDID1 = [[UITextField alloc] initWithFrame:CGRectMake(216, 71, 206, 30)];
        self.textFieldDID = textFieldDID1;
        textFieldDID1.clearButtonMode = UITextFieldViewModeWhileEditing;
        textFieldDID1.layer.borderColor=[[UIColor whiteColor] CGColor];
        textFieldDID1.layer.borderWidth=1;
        [self.view addSubview:textFieldDID1];
        [textFieldDID1 release];
        
        //The Account textfield
        UILabel *labelAccount = [[UILabel alloc] initWithFrame:CGRectMake(120, 113, 80, 21)];
        labelAccount.text = @"Account:";
        [self.view addSubview:labelAccount];
        [labelAccount release];
        
        UITextField *textFieldAccount1 = [[UITextField alloc] initWithFrame:CGRectMake(216, 109, 206, 30)];
        self.textFieldAccount = textFieldAccount1;
        textFieldAccount1.layer.borderColor=[[UIColor whiteColor] CGColor];
        textFieldAccount1.layer.borderWidth=1;
        textFieldAccount1.clearButtonMode = UITextFieldViewModeWhileEditing;
        [self.view addSubview:textFieldAccount1];
        [textFieldAccount1 release];
        
        //The Password textfield
        UILabel *labelPassword = [[UILabel alloc] initWithFrame:CGRectMake(110, 151, 100, 21)];
        labelPassword.text = @"Password:";
        [self.view addSubview:labelPassword];
        [labelPassword release];
        
        UITextField *textFieldPassword1 = [[UITextField alloc] initWithFrame:CGRectMake(216, 147, 206, 30)];
        self.textFieldPassword = textFieldPassword1;
        textFieldPassword1.layer.borderColor=[[UIColor whiteColor] CGColor];
        textFieldPassword1.layer.borderWidth=1;
        textFieldPassword1.clearButtonMode = UITextFieldViewModeWhileEditing;
        [self.view addSubview:textFieldPassword1];
        [textFieldPassword1 release];
        
        //btnConnect
        UIButton *btnConnect1=[UIButton buttonWithType:UIButtonTypeCustom];
        self.btnConnect = btnConnect1;
        btnConnect1.layer.borderColor=[[UIColor whiteColor] CGColor];
        btnConnect1.layer.borderWidth=2;
        [btnConnect1 setTitle:@"Connect" forState:UIControlStateNormal];
        [btnConnect1 setFrame:CGRectMake(510, 110, 150, 50)];
        [btnConnect1 addTarget:self action:@selector(btnConnectClicked:) forControlEvents:UIControlEventTouchUpInside];
        [self.view addSubview:btnConnect1];
        
        // show device status label
        UILabel *labelDeviceStatus = [[UILabel alloc] initWithFrame:CGRectMake(167, 208, 286, 21)];
        self.lableShowStatus = labelDeviceStatus;
        [self.view addSubview:labelDeviceStatus];
        [labelDeviceStatus release];
        
        // show the video image
        UIImageView *imgVideo=[[UIImageView alloc] initWithFrame:CGRectMake(109, 237, 550, 264)];
        imgVideo.backgroundColor = [UIColor blackColor];
        self.imgViewShowVideo = imgVideo;
        [self.view addSubview:imgVideo];
        [imgVideo release];
        
        //btnUpClicked
        UIButton *btnUp1=[UIButton buttonWithType:UIButtonTypeCustom];
        self.btnUp = btnUp1;
        btnUp1.layer.borderColor=[[UIColor whiteColor] CGColor];
        btnUp1.layer.borderWidth=2;
        [btnUp1 setTitle:@"Up" forState:UIControlStateNormal];
        [btnUp1 setFrame:CGRectMake(190, 509, 80, 44)];
        [btnUp1 addTarget:self action:@selector(btnUpClicked:) forControlEvents:UIControlEventTouchUpInside];
        [self.view addSubview:btnUp1];
        
        //btnDownClicked
        UIButton *btnDown1=[UIButton buttonWithType:UIButtonTypeCustom];
        self.btnDown = btnDown1;
        btnDown1.layer.borderColor=[[UIColor whiteColor] CGColor];
        btnDown1.layer.borderWidth=2;
        [btnDown1 setTitle:@"Down" forState:UIControlStateNormal];
        [btnDown1 setFrame:CGRectMake(285, 509, 80, 44)];
        [btnDown1 addTarget:self action:@selector(btnDownClicked:) forControlEvents:UIControlEventTouchUpInside];
        [self.view addSubview:btnDown1];
        
        //btnLeftClicked
        UIButton *btnLeft1=[UIButton buttonWithType:UIButtonTypeCustom];
        self.btnLeft = btnLeft1;
        btnLeft1.layer.borderColor=[[UIColor whiteColor] CGColor];
        btnLeft1.layer.borderWidth=2;
        [btnLeft1 setTitle:@"Left" forState:UIControlStateNormal];
        [btnLeft1 setFrame:CGRectMake(391, 509, 80, 44)];
        [btnLeft1 addTarget:self action:@selector(btnLeftClicked:) forControlEvents:UIControlEventTouchUpInside];
        [self.view addSubview:btnLeft1];
        
        //btnRightClicked
        UIButton *btnRight1=[UIButton buttonWithType:UIButtonTypeCustom];
        self.btnRight = btnRight1;
        btnRight1.layer.borderColor=[[UIColor whiteColor] CGColor];
        btnRight1.layer.borderWidth=2;
        [btnRight1 setTitle:@"Right" forState:UIControlStateNormal];
        [btnRight1 setFrame:CGRectMake(495, 509, 80, 44)];
        [btnRight1 addTarget:self action:@selector(btnRightClicked:) forControlEvents:UIControlEventTouchUpInside];
        [self.view addSubview:btnRight1];

        //btnStartVideo
        UIButton *btnStartVideo1=[UIButton buttonWithType:UIButtonTypeCustom];
        self.btnStartVideo = btnStartVideo1;
        btnStartVideo1.layer.borderColor=[[UIColor whiteColor] CGColor];
        btnStartVideo1.layer.borderWidth=2;
        [btnStartVideo1 setTitle:@"StartVideo" forState:UIControlStateNormal];
        [btnStartVideo1 setFrame:CGRectMake(96, 571, 108, 44)];
        [btnStartVideo1 addTarget:self action:@selector(btnStartVideoClicked:) forControlEvents:UIControlEventTouchUpInside];
        [self.view addSubview:btnStartVideo1];
        
        //btnStartAudio
        UIButton *btnStartAudio1=[UIButton buttonWithType:UIButtonTypeCustom];
        self.btnStartAudio = btnStartAudio1;
        btnStartAudio1.layer.borderColor=[[UIColor whiteColor] CGColor];
        btnStartAudio1.layer.borderWidth=2;
        [btnStartAudio1 setTitle:@"StartVideo" forState:UIControlStateNormal];
        [btnStartAudio1 setFrame:CGRectMake(242, 571, 108, 44)];
        [btnStartAudio1 addTarget:self action:@selector(btnStartAudioClicked:) forControlEvents:UIControlEventTouchUpInside];
        [self.view addSubview:btnStartAudio1];
        
        //btnStartTalk
        UIButton *btnStartTalk1=[UIButton buttonWithType:UIButtonTypeCustom];
        self.btnStartTalk = btnStartTalk1;
        btnStartTalk1.layer.borderColor=[[UIColor whiteColor] CGColor];
        btnStartTalk1.layer.borderWidth=2;
        [btnStartTalk1 setTitle:@"StartVideo" forState:UIControlStateNormal];
        [btnStartTalk1 setFrame:CGRectMake(383, 571, 108, 44)];
        
        [btnStartTalk1 addTarget:self action:@selector(btnStartTalkClicked:) forControlEvents:UIControlEventTouchUpInside];
        [self.view addSubview:btnStartTalk1];
        
        //btnSendCommand
        UIButton *btnSendCommand1=[UIButton buttonWithType:UIButtonTypeCustom];
        self.btnSendCommand = btnSendCommand1;
        btnSendCommand1.layer.borderColor=[[UIColor whiteColor] CGColor];
        btnSendCommand1.layer.borderWidth=2;
        [btnSendCommand1 setTitle:@"StartVideo" forState:UIControlStateNormal];
        [btnSendCommand1 setFrame:CGRectMake(521, 571, 150, 44)];
        
        [btnSendCommand1 addTarget:self action:@selector(btnSendCommandClicked:) forControlEvents:UIControlEventTouchUpInside];
        [self.view addSubview:btnSendCommand1];
        
        // show the log information textview
        UITextView *tvShowLog = [[UITextView alloc] initWithFrame:CGRectMake(96, 641, 597, 322)];
        self.textViewShowLog = tvShowLog;
        [self.view addSubview:tvShowLog];
        [tvShowLog release];
    }
}

-(void)leftItemClick{
    [[SliderViewController sharedSliderController] leftItemClick];
}
-(void)rightItemClick{
    [[SliderViewController sharedSliderController] rightItemClick];
}
-(void)btnNextClick:(id)sender{
//    ViewController1 *controller = [[ViewController1 alloc] init];
//    controller.m_pCamera = self.m_pCamera;
//    [[SliderViewController sharedSliderController].navigationController pushViewController:controller animated:YES];
//    [controller release];
}

- (void) registTheNotifications
{
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didReceiveSelectOneDeviceNotification:) name:@"KYL_notication_select_one_device" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(changeFrames:)
                                                 name:UIDeviceOrientationDidChangeNotification
                                               object:nil];
}

- (void) removeTheNotifications
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"KYL_notication_select_one_device" object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UIDeviceOrientationDidChangeNotification object:nil];
}

-(void)changeFrames:(NSNotification *)notification
{
    NSLog(@"change notification: %@", notification.userInfo);
    float width=[[UIScreen mainScreen]bounds].size.width*[[UIScreen mainScreen] scale];
    float height=[[UIScreen mainScreen]bounds].size.height*[[UIScreen mainScreen] scale];
    if ([[UIDevice currentDevice] orientation]==UIInterfaceOrientationPortrait
        || [[UIDevice currentDevice] orientation]==UIInterfaceOrientationPortraitUpsideDown)
    {
        NSLog(@"portrait");
        //[self adjustUIForPortaitMode];
    }
    else
    {
        NSLog(@"landscape");
        //[self adjustUIForLandscapeMode];
    }
    NSLog(@"view is %@",self);
}


- (void)didReceiveSelectOneDeviceNotification:(id) notification
{
    NSNotification *notice = (NSNotification *)notification;
    NSDictionary *dic = notice.userInfo;
    KYLDeviceInfo *oneDevice = (KYLDeviceInfo *)[dic objectForKey:@"device"];
    NSString *strDID = [[NSString alloc] initWithFormat:@"%@",oneDevice.m_sDID];
    self.textFieldDID.text = strDID;
    self.strDID = strDID;
    [strDID release];
}


#pragma mark - KYLCameraDelegate call back

// get camera's params
- (void) didReceiveGetCameraParams:(NSString *)did resolution:(int) nResultion bright:(int) nBright contrast:(int) nContrast hue:(int) nHue saturation:(int) nSaturation osd:(int) bOSD mode:(int) nMode flip:(int) nFlip reserve1:(NSString *) strReserve1 irLed:(int) nIRLed reserver2:(NSString *) strReserver2 user:(void *) pUser
{
    NSString *strValue = [[NSString alloc] initWithFormat:@"\nResponse: %@ GET_CAMERA_PARAM_RESP, nResolution=%d, nBright=%d, nContrast=%d, nHue=%d, nSaturation=%d, bOSD=%d, nMode=%d, nFlip=%d, nIRLed=%c\n",did,nResultion,nBright,nContrast,nHue,nSaturation,bOSD,nMode,nFlip,nIRLed];
    NSDictionary *dic = [[NSDictionary alloc] initWithObjectsAndKeys:strValue,@"value", nil];
    [strValue release];
    [self performSelectorOnMainThread:@selector(updateTheDeviceLogText:) withObject:dic waitUntilDone:NO];
    [dic release];
}

- (void) didSetCameraParamsFinish:(NSString *) did result:(int) nResult reserver:(NSString *) sReserver  bitMaskToSet:(int) nBitMaskToSet  user:(void *) pUser
{
    
}

- (void) didSetDefaultCameraParamsFinish:(NSString *) did result:(int) nResult reserver:(NSString *) sReserver  user:(void *) pUser
{

}
// search device in lan network

- (void) didSucceedGetOneSearchCameraResult:(NSString *)mac Name:(NSString *)name Addr:(NSString *)addr Port:(NSString *)port DID:(NSString*)did user:(void *) pUser
{
    
}


// set user information

- (void) didReceiveGetUserPwdResult:(NSString *)did admin:(NSString*)strAdmin adminPwd:(NSString*)strAdminPwd vistor:(NSString*)strVistor vistorPwd:(NSString*)strVistorPwd resever:(NSString*)strReserver user:(void *) pUser
{
    NSString *strValue = [[NSString alloc] initWithFormat:@"\nResponse: %@ GET_USER_INFO_RESP, chVisitor=%@, chVisitorPwd=%@, reserve=%@, chAdmin=%@, chAdminPwd=%@ \n",did,strAdmin,strAdminPwd,strVistor,strVistorPwd,strReserver];
    NSDictionary *dic = [[NSDictionary alloc] initWithObjectsAndKeys:strValue,@"value", nil];
    [strValue release];
    [self performSelectorOnMainThread:@selector(updateTheDeviceLogText:) withObject:dic waitUntilDone:NO];
    [dic release];
}

- (void) didSetUserInfoFinish:(NSString *) did result:(int) nResult reserver:(NSString *) sReserver  user:(void *) pUser
{

}

//set wifi, get wifi list

- (void) didReceiveGetWifiParams: (NSString*)strDID enable:(NSInteger)enable ssid:(NSString*)strSSID channel:(NSInteger)channel mode:(NSInteger)mode authtype:(NSInteger)authtype encryp:(NSInteger)encryp keyformat:(NSInteger)keyformat defkey:(NSInteger)defkey strKey1:(NSString*)strKey1 strKey2:(NSString*)strKey2 strKey3:(NSString*)strKey3 strKey4:(NSString*)strKey4 key1_bits:(NSInteger)key1_bits key2_bits:(NSInteger)key2_bits key3_bits:(NSInteger)key3_bits key4_bits:(NSInteger)key4_bits wpa_psk:(NSString*)wpa_psk user:(void *) pUser
{
    
}

- (void) didSucceedSetWifiParams: (NSString*)strDID enable:(NSInteger)enable ssid:(NSString*)strSSID channel:(NSInteger)channel mode:(NSInteger)mode authtype:(NSInteger)authtype encryp:(NSInteger)encryp keyformat:(NSInteger)keyformat defkey:(NSInteger)defkey strKey1:(NSString*)strKey1 strKey2:(NSString*)strKey2 strKey3:(NSString*)strKey3 strKey4:(NSString*)strKey4 key1_bits:(NSInteger)key1_bits key2_bits:(NSInteger)key2_bits key3_bits:(NSInteger)key3_bits key4_bits:(NSInteger)key4_bits wpa_psk:(NSString*)wpa_psk user:(void *) pUser
{
    NSString *strValue = [[NSString alloc] initWithFormat:@"\nResponse:%@ GET_CURRENT_WIFI_RESP, ssid=%@, enable=%d, channel=%d, mode=%d, authtype=%d, encrypt=%d, keyformat=%d, defkey=%d, key1=%@, key2=%@, key3=%@, key4=%@, key1_bits=%d, key2_bits=%d, key3_bits=%d, key4_bits=%d, wpa_psk=%@\n\n",strDID,strSSID,enable,channel,mode,authtype,encryp,keyformat,defkey,strKey1,strKey2,strKey3,strKey4,key1_bits,key2_bits,key3_bits,key4_bits,wpa_psk];
    NSDictionary *dic = [[NSDictionary alloc] initWithObjectsAndKeys:strValue,@"value", nil];
    [strValue release];
    [self performSelectorOnMainThread:@selector(updateTheDeviceLogText:) withObject:dic waitUntilDone:NO];
    [dic release];
}

- (void) didSucceedGetOneWifiScanResult: (NSString*)strDID ssid:(NSString*)strSSID mac:(NSString*)strMac security:(NSInteger)security db0:(NSString*)db0 db1:(NSString*)db1 mode:(NSInteger)mode channel:(NSInteger)channel bEnd:(NSInteger)bEnd user:(void *) pUser
{
    NSString *strValue = [[NSString alloc] initWithFormat:@"\nResponse: Get one wifi scan result %@ MSG_GET_EMAIL_INFO_RESP, ssid=%@, mac=%@, security=%d, db0=%@, db1=%@, mode=%d, channel=%d, bEnd=%d\n",strDID,strSSID,strMac,security,db0,db1,mode,channel,bEnd];
    DebugLog(@"获取一条wifi %@",strValue)
    KYLWifiObject  *oneWifi = [[KYLWifiObject alloc] init];
    oneWifi.sSSID = strSSID;
    oneWifi.sMac = strMac;
    oneWifi.nAuthType =(int) security;
    oneWifi.sSignLevel = db0;
    oneWifi.sPercent = db1;
    oneWifi.nMode = (int)mode;
    oneWifi.reserve = (int)channel;
    [self.listWifiInfo addObject:oneWifi];
    [oneWifi release];
    
    //[self.listSGPoPSelectData addObject:strSSID];
    
    if (bEnd) {
        //return;
    }
    NSDictionary *dic = [[NSDictionary alloc] initWithObjectsAndKeys:strValue,@"value", nil];
    [strValue release];
    [self performSelectorOnMainThread:@selector(updateTheDeviceLogText:) withObject:dic waitUntilDone:NO];
    [dic release];
}

- (void) didSetWifiFinish:(NSString *) did result:(int) nResult reserver:(NSString *) sReserver  user:(void *) pUser
{
    if ([self.m_pCamera.m_sDID isEqualToString:did])
    {
        if (nResult >= 0)
        {
            NSLog(@"Set wifi Succeed!");
            NSString *strResult = @"成功";
            [self performSelectorOnMainThread:@selector(showTheSetWiFiResult:) withObject:strResult waitUntilDone:NO];
        }
        else
        {
            NSLog(@"Set wifi Failed!");
            NSString *strResult = @"失败";
            [self performSelectorOnMainThread:@selector(showTheSetWiFiResult:) withObject:strResult waitUntilDone:NO];
        }
    }

}

// get datetime information
- (void) didReceiveGetDateTimeParams:(NSString *) strDID now:(int) now tz:(int) tz ntpEnable:(int) ntpEnable ntpServer:(NSString *) ntpSvr indexOfTimeZoneTable:(int) nIndex user:(void *) pUser
{
    NSString *strValue = [[NSString alloc] initWithFormat:@"\nResponse: %@ MSG_GET_DATETIME_RESP, now=%d, tz=%d, ntpEnable=%d, ntpSvr=%@\n",strDID,now,tz,ntpEnable,ntpSvr];
    NSDictionary *dic = [[NSDictionary alloc] initWithObjectsAndKeys:strValue,@"value", nil];
    [strValue release];
    [self performSelectorOnMainThread:@selector(updateTheDeviceLogText:) withObject:dic waitUntilDone:NO];
    [dic release];
}

- (void) didSetDatetimeInfoFinish:(NSString *) did result:(int) nResult reserver:(NSString *) sReserver  user:(void *) pUser
{
    
}

// snap picture

- (void) didSucceedSnapshotNotify: (NSString*) strDID data:(char*) data length:(int) length user:(void *) pUser
{
    
}

- (void) didSucceedSnapshotNotify: (NSString*) strDID image:(UIImage *) image user:(void *) pUser
{
    
}


// set sdcard schedule
-(void)didSucceedGetSdcardScheduleParams:(NSString *)did
                    bRecordCoverInSDCard:(int)bRecordCoverInSDCard
                          nRecordTimeLen:(int)nRecordTimeLen
                                reserve1:(int)reserve1
                             bRecordTime:(int) bRecordTime
                                reserve2:(NSString *)reserve2
                           nSDCardStatus:(int)nSDCardStatus
                           nSDTotalSpace:(int)nSDTotalSpace
                            nSDFreeSpace:(int)nSDFreeSpace user:(void *) pUser
{
    NSString *strValue = [[NSString alloc] initWithFormat:@"\nResponse: %@ MSG_GET_SDCARD_REC_PARAM_RESP, bRecordCoverInSDCard=%d, nRecordTimeLen=%d, reserve1=%d, bRecordTime=%d,reserve2=%@, nSDCardStatus=%d, nSDTotalSpace=%d, nSDFreeSpace=%d\n",did,bRecordCoverInSDCard,nRecordTimeLen,reserve1,bRecordTime,reserve2, nSDCardStatus,nSDTotalSpace,nSDFreeSpace];
    NSDictionary *dic = [[NSDictionary alloc] initWithObjectsAndKeys:strValue,@"value", nil];
    [strValue release];
    [self performSelectorOnMainThread:@selector(updateTheDeviceLogText:) withObject:dic waitUntilDone:NO];
    [dic release];
}

- (void) didSetCameraSDCardRecordScheduleParamsFinish:(NSString *) did result:(int) nResult reserver:(NSString *) sReserver  user:(void *) pUser
{
    
}


// device's status change notification

- (void) didReceivePPPPStatus: (NSString*) strDID statusType:(NSInteger) statusType status:(NSInteger) status
{
    
}

// set email information

- (void) didSucceedGetMailParam:(NSString*) strDID server:(NSString *) server user:(NSString*)user pwd:(NSString*)pwd sender:(NSString *) sender recv1:(NSString*)recv1 recv2:(NSString*)recv2 recv3:(NSString*)recv3 recv4:(NSString*)recv4  subject:(NSString *) strSubject port:(int) port ssl:(int)ssl  user:(void *) pUser
{
    NSString *strValue = [[NSString alloc] initWithFormat:@"\nResponse: %@ MSG_GET_EMAIL_INFO_RESP,  svr=%@, user=%@, pwd=%@, sender=%@, receiver1=%@, receiver2=%@, receiver3=%@, receiver4=%@, subject=%@, port=%d, ssl=%d\n\n",strDID,server,user,pwd,sender,recv1,recv2,recv3,recv4,strSubject, port,ssl];
    NSDictionary *dic = [[NSDictionary alloc] initWithObjectsAndKeys:strValue,@"value", nil];
    [strValue release];
    [self performSelectorOnMainThread:@selector(updateTheDeviceLogText:) withObject:dic waitUntilDone:NO];
    [dic release];
}

- (void) didSetCameraEmailInfoFinish:(NSString *) did result:(int) nResult reserver:(NSString *) sReserver  user:(void *) pUser
{

}


// set ftp information

- (void) didSucceedGetFtpParam:(NSString*) strDID ftpserver: (NSString*) svr user:(NSString*) user pwd:(NSString*) pwd dir:(NSString*)dir port:(int)port uploadinterval: (int) uploadinterval mode:(int)mode user:(void *) pUser
{
    NSString *strValue = [[NSString alloc] initWithFormat:@"\nResponse: %@ MSG_GET_FTP_INFO_RESP, ftpserver=%@, user=%@, pwd=%@, dir=%@, port=%d, uploadinterval=%d, mode=%d\n",strDID,svr,user,pwd,dir,port,uploadinterval,mode];
    NSDictionary *dic = [[NSDictionary alloc] initWithObjectsAndKeys:strValue,@"value", nil];
    [strValue release];
    [self performSelectorOnMainThread:@selector(updateTheDeviceLogText:) withObject:dic waitUntilDone:NO];
    [dic release];
}

- (void) didSetCameraFTPInfoFinish:(NSString *) did result:(int) nResult reserver:(NSString *) sReserver  user:(void *) pUser
{
    
}


// get camera hard soft version informat
- (void) didSucceedGetCameraHardVerson:(NSString *) strDID
                            p2papi_ver:(NSString *) p2papi_ver
                        fwp2p_app_ver :(NSString *)fwp2p_app_ver  fwp2p_app_buildtime :(NSString *)fwp2p_app_buildtime fwddns_app_ver:(NSString *)fwddns_app_ver
                            fwhard_ver:(NSString *)fwhard_ver
                                vendor:(NSString *)vendor
                               product:(NSString *)product
                               reserve: (NSString *)reserve  user:(void *) pUser
{
    
}


// set alarm information
- (void) didSucceedGetAlarmProtocolResult:(NSString*) strDID
                                bMDEnable:(int)bMDEnable
                           nMDSensitivity:(int)nMDSensitivity
                              bInputAlarm:(int)bInputAlarm
                          nInputAlarmMode:(int)nInputAlarmMode
                      bIOLinkageWhenAlarm:(int)bIOLinkageWhenAlarm
                                 reserve1:(int)reserve1
                      nPresetbitWhenAlarm:(int)nPresetbitWhenAlarm
                           bMailWhenAlarm:(int)bMailWhenAlarm
                   bSnapshotToSDWhenAlarm:(int)bSnapshotToSDWhenAlarm
                     bRecordToSDWhenAlarm:(int)bRecordToSDWhenAlarm
                  bSnapshotToFTPWhenAlarm:(int)bSnapshotToFTPWhenAlarm
                    bRecordToFTPWhenAlarm:(int)bRecordToFTPWhenAlarm
                              strreserve2:(NSString *)strreserve2 user:(void *) pUser
{
    NSString *strValue = [[NSString alloc] initWithFormat:@"\nResponse: %@ MSG_GET_ALARM_INFO_RESP, bMDEnable=%d, nMDSensitivity=%d, bInputAlarm=%d, nInputAlarmMode=%d, bIOLinkageWhenAlarm=%d, nPresetbitWhenAlarm=%d, bMailWhenAlarm=%d, bSnapshotToSDWhenAlarm=%d, bRecordToSDWhenAlarm=%d, bSnapshotToFTPWhenAlarm=%d, bRecordToFTPWhenAlarm=%d \n",strDID,bMDEnable,nMDSensitivity,bInputAlarm,nInputAlarmMode,bIOLinkageWhenAlarm,nPresetbitWhenAlarm,bMailWhenAlarm,bSnapshotToSDWhenAlarm,bRecordToSDWhenAlarm,bSnapshotToFTPWhenAlarm,bRecordToFTPWhenAlarm];
    NSDictionary *dic = [[NSDictionary alloc] initWithObjectsAndKeys:strValue,@"value", nil];
    [strValue release];
    [self performSelectorOnMainThread:@selector(updateTheDeviceLogText:) withObject:dic waitUntilDone:NO];
    [dic release];
}



- (void) didSetCameraAlarmInfoFinish:(NSString *) did result:(int) nResult reserver:(NSString *) sReserver  user:(void *) pUser
{
    
}

// talk request response
- (void) didReceiveTheTalkResponseNotify:(NSString *) strDID talkStatus:(int) nTalkStatus reserver:(NSString *) sReserve
{
    
}


//remote record file playback
- (void) didReceiveTheGetRemoteRecordDaysByMonth:(NSString *) strDID recordType:(int) nRecType yearMonth:(int) nYearMonth theRecordedDays:(NSArray *) dayList reserve:(NSString *) sReserve
{
    
}
- (void) didReceiveTheGetRemoteRecordFilesInOneDay:(NSString *) strDID
                                            result:(int) nResult
                                      fileTotalNum:(int) nFileTotalNum
                                 beginNofoThisTime:(int) nBeginNoOfThisTime
                                   endNoOfThisTime:(int) nEndNoOfThisTime
                                    theRecordFiles:(NSArray *) allRecordFilesByDayList
{
    
}
- (void) didReceiveTheGetOneRemoteRecordFileInOneDay:(NSString *) strDID
                                            filePath:(NSString *) sFilePath
                                           startTime:(NSString *) sStartTime
                                             endTime:(NSString *) sEndTime
                                      fileTimeLength:(int) nTimeLength
                                            fileSize:(int) nFileSize
                                          recordType:(int) nRecordType
                                             reserve:(NSString *) sReserve
                                       isTheLastItem:(BOOL) bLast
{
    
}
- (void) didReceiveTheStartPlayRemoteRecordFile:(NSString *) strDID
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
    
}

- (void) didReceiveTheStopPlayRemoteRecordFile:(NSString *) strDID result:(int) nResult filePath:(NSString *) sFilePath reserve1:(NSString *) sReserve1
{
    
}
- (void) didReceiveRemoteRecordPlaybackMJPEGImageNotify: (UIImage *)image timestamp: (NSInteger)timestamp DID:(NSString *)did user:(void *) pUser
{
    
}

- (void) didReceiveRemoteRecordPlaybackOneVideoFrameYUVNotify: (char*) yuvdata length:(unsigned long)length width: (int) width height:(int)height timestamp:(unsigned int)timestamp DID:(NSString *)did user:(void *) pUser
{
    
}

- (void) didReceiveRemoteRecordPlaybackOneVideoFrameRGB24Notify: (char*) rgb24data length:(unsigned long)length width: (int) width height:(int)height timestamp:(unsigned int)timestamp DID:(NSString *)did user:(void *) pUser
{
    
}

- (void) didReceiveRemoteRecordPlaybackOneH264VideoFrameWithH264Data: (char*) h264Framedata length: (unsigned long) length type: (int) type timestamp: (NSInteger) timestamp DID:(NSString *)did user:(void *) pUser
{
    
}



-  (void) didReceiveCameraStatus:(NSString *) did status:(int) status reserve:(NSString *) reserve1 user:(void *) pUser
{
    NSString *sttStatus = nil;
    switch (status) {
        case CONNECT_STATUS_CONNECTING:
        {
            sttStatus = @"connecting";
        }
            break;
        case CONNECT_STATUS_INITIALING:
        {
            sttStatus = @"initialing";
        }
            break;
        case CONNECT_STATUS_ONLINE:
        {
            sttStatus = @"online";
        }
            break;
        case CONNECT_STATUS_CONNECT_FAILED:
        {
            sttStatus = @"connect failed";
        }
            break;
        case CONNECT_STATUS_DISCONNECT:
        {
            sttStatus = @"disconnect";
        }
            break;
        case CONNECT_STATUS_INVALID_ID:
        {
            sttStatus = @"invalid id";
        }
            break;
        case CONNECT_STATUS_DEVICE_NOT_ONLINE:
        {
            sttStatus = @"offline";
        }
            break;
        case CONNECT_STATUS_CONNECT_TIMEOUT:
        {
            sttStatus = @"connect timeout";
        }
            break;
        case CONNECT_STATUS_WRONG_USER_PWD:
        {
            sttStatus = @"wrong password";
        }
            break;
        case CONNECT_STATUS_INVALID_REQ:
        {
            sttStatus = @"invalid request";
        }
            break;
        case CONNECT_STATUS_EXCEED_MAX_USER:
        {
            sttStatus = @"exceed max user";
        }
            break;
        case CONNECT_STATUS_CONNECTED:
        {
            sttStatus = @"connected";
            
            //获取wifi list
            [self doGetAllNearByWifiList];
        }
            break;
        case CONNECT_STATUS_UNKNOWN:
        {
            sttStatus = @"unknown status";
        }
            break;
            
        default:
            break;
    }
    
    
    DebugLog(@"设备状态改变，did=%@, status = %d", self.m_pCamera.m_sDID,status);
    NSDictionary *dic = [[NSDictionary alloc] initWithObjectsAndKeys:sttStatus,@"status",[NSNumber numberWithInt:status],@"nStatus", nil];
    [self performSelectorOnMainThread:@selector(updateTheDeviceStatusText:) withObject:dic waitUntilDone:NO];
    [dic release];

}



- (void) updateTheDeviceStatusText:(id) obj
{
    NSDictionary *dic = (NSDictionary *)obj;
    NSString *strStatus = [dic objectForKey:@"status"];
    self.lableShowStatus.text = strStatus;
    if([[dic objectForKey:@"nStatus"] integerValue] == CONNECT_STATUS_CONNECTED)
    {
        [self enableTheSendCommandButton:YES];
        [self enableTheStartVideoButton:YES];
    }
}





@end

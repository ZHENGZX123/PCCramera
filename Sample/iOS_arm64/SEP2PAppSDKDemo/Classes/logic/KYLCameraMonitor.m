//
//  KYLCameraMonitor.m
//  P2PCamera
//
//
//

#import "KYLCameraMonitor.h"

#import "APICommon.h"
#import "MBProgressHUD.h"
#import "KYLComFunUtil.h"

@interface KYLCameraMonitor()
{
    UIScrollView *m_pBgScrollView;
    
    BOOL bPlaying;
    BOOL m_bIsCreateOpenGL;
    BOOL m_bShowRightButton;
    BOOL m_bBtnStartShow;
    
    UIImageView *m_pImageViewForCover;
    AAShareBubbles *shareBubbles;
    float radius;
    float bubbleRadius;
    BOOL m_bIsSelected;

    BOOL m_bInBackground;
}

@end

@implementation KYLCameraMonitor
@synthesize delegate;
@synthesize m_pCameraObj;
@synthesize btnStart;
@synthesize btnRightBig;
@synthesize lableCameraName;
@synthesize m_pImageView;
@synthesize m_iIndex;
@synthesize m_nAudioStatusWhenLeaveFullScreen;
@synthesize m_nTalkStatusWhenLeaveFulllScreen;



- (id) init
{
    self = [super init];
    if (self) {
        [self initData];
    }
    return self;
}

- (void) initData
{
    m_nAudioStatusWhenLeaveFullScreen = 0;
    m_nTalkStatusWhenLeaveFulllScreen = 0;
    self.m_pImageView = nil;
    bPlaying = NO;
    m_bIsCreateOpenGL = NO;
    m_iIndex = 0;
    
    radius = 60;
    bubbleRadius = 20;
    m_bIsSelected = NO;

    m_bInBackground = NO;
    [self registerTheNotifications];
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        [self initData];
        // Initialization code
        [self initTheUIWithFrame:frame];
        
    }
    return self;
}


- (void) initTheUIWithFrame:(CGRect)frame
{
    m_pImageView = [[UIImageView alloc] initWithFrame:CGRectMake(1, 1, frame.size.width-2, frame.size.height-2)];
    m_pBgScrollView = [[UIScrollView alloc]initWithFrame:CGRectMake(1, 1, frame.size.width-2, frame.size.height-2)];
    m_pBgScrollView.minimumZoomScale = ZOOM_MIN_SCALE;
    m_pBgScrollView.maximumZoomScale = ZOOM_MAX_SCALE;
    m_pBgScrollView.contentMode = UIViewContentModeScaleAspectFit;
    m_pBgScrollView.contentSize = self.m_pImageView.frame.size;
    m_pBgScrollView.delegate = self;
    [m_pBgScrollView addSubview:m_pImageView];
    
    [self addSubview:m_pBgScrollView];
    
    m_pImageViewForCover = [[UIImageView alloc] initWithFrame:m_pImageView.frame];
    m_pImageViewForCover.backgroundColor = [UIColor blackColor];
    
    UILabel *tempLabel = [[UILabel alloc] initWithFrame:CGRectMake( 5, 0, 100, 20)];
    self.lableCameraName = tempLabel;
    self.lableCameraName.textColor = [UIColor whiteColor];
    self.lableCameraName.font = [UIFont systemFontOfSize:9];
    self.lableCameraName.backgroundColor = [UIColor clearColor];
    [tempLabel release];
    [self addSubview:tempLabel];
    
    //UIButton *tempBtn = [[UIButton alloc] initWithFrame:CGRectMake(5, 4, 60, 60)];
    UIButton *tempBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    [tempBtn setBackgroundImage:[UIImage imageNamed:@"btn_start2"] forState:UIControlStateNormal];
    [tempBtn addTarget:self action:@selector(btnStartClicked:) forControlEvents:UIControlEventTouchUpInside];
    self.btnStart = tempBtn;
    [self addSubview:tempBtn];
    tempBtn.center = self.center;
    
    [self setMinimumGestureLength:40 MaximumVariance:20];
    
    self.backgroundColor  = [UIColor blackColor];
}

- (void) initTheUIWithFrame22:(CGRect)frame
{
    CGRect rect = CGRectMake(0, 0, frame.size.width, frame.size.height);
    
    m_pBgScrollView = [[UIScrollView alloc]initWithFrame:rect];
    m_pImageView = [[UIImageView alloc] initWithFrame:rect];
    m_pBgScrollView.minimumZoomScale = ZOOM_MIN_SCALE;
    m_pBgScrollView.maximumZoomScale = ZOOM_MAX_SCALE;
    m_pBgScrollView.contentMode = UIViewContentModeScaleAspectFit;
    m_pBgScrollView.contentSize = self.m_pImageView.frame.size;
    m_pBgScrollView.delegate = self;
    [m_pBgScrollView addSubview:m_pImageView];
    
    [self addSubview:m_pBgScrollView];
    
    m_pImageViewForCover = [[UIImageView alloc] initWithFrame:m_pImageView.frame];
    m_pImageViewForCover.backgroundColor = [UIColor blackColor];
    
    UILabel *tempLabel = [[UILabel alloc] initWithFrame:CGRectMake( 5, 0, 100, 20)];
    self.lableCameraName = tempLabel;
    self.lableCameraName.textColor = [UIColor whiteColor];
    self.lableCameraName.font = [UIFont systemFontOfSize:9];
    self.lableCameraName.backgroundColor = [UIColor clearColor];
    [tempLabel release];
    [self addSubview:tempLabel];
    
    //UIButton *tempBtn = [[UIButton alloc] initWithFrame:CGRectMake(5, 4, 60, 60)];
    UIButton *tempBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    [tempBtn setBackgroundImage:[UIImage imageNamed:@"btn_start2"] forState:UIControlStateNormal];
    [tempBtn addTarget:self action:@selector(btnStartClicked:) forControlEvents:UIControlEventTouchUpInside];
    self.btnStart = tempBtn;
    [self addSubview:tempBtn];
    tempBtn.center = self.center;
    

    //self.btnStart.hidden = YES;
    [self setMinimumGestureLength:40 MaximumVariance:20];
    
    self.backgroundColor  = [UIColor blackColor];
}


- (void)dealloc
{
    NSLog(@"KYLCameraMonitor dealloc()");
    [self removeTheNotifications];
    self.m_pCameraObj.delegate = nil;
    self.m_pCameraObj.m_pImageDelegate = nil;
    
    if (m_pCameraObj) {
        [m_pCameraObj stopVideo];
        [m_pCameraObj stopTalk];
        [m_pCameraObj stopAudio];
    }
    self.m_pCameraObj = nil;
    //self.myGLViewController = nil;
    
    self.lableCameraName = nil;
    self.btnStart = nil;
    self.btnRightBig = nil;
    self.m_pImageView = nil;
    if (m_pImageViewForCover) {
        [m_pImageViewForCover release];
        m_pImageViewForCover = nil;
    }
    if (m_pBgScrollView) {
        [m_pBgScrollView release];
        m_pBgScrollView = nil;
    }

    [super dealloc];
}


#pragma mark actions
- (void) btnStartClicked:(id) sender
{
    m_bBtnStartShow = !m_bBtnStartShow;
    if (m_bBtnStartShow) {
        //[btnStart setBackgroundImage:[UIImage imageNamed:@""] forState:UIControlStateNormal];
    }
    else
    {
        //[btnStart setBackgroundImage:[UIImage imageNamed:@""] forState:UIControlStateNormal];
    }
}

- (void) setNeedsLayout
{
    [super setNeedsLayout];
    //[self refreshTheUI];
    [self refreshTheFrame:self.frame];
}

- (void) refreshTheFrame:(CGRect) frame
{
    m_pImageView.frame = CGRectMake(1, 1, frame.size.width-2, frame.size.height-2);
    m_pImageViewForCover.frame = m_pImageView.frame;
    m_pBgScrollView.frame = m_pImageView.frame;

    m_pBgScrollView.zoomScale = 1.0;
    lableCameraName.frame = CGRectMake( 5, 4, 100, 20);
    self.btnStart.center = self.center;
    
    [self bringSubviewToFront:btnStart];
}

- (void) refreshTheUI
{
    CGRect rect = CGRectMake(0, 0, self.frame.size.width, self.frame.size.height);
    m_pImageView.frame = rect;
    m_pImageViewForCover.frame = rect;

    m_pBgScrollView.frame = rect;
    //m_pBgScrollView.zoomScale = 1.0;
    lableCameraName.frame = CGRectMake( 5, 4, 100, 20);
    self.btnStart.center = self.center;
    [self bringSubviewToFront:btnStart];
}


- (void) recoverTheScrollView
{
    if (m_pBgScrollView) {
        m_pBgScrollView.frame = m_pImageView.frame;
        m_pBgScrollView.zoomScale = 1.0;
    }
}

#pragma mark - Public Methods

- (void) initTheDisplayView
{

    [self setPlay:NO];
}


- (void)setMinimumGestureLength:(NSInteger)length MaximumVariance:(NSInteger)variance
{
    minGestureLength = length;
    maxVariance = variance;
    
    UIPinchGestureRecognizer *pinch = [[UIPinchGestureRecognizer alloc]
                                       initWithTarget:self
                                       action:@selector(doPinch:)] ;
    [self addGestureRecognizer:pinch];
    [pinch release];
    
    [self addGesture];
}


- (void) addGesture
{
    UITapGestureRecognizer* singleTapRecognizer;
    singleTapRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleSingleTapFrom:)];
    singleTapRecognizer.numberOfTapsRequired = 1; // 单击
    [self addGestureRecognizer:singleTapRecognizer];
    
    // 双击的 Recognizer
    UITapGestureRecognizer* doubleTapRecognizer;
    doubleTapRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleDoubleTapFrom:)];
    doubleTapRecognizer.numberOfTapsRequired = 2; // 双击
    [self addGestureRecognizer:doubleTapRecognizer];
    
    // 关键在这一行，如果双击确定偵測失败才會触发单击
    [singleTapRecognizer requireGestureRecognizerToFail:doubleTapRecognizer];
    
    // 向上擦碰
    UISwipeGestureRecognizer *oneFingerSwipeUp =
    [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerSwipeUp:)] ;
    [oneFingerSwipeUp setDirection:UISwipeGestureRecognizerDirectionUp];
    [self  addGestureRecognizer:oneFingerSwipeUp];
    
    
    // 向下擦碰
    UISwipeGestureRecognizer *oneFingerSwipeDown =
    [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerSwipeDown:)] ;
    [oneFingerSwipeDown setDirection:UISwipeGestureRecognizerDirectionDown];
    [self  addGestureRecognizer:oneFingerSwipeDown];
    
    // 向左擦碰
    UISwipeGestureRecognizer *oneFingerSwipeLeft =
    [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerSwipeLeft:)] ;
    [oneFingerSwipeLeft setDirection:UISwipeGestureRecognizerDirectionLeft];
    [self  addGestureRecognizer:oneFingerSwipeLeft];
    
    
    // 向右擦碰
    UISwipeGestureRecognizer *oneFingerSwipeRight =
    [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerSwipeRight:)]   ;
    [oneFingerSwipeRight setDirection:UISwipeGestureRecognizerDirectionRight];
    [self  addGestureRecognizer:oneFingerSwipeRight];
    
    //长按手势
    UILongPressGestureRecognizer *longPressGesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(longPressGestureDid:)];
    longPressGesture.minimumPressDuration = 2;
    [self addGestureRecognizer:longPressGesture];
    
    
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
        
        if (self.delegate && [self.delegate respondsToSelector:@selector(KYLMontiorTouchProtocolDidGestureLongPressed:)]) {
            [self.delegate KYLMontiorTouchProtocolDidGestureLongPressed:self];
        }
    }
    //[self showTheMenu];
    
}

// 单击
- (void)handleSingleTapFrom:(UITapGestureRecognizer*)recognizer {
    if (recognizer.state == UIGestureRecognizerStateEnded) {
        
        //[self changeBordColorToChoosedColor];
        if (self.delegate && [self.delegate respondsToSelector:@selector(KYLMontiorTouchProtocolDidGestureOneTap:)]) {
            [self.delegate KYLMontiorTouchProtocolDidGestureOneTap:self];
        }
    }
    
}

// 双击
- (void)handleDoubleTapFrom:(UITapGestureRecognizer*)recognizer {
    if (recognizer.state == UIGestureRecognizerStateEnded) {
        NSLog(@"KYLCameraMonitor 双击窗口");
        if (self.delegate && [self.delegate respondsToSelector:@selector(KYLMontiorTouchProtocolDidGestureDoubleTap:)]) {
            [self.delegate KYLMontiorTouchProtocolDidGestureDoubleTap:self];
        }
    }
    
}

//向上滑动
- (void)oneFingerSwipeUp:(UISwipeGestureRecognizer *)recognizer
{
    NSLog(@"向上滑动");
    if (recognizer.state == UIGestureRecognizerStateEnded) {
        
        if (self.delegate && [self.delegate respondsToSelector:@selector(KYLMontiorTouchProtocolDidGestureSwiped:user:)]) {
            [self.delegate KYLMontiorTouchProtocolDidGestureSwiped:DirectionUp user:self];
        }
        //[self up];
        //机器向下运动，看到的图像与机器运动方向相反，图像向上运动
        if ([self.m_pCameraObj IsConnectedOK] && [self.m_pCameraObj IsAdmin])
        {
            [self down];
        }
        
    }
    
}

// 向下滑动
- (void)oneFingerSwipeDown:(UISwipeGestureRecognizer *)recognizer
{
    NSLog(@"向下滑动");
    if (recognizer.state == UIGestureRecognizerStateEnded) {
        
        if (self.delegate && [self.delegate respondsToSelector:@selector(KYLMontiorTouchProtocolDidGestureSwiped:user:)]) {
            [self.delegate KYLMontiorTouchProtocolDidGestureSwiped:DirectionDown user:self];
        }
        if ([self.m_pCameraObj IsConnectedOK] && [self.m_pCameraObj IsAdmin])
        {
            [self up];
        }
        //[self up];
    }
}

//向左滑动
- (void)oneFingerSwipeLeft:(UISwipeGestureRecognizer *)recognizer
{
    NSLog(@"向左滑动");
    if (recognizer.state == UIGestureRecognizerStateEnded) {
        
        if (self.delegate && [self.delegate respondsToSelector:@selector(KYLMontiorTouchProtocolDidGestureSwiped:user:)]) {
            [self.delegate KYLMontiorTouchProtocolDidGestureSwiped:DirectionLeft user:self];
        }
      
        if ([self.m_pCameraObj IsConnectedOK] && [self.m_pCameraObj IsAdmin])
        {
            [self right];
        }
        //[self right];
    }
}

// 向右滑动
- (void)oneFingerSwipeRight:(UISwipeGestureRecognizer *)recognizer
{
    NSLog(@"向右滑动");
    if (recognizer.state == UIGestureRecognizerStateEnded) {
        if (self.delegate && [self.delegate respondsToSelector:@selector(KYLMontiorTouchProtocolDidGestureSwiped:user:)]) {
            [self.delegate KYLMontiorTouchProtocolDidGestureSwiped:DirectionRight user:self];
        }
        if ([self.m_pCameraObj IsConnectedOK] && [self.m_pCameraObj IsAdmin])
        {
            [self left];
        }
        //[self left];
    }
}

- (void)doPinch:(UIPinchGestureRecognizer *)pinch
{
    if (pinch.state == UIGestureRecognizerStateEnded) {
        
        if (self.delegate && [self.delegate respondsToSelector:@selector(KYLMontiorTouchProtocolDidGesturePinched:user:)]) {
            [self.delegate KYLMontiorTouchProtocolDidGesturePinched:pinch.scale user:self];
        }
    }
}

- (void) touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    NSLog(@"\n KYLCameraMonitor touchedbegan------\n");
}

#pragma mark opengl 
- (void) CreateGLView
{
    m_bIsCreateOpenGL = YES;
    
    [self bringSubviewToFront:lableCameraName];
    [self bringSubviewToFront:btnStart];
}

- (void) showTheRightButton:(BOOL) bShow
{
    if (bShow) {
        self.btnRightBig.hidden = NO;
    }
    else
    {
        self.btnRightBig.hidden = YES;
    }
}

//显示HUD
- (void) showTheHUD
{
    MBProgressHUD *hud = [MBProgressHUD showHUDAddedTo:self animated:YES];
	hud.labelText =  NSLocalizedString(@"loading...", @"");
    //CGRect rect = self.view.frame;
    //hud.minSize = rect.size;
    hud.backgroundColor = [UIColor clearColor];
    hud.opacity = 0.2;
    hud.opaque = NO;
    hud.minSize = CGSizeMake(80.f, 80.f);
    
}

- (void) showTheHUD2
{
    MBProgressHUD *hud = [MBProgressHUD showHUDAddedTo:self animated:YES];
	hud.labelText =  NSLocalizedString(@"", @"");
    //CGRect rect = self.view.frame;
    //hud.minSize = rect.size;
    hud.backgroundColor = [UIColor clearColor];
    hud.opacity = 0.2;
    hud.opaque = NO;
    hud.minSize = CGSizeMake(80.f, 80.f);
    
}

//隐藏HUD
- (void) hideTheHUD
{
    [MBProgressHUD hideHUDForView:self animated:YES];
}

- (BOOL) isCreateGLView
{
    return m_bIsCreateOpenGL;
}


- (void) showTheVideoStopedStatus
{
    if (m_pImageViewForCover) {
        m_pImageViewForCover.image = [UIImage imageNamed:@"backdev"];
        [self insertSubview:m_pImageViewForCover aboveSubview:m_pBgScrollView];
    }
}

- (void) hideTheVideoStopedStatus
{
    if (m_pImageViewForCover) {
        [m_pImageViewForCover removeFromSuperview];
    }
}


- (void) showTheVideoStopedBgImage:(BOOL) bShow
{
    if (bShow) {
        [self showTheVideoStopedStatus];
    }
    else
    {
        [self hideTheVideoStopedStatus];
    }
}

#pragma mark menus

- (void) showTheMenu
{
    if(shareBubbles == nil) {
        // shareBubbles = [[[AAShareBubbles alloc] initWithPoint:self.center radius:radius inView:self] autorelease];
    }
    shareBubbles = [[[AAShareBubbles alloc] initWithPoint:self.center radius:radius inView:self] autorelease];
    shareBubbles.delegate = self;
    shareBubbles.bubbleRadius = bubbleRadius;
    shareBubbles.showFacebookBubble = YES;
    shareBubbles.showTwitterBubble = YES;
    shareBubbles.showGooglePlusBubble = YES;
    shareBubbles.showMailBubble = YES;
    shareBubbles.showTumblrBubble = YES;
    [shareBubbles show];
    
}


#pragma mark AAShareBubbles

-(void)aaShareBubbles:(AAShareBubbles *)shareBubbles tappedBubbleWithType:(AAShareBubbleType)bubbleType
{
    switch (bubbleType) {
        case AAShareBubbleTypeFacebook:
            //NSLog(@"Facebook");
            NSLog(@"Audio");
            break;
        case AAShareBubbleTypeTwitter:
            //NSLog(@"Twitter");
            NSLog(@"Talk");
            break;
        case AAShareBubbleTypeMail:
            //NSLog(@"Email");
            NSLog(@"record");
            break;
        case AAShareBubbleTypeGooglePlus:
            //NSLog(@"Google+");
            NSLog(@"horrioral scan");
            break;
        case AAShareBubbleTypeTumblr:
            //NSLog(@"Tumblr");0
            NSLog(@"verical scan");
            break;
            
        default:
            break;
    }
}

#pragma mark action
- (int) startVideo
{
    int nRet = -1;
    if (self.m_pCameraObj) {
        NSString *strCameraName = self.m_pCameraObj.m_sDeviceName;
        self.lableCameraName.text = strCameraName;
        if (self.m_pCameraObj.m_nDeviceType == DEVICE_TYPE_NVR) {
            self.lableCameraName.text = [NSString stringWithFormat:@"%@_%d",NSLocalizedString(@"Channel", nil),self.m_pCameraObj.m_nCurrentChannel+1];;
        }
        [self hideTheHUD];
        [self showTheHUD2];
        [self performSelector:@selector(hideTheHUD) withObject:nil afterDelay:2];
        self.m_pCameraObj.m_pImageDelegate = self;
        //[self.m_pCameraObj stopVideo];
        [self setPlay:NO];
        nRet = [self.m_pCameraObj startVideo];
    }
    return nRet;
}

- (int) stopVideo
{
    int nRet = -1;
    if (self.m_pCameraObj) {
        self.m_pCameraObj.m_pImageDelegate = nil;
        nRet = [self.m_pCameraObj stopVideo];
        [self.m_pCameraObj stopAudio];
        [self.m_pCameraObj stopTalk];
        [self hideTheHUD];
        [self initTheDisplayView];
    }
    return nRet;
}

- (void) startAudio
{
    if (m_pCameraObj) {
        [m_pCameraObj startAudio];
    }
}

- (void) stopAudio
{
    if (m_pCameraObj) {
        [m_pCameraObj stopAudio];
    }
}

- (void) startTalk
{
    if (m_pCameraObj) {
        [m_pCameraObj startTalk];
    }
}

- (void) stopTalk
{
    if (m_pCameraObj) {
        [m_pCameraObj stopTalk];
    }
}


//record
- (void) stopRecord
{
    if (m_pCameraObj) {
        [m_pCameraObj startLocalRecord];
    }
}

- (void) startRecord
{
    if (m_pCameraObj) {
        [m_pCameraObj stopLocalRecord];
    }
}




- (int) up
{
    int nRet = -1;
    if (self.m_pCameraObj && bPlaying) {
        nRet = [self.m_pCameraObj up];
    }
    return nRet;
}

- (int) down
{
    int nRet = -1;
    if (self.m_pCameraObj && bPlaying) {
        nRet = [self.m_pCameraObj down];
    }
    return nRet;
}


- (int) left
{
    int nRet = -1;
    if (self.m_pCameraObj && bPlaying) {
        nRet = [self.m_pCameraObj left];
    }
    return nRet;
}


- (int) right
{
    int nRet = -1;
    if (self.m_pCameraObj && bPlaying) {
        nRet = [self.m_pCameraObj right];
    }
    return nRet;
}

- (int) goLeftRight
{
    int nRet = -1;
    if (self.m_pCameraObj && bPlaying) {
        nRet = [self.m_pCameraObj startGoLeftRight];
    }
    return nRet;
}


- (int) goUpDown
{
    int nRet = -1;
    if (self.m_pCameraObj && bPlaying) {
        nRet = [self.m_pCameraObj startGoUpDown];
    }
    return nRet;
}

- (void) startTurnLeftRight
{
    if (m_pCameraObj) {
        [m_pCameraObj startGoLeftRight];
    }
}

- (void) stopTurnLeftRight
{
    if (m_pCameraObj) {
        [m_pCameraObj stopGoLeftRight];
    }
}

- (void) startTurnUpDown
{
    if (m_pCameraObj) {
        [m_pCameraObj startGoUpDown];
    }
}

- (void) stopTurnUpDown
{
    if (m_pCameraObj) {
        [m_pCameraObj stopGoUpDown];
    }
}

- (void) setPlay:(BOOL) bPlay
{
    {
        bPlaying = bPlay;
    }
}

- (void) snapPicture
{
}



- (void) initTheBordColor
{
    //self.layer.borderColor=[[UIColor greenColor] CGColor];
    self.layer.borderColor=[[UIColor whiteColor] CGColor];
    self.layer.borderWidth=0.6;
    m_bIsSelected = NO;
}

- (void) clearTheBordColor
{
    self.layer.borderColor=[[UIColor clearColor] CGColor];
    self.layer.borderWidth=0;
    m_bIsSelected = NO;
}

- (void) changeBordColorToChoosedColor
{
    self.layer.borderColor=[[UIColor yellowColor] CGColor];
    self.layer.borderWidth=2;
    m_bIsSelected = YES;
}

- (BOOL) isSelected
{
    return m_bIsSelected;
}


#pragma mark common functions
- (NSString*) GetRecordFileName
{
    
    NSDate* date = [NSDate date];
    NSDateFormatter* formatter = [[NSDateFormatter alloc] init];
    [formatter setDateFormat:@"yyyy-MM-dd_HH:mm:ss"];
    NSString* strDateTime = [formatter stringFromDate:date];
    
    NSString *strFileName = [NSString stringWithFormat:@"%@_%@.rec", self.m_pCameraObj.m_sDID, strDateTime];
    
    [formatter release];
    
    return strFileName;
    
}

- (NSString*) GetRecordPath: (NSString*)strFileName
{
//    //创建文件管理器
//    NSFileManager *fileManager = [NSFileManager defaultManager];
//    //获取路径
//    //参数NSDocumentDirectory要获取那种路径
//    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
//    NSString *documentsDirectory = [paths objectAtIndex:0];//去处需要的路径
//    
//    NSString *strPath = [documentsDirectory stringByAppendingPathComponent:self.m_pCameraObj.m_sDID];
//    //NSLog(@"strPath: %@", strPath);
//    
//    [fileManager createDirectoryAtPath:strPath withIntermediateDirectories:YES attributes:nil error:nil];
    
    
    //{{-- kongyulu at 20150427
    //设置不备份到iCloud的标识。
    NSString *strPath = [KYLComFunUtil getDefaultFolderInDocumentForDid:self.m_pCameraObj.m_sDID];

    //}}-- kongyulu at 20150427
    
    strPath = [strPath stringByAppendingPathComponent:strFileName];
    
    return strPath;
}




#pragma mark - ScrollView Delegate
- (UIView *)viewForZoomingInScrollView:(UIScrollView *)scrollView {
    UIView *pView = nil;
    pView = self.m_pImageView;

     return pView;
}

- (void)scrollViewDidEndZooming:(UIScrollView *)scrollView
                       withView:(UIView *)view
                        atScale:(CGFloat)scale
{

}

#pragma mark the KYLCameraDelegate call back
- (void) updateImage:(UIImage *) img
{
    if (img && self.m_pImageView) {
        self.m_pImageView.image = img;
    }
}

- (void) updateTheCamearStatus:(NSString *) sStatus
{
    int status = [sStatus intValue];
    switch (status) {
        case CONNECT_STATUS_CONNECTED:
            {
                self.btnStart.hidden = YES;
            }
            break;
            
            case CONNECT_STATUS_DISCONNECT:
            {
                self.btnStart.hidden = NO;
            }
            break;
            
            case CONNECT_STATUS_CONNECTING:
            {
                
            }
            break;
            
            case CONNECT_STATUS_ONLINE:
            {
                
            }
            break;
            
            case CONNECT_STATUS_CONNECT_FAILED:
            case CONNECT_STATUS_INVALID_ID:
            case CONNECT_STATUS_DEVICE_NOT_ONLINE:
            case CONNECT_STATUS_CONNECT_TIMEOUT:
            case CONNECT_STATUS_WRONG_USER_PWD:
            case CONNECT_STATUS_INVALID_REQ:
            case CONNECT_STATUS_EXCEED_MAX_USER:
            case CONNECT_STATUS_INITIALING:
            case CONNECT_STATUS_UNKNOWN:
            {
                
            }
            break;
            
        default:
            break;
    }
}

// video stream
- (NSString *) getThePath
{
    //{{-- kongyulu at 20150427
    //设置不备份到iCloud的标识。
    NSString *strPath  = [KYLComFunUtil getDefaultFolderInDocumentForDid:self.m_pCameraObj.m_sDID];

    //}}-- kongyulu at 20150427
    
    NSDate* date = [NSDate date];
    NSDateFormatter* formatter = [[NSDateFormatter alloc] init];
    [formatter setDateFormat:@"yyyy-MM-dd_HH:mm:ss"];
    NSString* strDateTime = [formatter stringFromDate:date];
    [formatter release];
    
    NSString *strFileName = [NSString stringWithFormat:@"%@_%@.rec", [NSString stringWithFormat:@"%@",self.m_pCameraObj.m_sDID], strDateTime];
    strFileName = [strPath stringByAppendingString:strFileName];
    
    return strFileName;
}

#pragma mark ImageProtocol delegate
- (void) KYLImageProtocol_didReceiveMJPEGImageNotify: (UIImage *)image timestamp: (NSInteger)timestamp DID:(NSString *)did user:(void *) pUser
{
    if(m_bInBackground) return;
    if (image != nil) {
        [self performSelectorOnMainThread:@selector(updateImage:) withObject:image waitUntilDone:NO];
    }
    if (bPlaying == NO)
    {
        bPlaying = YES;
        [self performSelectorOnMainThread:@selector(hideTheHUD) withObject:nil waitUntilDone:NO];
    }
}

- (void) KYLImageProtocol_didReceiveOneVideoFrameYUVNotify: (char*) yuvdata length:(unsigned long)length width: (int) width height:(int)height timestamp:(unsigned int)timestamp DID:(NSString *)did user:(void *) pUser
{
    if(m_bInBackground) return;

}

- (void) KYLImageProtocol_didReceiveOneVideoFrameRGB24Notify: (char*) rgb24data length:(unsigned long)length width: (int) width height:(int)height timestamp:(unsigned int)timestamp DID:(NSString *)did user:(void *) pUser
{
    if(m_bInBackground) return;

    if (bPlaying == NO)
    {
        bPlaying = YES;
        [self performSelectorOnMainThread:@selector(hideTheHUD) withObject:nil waitUntilDone:NO];
    }
    float version = [[[UIDevice currentDevice] systemVersion] floatValue];
    //if ([IpCameraClientAppDelegate is43Version])
    //if (version < 4.5)
    {//4.3.3版本
        
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        UIImage *image=[APICommon RGB888toImage:(Byte*)rgb24data width:width height:height];
        
        if (image != nil) {
            [self performSelectorOnMainThread:@selector(updateImage:) withObject:image waitUntilDone:NO];
        }
        
        [pool drain];
        return;
    }
}

- (void) KYLImageProtocol_didReceiveOneH264VideoFrameWithH264Data: (char*) h264Framedata length: (unsigned long) length type: (int) type timestamp: (NSInteger) timestamp DID:(NSString *)did user:(void *) pUser
{
    
}

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
    //保存设备窗口当前视频的状态
    @synchronized(self)
    {
        //{{-- kongyulu at 20160829
        NSLog(@"%s,进入后台",__FUNCTION__);
        m_bInBackground = YES;
        if(m_pCameraObj)
        {
            //1>stop all talk !!!!
            //2>stop all video!!!!
            //3>stop all audio!!!!
            //4>disconnect all connection
            [m_pCameraObj disconnect];
        }
        //}}-- kongyulu at 20160829
    }
}

//进入前台通知
- (void) didReceiveAppWillInForgroundNotification:(NSNotification *) notification
{
    @synchronized(self)
    {
        //{{-- kongyulu at 20160829
        m_bInBackground = NO;
        NSLog(@"%s,进入前台",__FUNCTION__);

        if(m_pCameraObj)
        {
            [m_pCameraObj connect];
        }
        //}}-- kongyulu at 20160829
    }
}


@end

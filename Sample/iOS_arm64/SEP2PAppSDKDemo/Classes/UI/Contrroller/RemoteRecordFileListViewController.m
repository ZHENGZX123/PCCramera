//
//  RemoteRecordFileListViewController.m
//  P2PCamera
//
//  Created by Tsang on 12-12-14.
//
//

#import "RemoteRecordFileListViewController.h"



#define STR_RECORD_FILE_NAME "STR_RECORD_FILE_NAME"
#define STR_RECORD_FILE_SIZE "STR_RECORD_FILE_SIZE"

#define REFRESH_HEADER_HEIGHT_2 52.0f



@interface RemoteRecordFileListViewController ()
{
    int m_nCurrentYear;
    int m_nCurrentMonth;
    int m_nCurrentDay;
}

@end

@implementation RemoteRecordFileListViewController
@synthesize navigationBar;
@synthesize tableView;
@synthesize m_strDID;
@synthesize m_strName;
@synthesize pmCC;
@synthesize btnShowDateTime;
@synthesize m_pCameraObj;
@synthesize segmentedControl;
@synthesize m_currentSelectedPeriod;
@synthesize m_listHasRecordDays;
@synthesize m_selectDayInCalendar;


- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
        self.m_selectDayInCalendar = nil;
        self.m_pCameraObj = nil;
        m_nCurrentSelectTime = 0;

    }
    return self;
}

- (void) refresh:(id)param
{
    
    selectedSection = -1;
    
    
    m_timer = [NSTimer scheduledTimerWithTimeInterval:10.0 target:self selector:@selector(handleTimer:) userInfo:nil repeats:NO];
    [self ReloadTableView];
    m_bFinished = NO;
    
    [self showCalendar:self.btnShowDateTime];
    
    //m_bShowTheTableHeader = !m_bShowTheTableHeader;
    
    
}

//- (void) getAllRecordDaysInCurrentMonth
//{
//    [m_listHasRecordDays removeAllObjects];
//    
//    NSDate* date = [NSDate date];
//    NSDateFormatter* formatter = [[NSDateFormatter alloc] init];
//
//    [formatter setDateFormat:@"yyyyMM"];
//    NSString* strDateTime2 = [formatter stringFromDate:date];
//    int nYearMonth = [strDateTime2 intValue];
//    [formatter setDateFormat:@"yyyy-MM-dd"];
//    NSString* strDateTime = [formatter stringFromDate:date];
//    NSLog(@"SDcard录像列表，getAllRecordDaysInCurrentMonth: 当前时间：%@", strDateTime);
//    
////    NSCalendar *currentCalendar = [NSCalendar currentCalendar];
////    NSLog(@"the currentCalendar = %@, describe=%@",currentCalendar,[currentCalendar description]);
////    NSCalendar *japanesCalendar = [[[NSCalendar alloc] initWithCalendarIdentifier:NSJapaneseCalendar] autorelease];
////    NSLog(@"the japanesCalendar = %@, describe=%@",japanesCalendar,[japanesCalendar description]);
////    NSCalendar *userCalendar = [[NSLocale currentLocale] objectForKey:NSLocaleCalendar];
////    NSLog(@"the userCalendar = %@, describe=%@",userCalendar,[userCalendar description]);
////    NSTimeZone *timeZoneSystem = [NSTimeZone systemTimeZone];
////    NSLog(@"the system timezone-----%@, describ = %@",timeZoneSystem, [timeZoneSystem description]);
////    NSTimeZone *timeZoneLocal = [NSTimeZone localTimeZone];
////     NSLog(@"the local timezone-----%@, describ = %@",timeZoneLocal, [timeZoneLocal description]);
//    
//
//    NSArray *tempArr = [strDateTime componentsSeparatedByString:@"-"];
//    if (tempArr && [tempArr count] == 3) {
//        NSString *tempStr = nil;
//        tempStr = [tempArr objectAtIndex:0];
//        if (tempStr) {
//            m_nCurrentYear = [tempStr intValue];
//        }
//        tempStr = [tempArr objectAtIndex:1];
//        if (tempStr) {
//            m_nCurrentMonth = [tempStr intValue];
//        }
//        tempStr = [tempArr objectAtIndex:2];
//        if (tempStr) {
//            m_nCurrentDay = [tempStr intValue];
//        }
//    }
//    NSLog(@"当前的日期：%d-%d-%d",m_nCurrentYear,m_nCurrentMonth,m_nCurrentDay);
//    [self getAllRecordDaysInMonth:nYearMonth];
//    
//    [formatter release];
//    formatter = nil;
//    
//}


- (void) startGetTheAllRecordDaysThread
{
        #ifdef __BLOCKS__
            NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        	// No need to retain (just a local variable)
    
        	dispatch_async(dispatch_get_global_queue( DISPATCH_QUEUE_PRIORITY_HIGH, 0), ^{//启动线程
        		// Do a taks in the background
                [self getAllRecordDaysBetweenOneYearInThread];
    
        		dispatch_async(dispatch_get_main_queue(), ^{ //回到主线程
                    //刷新列表
                    //[self ReloadCameraTableView];
                });
        	});
            [pool drain];
        #endif
}


- (void) getCurrentDateFromTheCalender
{
    NSCalendar *calendar = [[NSCalendar alloc] initWithCalendarIdentifier:NSGregorianCalendar];
    NSDateComponents *comps = nil;
    comps = [calendar components:NSYearCalendarUnit|NSMonthCalendarUnit|NSDayCalendarUnit fromDate:[NSDate date]];
    NSInteger nTempYear = comps.year;
    NSInteger nTempMonth = comps.month;
    NSInteger nTempDay = comps.day;
    m_nCurrentYear = (int) nTempYear;
    m_nCurrentMonth = (int) nTempMonth;
    m_nCurrentDay = (int) nTempDay;
    NSLog(@"日历获取时间： year=%d, month=%d,day=%d",m_nCurrentYear,m_nCurrentMonth,m_nCurrentDay);
    [calendar release];
}


- (void) getAllRecordDaysBetweenOneYearInThread
{
    [m_listHasRecordDays removeAllObjects];
    
    NSDateFormatter* formatter = [[NSDateFormatter alloc] init];
    NSCalendar *calendar = [[NSCalendar alloc] initWithCalendarIdentifier:NSGregorianCalendar];
    NSDateComponents *comps = nil;
    comps = [calendar components:NSYearCalendarUnit|NSMonthCalendarUnit|NSDayCalendarUnit fromDate:[NSDate date]];

    NSInteger nTempYear = comps.year;
    NSInteger nTempMonth = comps.month;
    NSInteger nTempDay = comps.day;
    m_nCurrentYear = (int) nTempYear;
    m_nCurrentMonth = (int) nTempMonth;
    m_nCurrentDay = (int) nTempDay;
    NSLog(@"日历获取时间： year=%d, month=%d,day=%d",m_nCurrentYear,m_nCurrentMonth,m_nCurrentDay);

    NSDate *totay = comps.date;
    [formatter setDateFormat:@"yyyy-MM-dd"];
    NSString *strDateTime = [formatter stringFromDate:totay];
    NSLog(@"totay当前时间：%@", strDateTime);
    
    NSDate *newdate2 = [calendar dateByAddingComponents:comps toDate:[NSDate date] options:0];
    [formatter setDateFormat:@"yyyy-MM-dd"];
    strDateTime = [formatter stringFromDate:newdate2];
    NSLog(@"newdate2当前时间：%@", strDateTime);
    
    [formatter release];
    formatter = nil;
    int nCurrentMonthNum = 0;
    int nTempRightMonth = m_nCurrentMonth;
    int nTempRightYear = m_nCurrentYear;
    int nTempYearMonth = 0;
    for (int i=m_nCurrentMonth; i ; i--)
    {
        nCurrentMonthNum++;
        nTempRightMonth = nCurrentMonthNum;
        NSLog(@"i=%d nCurrentNum=%d  %d-%d\n",i,nCurrentMonthNum,nTempRightYear,nTempRightMonth);
        NSString *strYearMonth = [NSString stringWithFormat:@"%d%02d",nTempRightYear,nTempRightMonth];
        nTempYearMonth = [strYearMonth intValue];
        NSLog(@"strYearMonth = %d\n",nTempYearMonth);
        
        [self getAllRecordDaysInMonth:nTempYearMonth];
        usleep(6000);
    }
    if (nCurrentMonthNum < KYL_MAX_SDCARD_RECORD_MONTH_NUM)
    {
        int nTempNum = nCurrentMonthNum;
        nTempRightYear = nTempRightYear-1;
        for (int i=12; i > 12-(KYL_MAX_SDCARD_RECORD_MONTH_NUM-nTempNum); i--)
        {
            nCurrentMonthNum++;
            nTempRightMonth = i;
            NSLog(@"i=%d nCurrentNum=%d  %d-%d\n",i,nCurrentMonthNum,nTempRightYear,nTempRightMonth);
            NSString *strYearMonth = [NSString stringWithFormat:@"%d%02d",nTempRightYear,nTempRightMonth];
            nTempYearMonth = [strYearMonth intValue];
            NSLog(@"strYearMonth = %d\n",nTempYearMonth);
            
            [self getAllRecordDaysInMonth:nTempYearMonth];
            usleep(6000);
        }
    }
}




- (void) btnBackClicked
{
    [self StopTimer];
    self.m_pCameraObj.m_pRemoteRecordDelegate = nil;
    self.m_pCameraObj.m_pDeviceStatusChangedDelegate = nil;
    self.m_pCameraObj = nil;
}

- (void)handleTimer:(id)param
{
    [self StopTimer];
    [self hideLoadingIndicator];
    m_bFinished = YES;
}

- (void) StopTimer
{
    [m_timerLock lock];
    if (m_timer != nil) {
        [m_timer invalidate];
        m_timer = nil;
    }
    [m_timerLock unlock];
}


- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    m_bShowTheTableHeader = NO;
    selectedSection = -1;
    
    self.navigationBar.delegate = self;
    float version = [[[UIDevice currentDevice] systemVersion] floatValue];

    m_timerLock = [[NSCondition alloc] init];
    m_bFinished = NO;
    m_RecordFileList = [[NSMutableArray alloc] init];
    m_listHasRecordDays = [[NSMutableArray alloc] init];
    //[self hideLoadingIndicator];
    m_timer = [NSTimer scheduledTimerWithTimeInterval:10.0 target:self selector:@selector(handleTimer:) userInfo:nil repeats:NO];
    
    CGRect rect = self.view.frame;
    int nWidth = rect.size.width;
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone)//iphone
    {
    }
    else//ipad
    {
        nWidth = 768;
    }
    self.btnShowDateTime.frame = CGRectMake(nWidth - 60, rect.origin.y, 30, 30);
    self.btnShowDateTime.hidden = YES;
    
    
    NSDate* date = [NSDate date];
    NSDateFormatter* formatter = [[NSDateFormatter alloc] init];
    [formatter setDateFormat:@"yyyyMMdd"];
    NSString* strDateTime = [formatter stringFromDate:date];
    int nYearMonthDay = [strDateTime intValue];

    //begin add by kongyulu at 20140325
    [formatter release];
    m_nCurrentSelectTime = nYearMonthDay;
    m_nSelectedSegmentIndex = 1;
    
    [self getCurrentDateFromTheCalender];
    strDateTime = [NSString stringWithFormat:@"%d%02d%02d",m_nCurrentYear,m_nCurrentMonth,m_nCurrentDay];
    int nTempCurrentDateTime = [strDateTime intValue];
    m_nCurrentSelectTime = nTempCurrentDateTime;
    
    m_bShowTheTableHeader = false;
    
    [self showLoadingIndicator];
    
    
    //获取半年的录像日期
    [self startGetTheAllRecordDaysThread];
    
    self.m_pCameraObj.m_pDeviceStatusChangedDelegate = self;
    [self getAllTheScheduleRecordBySelectDay];
    

    [self performSelector:@selector(ReloadTableView) withObject:nil afterDelay:1];

    // addd the table view single tap
//    UITapGestureRecognizer* singleTapRecognizer;
//    singleTapRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleSingleTapFrom:)];
//    singleTapRecognizer.numberOfTapsRequired = 1; // 单击
//    [self.view addGestureRecognizer:singleTapRecognizer];
//    [singleTapRecognizer release];
//    singleTapRecognizer = nil;
    

}

- (void) addGesture:(UIView *) pView
{
    UITapGestureRecognizer* singleTapRecognizer;
    singleTapRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleSingleTapFrom:)];
    singleTapRecognizer.numberOfTapsRequired = 1; // 单击
    [pView addGestureRecognizer:singleTapRecognizer];
    
    // 双击的 Recognizer
    UITapGestureRecognizer* doubleTapRecognizer;
    doubleTapRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleDoubleTapFrom:)];
    doubleTapRecognizer.numberOfTapsRequired = 2; // 双击
    [pView addGestureRecognizer:doubleTapRecognizer];
    
    // 关键在这一行，如果双击确定偵測失败才會触发单击
    [singleTapRecognizer requireGestureRecognizerToFail:doubleTapRecognizer];
    
    // 向上擦碰
    UISwipeGestureRecognizer *oneFingerSwipeUp =
    [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerSwipeUp:)] ;
    [oneFingerSwipeUp setDirection:UISwipeGestureRecognizerDirectionUp];
    [pView  addGestureRecognizer:oneFingerSwipeUp];
    
    
    // 向下擦碰
    UISwipeGestureRecognizer *oneFingerSwipeDown =
    [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerSwipeDown:)] ;
    [oneFingerSwipeDown setDirection:UISwipeGestureRecognizerDirectionDown];
    [pView  addGestureRecognizer:oneFingerSwipeDown];
    
    // 向左擦碰
    UISwipeGestureRecognizer *oneFingerSwipeLeft =
    [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerSwipeLeft:)] ;
    [oneFingerSwipeLeft setDirection:UISwipeGestureRecognizerDirectionLeft];
    [pView  addGestureRecognizer:oneFingerSwipeLeft];
    
    
    // 向右擦碰
    UISwipeGestureRecognizer *oneFingerSwipeRight =
    [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerSwipeRight:)]   ;
    [oneFingerSwipeRight setDirection:UISwipeGestureRecognizerDirectionRight];
    [pView  addGestureRecognizer:oneFingerSwipeRight];
    
    //长按手势
    UILongPressGestureRecognizer *longPressGesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(longPressGestureDid:)];
    longPressGesture.minimumPressDuration = 2;
    [pView addGestureRecognizer:longPressGesture];
    
    
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
}

// 单击
- (void)handleSingleTapFrom:(UITapGestureRecognizer*)recognizer {
    
    if (recognizer.state == UIGestureRecognizerStateEnded) {
        [self hideTheCalendarViewWhenTagTableView];
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
    
    NSLog(@"向上滑动");
    if (recognizer.state == UIGestureRecognizerStateEnded) {
        
    }
}

// 向下滑动
- (void)oneFingerSwipeDown:(UISwipeGestureRecognizer *)recognizer
{
    
    NSLog(@"向下滑动");
    if (recognizer.state == UIGestureRecognizerStateEnded) {
    }
    
}

//向左滑动
- (void)oneFingerSwipeLeft:(UISwipeGestureRecognizer *)recognizer
{
    
    NSLog(@"向左滑动");
    if (recognizer.state == UIGestureRecognizerStateEnded) {
        
    }
    
}

// 向右滑动
- (void)oneFingerSwipeRight:(UISwipeGestureRecognizer *)recognizer
{
    NSLog(@"向右滑动");
    if (recognizer.state == UIGestureRecognizerStateEnded) {
    }
    
}



- (void)doPinch:(UIPinchGestureRecognizer *)pinch
{
    if (pinch.state == UIGestureRecognizerStateEnded) {
        
    }
}




- (void) viewDidUnload
{
    [super viewDidUnload];
    if (m_RecordFileList != nil) {
        [m_RecordFileList release];
        m_RecordFileList = nil;
    }
}

- (void) viewWillDisappear:(BOOL)animated
{
    [self StopTimer];
    [super viewWillDisappear:animated];
}

- (void)showLoadingIndicator
{
    NSString *strTitle = m_strName;
    UINavigationItem *back = [[UINavigationItem alloc] initWithTitle:NSLocalizedString(@"Back", nil)];
    UINavigationItem *item = [[UINavigationItem alloc] initWithTitle:strTitle];
    
    //创建一个右边按钮
    UIActivityIndicatorView *indicator =
    [[[UIActivityIndicatorView alloc]
      initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhite]
     autorelease];
	indicator.frame = CGRectMake(0, 0, 24, 24);
	[indicator startAnimating];
	UIBarButtonItem *progress =
    [[[UIBarButtonItem alloc] initWithCustomView:indicator] autorelease];
    item.rightBarButtonItem = progress;
    NSArray *array = [NSArray arrayWithObjects:back, item, nil];
    [self.navigationBar setItems:array];
    [item release];
    [back release];
}

- (void)hideLoadingIndicator
{
    NSString *strTitle = m_strName;
    UINavigationItem *back = [[UINavigationItem alloc] initWithTitle:NSLocalizedString(@"Back", nil)];
    UINavigationItem *item = [[UINavigationItem alloc] initWithTitle:strTitle];
    
    //创建一个右边按钮
	UIBarButtonItem *refreshButton =
    [[UIBarButtonItem alloc]
     initWithBarButtonSystemItem:UIBarButtonSystemItemRefresh
     target:self
     action:@selector(refresh:)];
    item.rightBarButtonItem = refreshButton;
    NSArray *array = [NSArray arrayWithObjects:back, item, nil];
    [self.navigationBar setItems:array];
    [refreshButton release];
    [item release];
    [back release];
    
}


- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)dealloc
{
    self.m_selectDayInCalendar = nil;
    self.m_listHasRecordDays = nil;
    self.m_pCameraObj.m_pRemoteRecordDelegate = nil;
    self.m_pCameraObj.m_pDeviceStatusChangedDelegate = nil;
    self.m_pCameraObj = nil;
    self.navigationBar = nil;
    self.tableView = nil;
    self.m_strDID = nil;
    self.m_strName = nil;
    if (m_RecordFileList != nil) {
        [m_RecordFileList release];
        m_RecordFileList = nil;
    }
    self.pmCC = nil;

    [super dealloc];
}

#pragma mark -
#pragma mark TableViewDelegate





- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section{
    if (m_bShowTheTableHeader) {
        return HEADER_HEIGHT;
    }
    return 0;
}

- (NSString*) secTimeToString: (NSInteger) secTime
{
    int hour = (int) secTime / 60 / 60;
    int minute = (int)(secTime - hour * 3600 ) / 60;
    int sec = (int)(secTime - hour * 3600 - minute * 60) ;
    NSString *strTime = [NSString stringWithFormat:@"%02d:%02d:%02d", hour, minute, sec];
    return strTime;
}

- (void)segmentedControlSelected:(id)sender
{
    m_nSelectedSegmentIndex = segmentedControl.selectedSegmentIndex;
    //[self startGetTheRecordFileByCondition];
    
}


-(UIView*)tableView:(UITableView*)tableView viewForHeaderInSection:(NSInteger)section {
    
    if (m_bShowTheTableHeader) {
        
        UIView *pViewHead = [[[UIView alloc] initWithFrame:CGRectMake(0.0, 0.0, self.tableView.bounds.size.width, HEADER_HEIGHT)] autorelease];
        segmentedControl = [[UISegmentedControl alloc] init];
        segmentedControl.segmentedControlStyle = UISegmentedControlStyleBar;
        //self.navigationItem.titleView = segmentedControl;
        [segmentedControl addTarget:self action:@selector(segmentedControlSelected:) forControlEvents:UIControlEventValueChanged];
        [segmentedControl insertSegmentWithTitle:NSLocalizedString(@"alarm record", nil) atIndex:segmentedControl.numberOfSegments animated:NO];
        [segmentedControl insertSegmentWithTitle:NSLocalizedString(@"schedule record", nil) atIndex:segmentedControl.numberOfSegments animated:NO];
        [segmentedControl insertSegmentWithTitle:NSLocalizedString(@"all record", nil) atIndex:segmentedControl.numberOfSegments animated:NO];

        [segmentedControl sizeToFit];
        [pViewHead addSubview:segmentedControl];
        pViewHead.backgroundColor = [UIColor whiteColor];
        segmentedControl.center = pViewHead.center;
        if(segmentedControl)
        {
            [segmentedControl setSelectedSegmentIndex:m_nSelectedSegmentIndex];
        }
        return pViewHead;
        
    }
    else
    {
        return nil;
    }
    return nil;

}





- (NSInteger)tableView:(UITableView *)aTableView numberOfRowsInSection:(NSInteger)section
{
    return [m_RecordFileList count];
    
}

- (UITableViewCell *) tableView:(UITableView *)aTableView cellForRowAtIndexPath:(NSIndexPath *)anIndexPath
{
    //NSLog(@"cellForRowAtIndexPath");
    
    NSString *cellIdentifier = @"RemoteRecordFileListCell";
    //当状态为显示当前的设备列表信息时
    SDCardRecordInfoCell *cell =  (SDCardRecordInfoCell*)[aTableView dequeueReusableCellWithIdentifier:cellIdentifier];
    if (cell == nil)
    {
        NSArray *nib = [[NSBundle mainBundle] loadNibNamed:@"SDCardRecordInfoCell" owner:self options:nil];
        cell = [nib objectAtIndex:0];
    }
    
    cell.l_fileTimeLenTitle.text = @"";
    cell.l_fileNameTitle.text = @"";
    cell.l_fileSizeTitle.text = @"";
    cell.l_fileNameValue.text = @"";
    cell.l_fileSizeValue.text = @"";
    cell.l_fileTimeLenValue.text = @"";
    NSInteger row = [anIndexPath row];
    
    if (m_RecordFileList)
    {
        NSInteger count = [m_RecordFileList count];
        if (count > row)
        {
            KYLRecordFileInfo *oneRecordInfo = [m_RecordFileList objectAtIndex:row];
            if (oneRecordInfo) {
                NSString *fileName = [oneRecordInfo.m_sFilePath lastPathComponent];
                double nFileSize = oneRecordInfo.m_nFileSize_KB;
                NSInteger nTimeLength = oneRecordInfo.m_nTimeLength_Sec;
                NSString *strTimeLength = [self secTimeToString:nTimeLength];
                nFileSize = nFileSize/1024.000 ;
                
                cell.l_fileNameValue.text = fileName;
                cell.l_fileSizeValue.text = [NSString stringWithFormat:@"%0.2f MB",nFileSize];
                cell.l_fileTimeLenValue.text = [NSString stringWithFormat:@"%@",strTimeLength];
            }
            cell.l_fileTimeLenTitle.text = NSLocalizedString(@"Record Length:", nil);
            cell.l_fileNameTitle.text = NSLocalizedString(@"File Name:", nil);
            cell.l_fileSizeTitle.text = NSLocalizedString(@"File Size:", nil);
            
            //cell.selectionStyle = UITableViewCellSelectionStyleNone;
            cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
        }
    }
    if (row == m_nCurrentSelectedIndex) {
        cell.accessoryType = UITableViewCellAccessoryCheckmark;
        [cell setSelected:YES];
    }
                
    return cell;
    
}

- (CGFloat)tableView:(UITableView*)tableview heightForRowAtIndexPath:(NSIndexPath*)indexpath
{
    return 60;
}

- (void)tableView:(UITableView *)aTableView didSelectRowAtIndexPath:(NSIndexPath *)anIndexPath
{
    [aTableView deselectRowAtIndexPath:anIndexPath animated:YES];
    
    NSInteger row = anIndexPath.row;
    if (row > [m_RecordFileList count] - 1 ) {
        return;
    }
    RemotePlaybackViewController *remotePlaybackViewController = [[RemotePlaybackViewController alloc] init];
    KYLRecordFileInfo *oneRecordInfo = [m_RecordFileList objectAtIndex:row];
    if(oneRecordInfo == nil) return;
    remotePlaybackViewController.m_strName = m_strName;
    remotePlaybackViewController.m_strFileName = [oneRecordInfo.m_sFilePath lastPathComponent];
    //remotePlaybackViewController.m_pPPPPMgnt = m_pPPPPChannelMgt;
    remotePlaybackViewController.strDID = m_strDID;
    remotePlaybackViewController.m_pCameraObj = self.m_pCameraObj;
    remotePlaybackViewController.m_pRecordFileInfo = oneRecordInfo;
    remotePlaybackViewController.m_AllRecordFileList = m_RecordFileList;
    remotePlaybackViewController.m_nCurrentSelectedVideoListIndex = row;
    remotePlaybackViewController.m_strRecordFilePath = oneRecordInfo.m_sFilePath;
    remotePlaybackViewController.m_nRecordFileTotalTimeLength = oneRecordInfo.m_nTimeLength_Sec;
    remotePlaybackViewController.m_nRecordFileCurrentPlayTimePos = 0;
    remotePlaybackViewController.m_delegate = self;
    [remotePlaybackViewController setModalTransitionStyle:UIModalTransitionStyleFlipHorizontal];

    [self.navigationController pushViewController:remotePlaybackViewController animated:YES];

    [remotePlaybackViewController release];
    
}



- (void) clearFileList
{
    [m_RecordFileList removeAllObjects];
}

#pragma mark- camera action

- (int) getAllTheAlarmRecordBySelectDay
{
    int nRet = 0;
    if (m_pCameraObj) {
        self.m_pCameraObj.m_pRemoteRecordDelegate = self;
        nRet = [m_pCameraObj getAllRemoteAlarmRecordFilesByDayWithYearMonthDay:m_nCurrentSelectTime];
    }
    return nRet;
}

- (int) getAllTheScheduleRecordBySelectDay
{
    int nRet = 0;
    if (m_pCameraObj) {
        self.m_pCameraObj.m_pRemoteRecordDelegate = self;
        nRet = [m_pCameraObj getAllRemoteAllScheduleRecordFilesByDayWithYearMonthDay:m_nCurrentSelectTime];
    }
    return nRet;
}

- (int) getAllTheTypeRecordBySelectDay
{
    int nRet = 0;
    if (m_pCameraObj) {
        self.m_pCameraObj.m_pRemoteRecordDelegate = self;
        nRet = [m_pCameraObj getAllRemoteAllTypeRecordFilesByDayWithYearMonthDay:m_nCurrentSelectTime];
    }
    return nRet;
}

- (int) getAllRemoteRecordFilesByDayWithYear:(int) nYear month:(int) nMonth day:(int) nDay  recordType:(int) nRecordType beginIndex:(int) nBegin endIndex:(int) nEnd
{
    int nRet = 0;
    if (m_pCameraObj) {
        self.m_pCameraObj.m_pRemoteRecordDelegate = self;
        nRet = [m_pCameraObj getAllRemoteRecordFilesByDayWithYear:nYear month:nMonth day:nDay recordType:nRecordType beginIndex:nBegin endIndex:nEnd];
    }
    return nRet;
}


- (int) getAlarmRecordBySelectDayMaxItemsPerTime
{
    int nRet = 0;
    int nType = 2;
    int nBegin = 0;
    int nEnd = 20;
    if (m_pCameraObj) {
        self.m_pCameraObj.m_pRemoteRecordDelegate = self;
        nRet = [m_pCameraObj getAllRemoteRecordFilesByDayWithYearMonthDay:m_nCurrentSelectTime recordType:nType beginIndex:nBegin endIndex:nEnd];
    }
    return nRet;
}

- (int) getScheduleRecordBySelectDayMaxItemsPerTime
{
    int nRet = 0;
    int nType = 1;
    int nBegin = 0;
    int nEnd = 20;
    if (m_pCameraObj) {
        self.m_pCameraObj.m_pRemoteRecordDelegate = self;
        nRet = [m_pCameraObj getAllRemoteRecordFilesByDayWithYearMonthDay:m_nCurrentSelectTime recordType:nType beginIndex:nBegin endIndex:nEnd];
    }
    return nRet;
}

- (int) getAlltypeRecordBySelectDayMaxItemsPerTime
{
    int nRet = 0;
    int nType = 3;
    int nBegin = 0;
    int nEnd = 20;
    if (m_pCameraObj) {
        self.m_pCameraObj.m_pRemoteRecordDelegate = self;
        nRet = [m_pCameraObj getAllRemoteRecordFilesByDayWithYearMonthDay:m_nCurrentSelectTime recordType:nType beginIndex:nBegin endIndex:nEnd];
    }
    return nRet;
}

- (int) getAllRecordDaysInMonth:(int) nYearMonth
{
    int nRet=0;
    if (m_pCameraObj) {
        self.m_pCameraObj.m_pRemoteRecordDelegate = self;
        nRet = [m_pCameraObj getAllRemoteRecordDays:nYearMonth];
    }
    return nRet;
}


#pragma mark -
#pragma mark performOnMainThread

- (void) ReloadTableView
{
    if (nil == self.tableView) {
        return;
    }
    if([m_RecordFileList count] > 0)
    {
        self.tableView.separatorStyle = UITableViewCellSeparatorStyleSingleLine;
    }
    else{
        self.tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
    }
    [self.tableView reloadData];
}


#pragma mark -
#pragma mark navigationBarDelegate

- (BOOL) navigationBar:(UINavigationBar *)navigationBar shouldPopItem:(UINavigationItem *)item
{
    NSLog(@"pop the remoterecordFileListViewControl out");
    [self btnBackClicked];
    [self.navigationController popViewControllerAnimated:YES];
    return NO;
}

#pragma mark -
#pragma mark sdcardfilelistresult

- (void) SDCardRecordFileSearchResult:(NSString *)strFileName fileSize:(NSInteger)fileSize bEnd:(BOOL)bEnd
{
    //NSLog(@"strFileName: %@, fileSize: %d, bEnd: %d", strFileName, fileSize, bEnd);
    
    [self performSelectorOnMainThread:@selector(StopTimer) withObject:nil waitUntilDone:YES];
    
    if (m_bFinished == YES) {
        return;
    }
    
    if (bEnd == 1) {
        [self performSelectorOnMainThread:@selector(hideLoadingIndicator) withObject:nil waitUntilDone:YES];
        [self performSelectorOnMainThread:@selector(ReloadTableView) withObject:nil waitUntilDone:YES];
    }
}

#pragma  mark-
#pragma mark- SDcardScheduleDelegate
-(void)sdcardScheduleParams:(NSString *)did Tota:(int)total RemainCap:(int)remain SD_status:(int)status Cover:(int)cover_enable TimeLength:(int)timeLength FixedTimeRecord:(int)ftr_enable{

}
-(void)sdcardScheduleParams:(NSString *)did Tota:(int)total RemainCap:(int)remain SD_status:(int)status Cover:(int)cover_enable TimeLength:(int)timeLength FixedTimeRecord:(int)ftr_enable RecordSize:(int)recordSize{

}

- (IBAction)showCalendar:(id)sender
{

    CGSize calendarSize = CGSizeMake(300, 300);
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone)//iphone
    {
        
    }
    else//ipad
    {
        calendarSize = CGSizeMake(500, 500);
    }
    self.pmCC = [[[PMCalendarController alloc] initWithSize:calendarSize] autorelease];
    pmCC.delegate = self;
    pmCC.mondayFirstDayOfWeek = YES;
    [pmCC presentCalendarFromView:sender
         permittedArrowDirections:PMCalendarArrowDirectionDown
                         animated:YES];
    
    /*    [pmCC presentCalendarFromRect:[sender frame]
     inView:[sender superview]
     permittedArrowDirections:PMCalendarArrowDirectionAny
     animated:YES];*/
    
    //[self calendarController:pmCC didChangePeriod:pmCC.period];
    //pmCC.period = self.m_currentSelectedPeriod;
    
}



#pragma mark PMCalendarControllerDelegate methods

- (void)calendarController:(PMCalendarController *)calendarController didChangePeriod:(PMPeriod *)newPeriod
{
    NSString *strDate = [NSString stringWithFormat:@"%@"
                         , [newPeriod.startDate dateStringWithFormat:@"yyyyMMdd"]];
    NSLog(@"the selected date is %@",strDate);
    m_nCurrentSelectTime = [strDate intValue];
    
    NSString *strDate2 = [NSString stringWithFormat:@"%@ - %@"
                         , [newPeriod.startDate dateStringWithFormat:@"yyyy-MM-dd"]
                         , [newPeriod.endDate dateStringWithFormat:@"yyyy-MM-dd"]];
    NSLog(@"the selected date is %@",strDate2);
    
    //self.m_currentSelectedPeriod = newPeriod;
    
    //[self startGetTheRecordFileByCondition];
    
    //calendarController.view.hidden = YES;
    //[self performSelector:@selector(hideTheCalendarView) withObject:nil afterDelay:0.2];
    
}

- (void) hideTheCalendarView
{
    [self showTheCalendarView:false];
}

- (void) hideTheCalendarViewWhenTagTableView
{
    if (pmCC) {
        [self calendarController:pmCC didChangePeriod:pmCC.period];
    }
}

- (void) showTheCalendarView:(BOOL) bShow
{
    if (self.pmCC)
    {
        self.pmCC.view.hidden = !bShow;
    }
}

/**
 * Called on the delegate when the celendar controller will dismiss the popover.
 * Return NO to prevent the dismissal.
 *
 * This method is not called when -dismissCalendarAnimated: is called directly.
 */
- (BOOL)calendarControllerShouldDismissCalendar:(PMCalendarController *)calendarController
{
    return YES;
}

/**
 * Called on the delegate right after calendar controller removes itself from a superview.
 */
- (void)calendarControllerDidDismissCalendar:(PMCalendarController *)calendarController
{
    [self startGetTheRecordFileByCondition];
}

- (void) startGetTheRecordFileByCondition
{
    //clear the list
    [m_RecordFileList removeAllObjects];
    
    switch (m_nSelectedSegmentIndex) {
        case 0:// alaram record
        {
            [self getAllTheScheduleRecordBySelectDay];
            
        }
            break;
        case 1:// schedule record
        {
            [self getAllTheAlarmRecordBySelectDay];
        }
            break;
        case 2:// all record
        {
            [self getAllTheTypeRecordBySelectDay];
        }
            break;
            
        default:
            break;
    }
    [self showLoadingIndicator];
    [self ReloadTableView];
}


- (void) getTheRecordFileFromHasRecordDay
{
    for(int i=0; i< [m_listHasRecordDays count] ; i++)
    {
        NSDictionary *dic = [m_listHasRecordDays objectAtIndex:i];
        NSString *sType = [dic objectForKey:@"type"];
        NSString *sYear = [dic objectForKey:@"year"];
        NSString *sMonth = [dic objectForKey:@"month"];
        NSString *sDay = [dic objectForKey:@"day"];
        
        int nType = [sType intValue];
        int nYear = [sYear intValue];
        int nMonth = [sMonth intValue];
        int nDay = [sDay intValue];
        
        [self getAllRemoteRecordFilesByDayWithYear:nYear month:nMonth day:nDay recordType:nType beginIndex:0 endIndex:0];
    }
}

- (void) startGetAllRecordFile
{
#ifdef __BLOCKS__
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    // No need to retain (just a local variable)
    
    dispatch_async(dispatch_get_global_queue( DISPATCH_QUEUE_PRIORITY_HIGH, 0), ^{//启动线程
        // Do a taks in the background
        [self getTheRecordFileFromHasRecordDay];
        
        dispatch_async(dispatch_get_main_queue(), ^{ //回到主线程
            //刷新列表
            [self ReloadTableView];
        });
    });
    [pool drain];
#endif
}


#pragma mark - KYLRemoteRecordPlayProtocol

//remote record file playback
- (void) KYLRemoteRecordPlayProtocol_didReceiveTheGetRemoteRecordDaysByMonth:(NSString *) strDID recordType:(int) nRecType yearMonth:(int) nYearMonth theRecordedDays:(NSArray *) dayList reserve:(NSString *) sReserve
{
    NSLog(@"KYLRemoteRecordPlayProtocol recType=%d, yearMonth=%d,",nRecType,nYearMonth);
    if(dayList && [dayList count] > 0)
    {
        for(int i=0; i<[dayList count]; i++)
        {
            
            NSString *strYearMonth = [NSString stringWithFormat:@"%d",nYearMonth];
            if (strYearMonth && [strYearMonth length]>=6) {
                //int nTempYear = [[strYearMonth substringWithRange:NSMakeRange(0, 4)] intValue];
                NSString *strYear = [strYearMonth substringWithRange:NSMakeRange(0, 4)];
                NSString *strMonth = [strYearMonth substringWithRange:NSMakeRange(4, 2)];
                NSString *strDay = [dayList objectAtIndex:i];
                NSLog(@"Did=%@, 有SDcard录像的日期为：%@-%@-%@",strDID,strYear,strMonth,strDay);
                NSString *strType = [NSString stringWithFormat:@"%d",nRecType];
                NSDictionary *dic = [[NSDictionary alloc] initWithObjectsAndKeys:strYear,@"year",
                                     strMonth,@"month",
                                     strDay,@"day",
                                     strType,@"type",nil];
                [m_listHasRecordDays addObject:dic];
                [dic release];
            }
        }
        [self performSelectorOnMainThread:@selector(startGetAllRecordFile) withObject:nil waitUntilDone:NO];
    }
}


- (void) KYLRemoteRecordPlayProtocol_didReceiveTheGetRemoteRecordFilesInOneDay:(NSString *) strDID
                                                                        result:(int) nResult
                                                                  fileTotalNum:(int) nFileTotalNum
                                                             beginNofoThisTime:(int) nBeginNoOfThisTime
                                                               endNoOfThisTime:(int) nEndNoOfThisTime
                                                                theRecordFiles:(NSArray *) allRecordFilesByDayList
{
    
    NSLog(@"KYLRemoteRecordPlayProtocol_didReceiveTheGetRemoteRecordFilesInOneDay get all record file info:  nResult:%d nBeginNoOfThisTime:%d, nEndNoOfThisTime=%d, nFileTotalNum=%d",nResult,nBeginNoOfThisTime,nEndNoOfThisTime,nFileTotalNum);
    BOOL bEnd;
    for (int i = 0; i < allRecordFilesByDayList.count; i++)
    {
        KYLRecordFileInfo *oneRecordInfo = [allRecordFilesByDayList objectAtIndex:i];
        [m_RecordFileList addObject:oneRecordInfo];
    }
    
    bEnd = false;
    if (nEndNoOfThisTime+1 == nFileTotalNum) {
        bEnd = true;
    }
    if (bEnd) {
        [self performSelectorOnMainThread:@selector(hideLoadingIndicator) withObject:nil waitUntilDone:YES];
        [self performSelectorOnMainThread:@selector(ReloadTableView) withObject:nil waitUntilDone:YES];
    }
}

- (void) KYLRemoteRecordPlayProtocol_didReceiveTheGetOneRemoteRecordFileInOneDay:(NSString *) strDID
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

- (void) KYLRemoteRecordPlayProtocol_didReceiveTheStartPlayRemoteRecordFile:(NSString *) strDID
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

- (void) KYLRemoteRecordPlayProtocol_didReceiveTheStopPlayRemoteRecordFile:(NSString *) strDID result:(int) nResult filePath:(NSString *) sFilePath reserve1:(NSString *) sReserve1
{
    
}






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
                //[self performSelectorOnMainThread:@selector(showTheDeviceDisconnectInfo) withObject:nil waitUntilDone:NO];
            }
                break;
            case CONNECT_STATUS_CONNECTED:
            {
                //[self performSelectorOnMainThread:@selector(showTheDeviceReconnectInfo) withObject:nil waitUntilDone:NO];
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


#pragma mark - RemotePlaybackViewControl delegate
-  (void) didTheRmotePlaybackFinished:(NSString *) did currentIndex:(NSInteger) nIndex
{
    m_nCurrentSelectedIndex = nIndex;
    [self.tableView reloadData];
}



@end

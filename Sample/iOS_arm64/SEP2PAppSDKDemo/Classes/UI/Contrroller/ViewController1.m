//
//  ViewController1.m
//  LeftRightSlider
//
//  Created by Zhao Yiqi on 13-11-29.
//  Copyright (c) 2013年 heroims. All rights reserved.
//

#import "ViewController1.h"
#import "KYLDeviceInfo.h"
#import "KYLSearchTool.h"
#import "MBProgressHUD.h"

@interface ViewController1 ()
{
    KYLSearchTool *m_pSearchTool;
}

@end

@implementation ViewController1
@synthesize m_ptableView;
@synthesize m_listAllSearchDevice;
@synthesize btnSearch;
@synthesize m_pCamera;

- (void) dealloc
{
    self.m_pCamera = nil;
    self.btnSearch = nil;
    if (m_pSearchTool) {
        m_pSearchTool.delegate = nil;
        [m_pSearchTool release];
        m_pSearchTool = nil;
    }
    self.m_ptableView = nil;
    self.m_listAllSearchDevice = nil;
    [super dealloc];
}

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.view.backgroundColor=[UIColor redColor];
    
    [self initUI];
    
    [self initData];
	// Do any additional setup after loading the view.
    [self startSearch];
}

- (void) initData
{
    m_pSearchTool = nil;
    self.m_listAllSearchDevice = [[[NSMutableArray alloc] initWithCapacity:10] autorelease];
    if (nil == m_pSearchTool)
    {
        m_pSearchTool = [[KYLSearchTool alloc] init];
        m_pSearchTool.delegate = self;
    }
}

- (void) initUI
{
    [self initButtons];
    [self initTableView];
}

- (void) initTableView
{
    CGRect mainRect = self.view.frame;
    UITableView *tableView = [[UITableView alloc] initWithFrame:CGRectMake(mainRect.origin.x, mainRect.origin.y + 100, mainRect.size.width, mainRect.size.height - 100)];
    tableView.delegate = self;
    tableView.dataSource = self;
    [self.view addSubview:tableView];
    [tableView release];
}

- (void) initButtons
{
    CGRect mainRect = self.view.frame;
    UIButton *btnNext=[UIButton buttonWithType:UIButtonTypeCustom];
    btnNext.layer.borderColor=[[UIColor whiteColor] CGColor];
    btnNext.layer.borderWidth=2;
    [btnNext setTitle:@"Next" forState:UIControlStateNormal];
    [btnNext setFrame:CGRectMake(mainRect.size.width - 100, mainRect.origin.y + 10, 80, 50)];
    [btnNext addTarget:self action:@selector(btnNextClick:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:btnNext];
    
    
    UIButton *btnSearch1=[UIButton buttonWithType:UIButtonTypeCustom];
    self.btnSearch = btnSearch1;
    btnSearch1.layer.borderColor=[[UIColor whiteColor] CGColor];
    btnSearch1.layer.borderWidth=2;
    btnSearch1.titleLabel.font = [UIFont systemFontOfSize:10];
    [btnSearch1 setTitle:@"Search" forState:UIControlStateNormal];
    [btnSearch1 setFrame:CGRectMake(mainRect.origin.x + 120, mainRect.origin.y + 10, 60, 50)];
    [btnSearch1 addTarget:self action:@selector(btnSearchClick:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:btnSearch1];
    
    UIButton *btnSearchStop=[UIButton buttonWithType:UIButtonTypeCustom];
    btnSearchStop.layer.borderColor=[[UIColor whiteColor] CGColor];
    btnSearchStop.layer.borderWidth=2;
     btnSearchStop.titleLabel.font = [UIFont systemFontOfSize:10];
    [btnSearchStop setTitle:@"Stop" forState:UIControlStateNormal];
    [btnSearchStop setFrame:CGRectMake(mainRect.origin.x + 200, mainRect.origin.y + 10, 60, 50)];
    [btnSearchStop addTarget:self action:@selector(btnStopSearchClick:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:btnSearchStop];
    
    UIButton *btnPop=[UIButton buttonWithType:UIButtonTypeCustom];
    btnPop.layer.borderColor=[[UIColor whiteColor] CGColor];
    btnPop.layer.borderWidth=2;
    [btnPop setTitle:@"Back" forState:UIControlStateNormal];
    [btnPop setFrame:CGRectMake(mainRect.origin.x + 10, mainRect.origin.y + 10, 80, 50)];
    [btnPop addTarget:self action:@selector(btnPopClick:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:btnPop];

}

- (void) showTheHUD
{
    MBProgressHUD *hud = [MBProgressHUD showHUDAddedTo:self.view animated:YES];
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
    [MBProgressHUD hideHUDForView:self.view animated:YES];
}


-(void)btnPopClick:(id)sender
{
    [self.navigationController popViewControllerAnimated:YES];
}

-(void)btnNextClick:(id)sender
{
    [self.navigationController pushViewController:[[[ViewController2 alloc] init] autorelease] animated:YES] ;
}

-(void)btnSearchClick:(id)sender
{
    [self startSearch];
}

-(void)btnStopSearchClick:(id)sender
{
    [self stopSearch];
}

- (BOOL) startSearch
{
    [self showTheHUD];
    [self performSelector:@selector(refreshTheTableView) withObject:nil afterDelay:6];
    BOOL m_bResult = false;
    if (m_pSearchTool) {
        // clear all data
        if (self.m_listAllSearchDevice) {
            [self.m_listAllSearchDevice removeAllObjects];
        }
        [m_pSearchTool startSearch];
    }
//    if (self.m_pCamera) {
//        self.m_pCamera.searchDelegate = self;
//        // clear all data
//        if (self.m_listAllSearchDevice) {
//            [self.m_listAllSearchDevice removeAllObjects];
//        }
//        [self.m_pCamera startSearch];
//    }
    return m_bResult;
}

- (BOOL) stopSearch
{
    [self hideTheHUD];
    BOOL m_bResult = false;
    if (m_pSearchTool) {
        [m_pSearchTool stopSearch];
    }
//    if (self.m_pCamera) {
//        [self.m_pCamera stopSearch];
//        self.m_pCamera.searchDelegate = nil;
//    }
    return m_bResult;
}



- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark -
#pragma mark TableViewDelegate

- (NSInteger)numberOfSectionsInTableView:(UITableView *)aTableView
{
	int row = 1;
    
	//NSLog( @"numberOfSectionsInTableView : %d", row );
	
    return row;
    
}

- (NSInteger)tableView:(UITableView *)aTableView numberOfRowsInSection:(NSInteger)section
{
    return [self.m_listAllSearchDevice count];
}

- (UITableViewCell *)tableView:(UITableView *)aTableView cellForRowAtIndexPath:(NSIndexPath *)anIndexPath
{
    
    static NSString *SectionTableIdentifier = @"SectionTableIdentifier";
    UITableViewCell *cell = [aTableView dequeueReusableCellWithIdentifier:SectionTableIdentifier];
    
    
    int row = anIndexPath.row;
    int section = anIndexPath.section;
    if (section == 0)
    {
        
        if (cell == nil) {
            cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:SectionTableIdentifier] autorelease];
        }
        NSString *strDID = nil;
        NSString *strIP = nil;
        KYLDeviceInfo *oneDevice  = (KYLDeviceInfo *)[self.m_listAllSearchDevice objectAtIndex:row];
        strDID = oneDevice.m_sDID;
        strIP = [NSString stringWithFormat:@"11%@:%d",oneDevice.m_sIP,oneDevice.m_iPort];
//        if (self.m_listAllSearchDevice && [self.m_listAllSearchDevice count] >= row) {
//            KYLDeviceInfo *oneDevice  = (KYLDeviceInfo *)[self.m_listAllSearchDevice objectAtIndex:row];
//            strDID = oneDevice.m_sDID;
//            strIP = [NSString stringWithFormat:@"%@:%d",oneDevice.m_sIP,oneDevice.m_iPort];
//        }
        cell.detailTextLabel.font = [UIFont fontWithName:@"Arial" size:11];
        cell.detailTextLabel.textColor = [UIColor redColor];
        cell.detailTextLabel.text = strIP;
        cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
        cell.textLabel.text = strDID;

    }
	return cell;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    
    NSString *sTitle = nil;
    return sTitle;
}

- (void) tableView:(UITableView *)anTableView didSelectRowAtIndexPath:(NSIndexPath *)anIndexPath
{
    //[anTableView deselectRowAtIndexPath:anIndexPath animated:YES];
    //NSInteger section = anIndexPath.section ;
    //NSInteger row = anIndexPath.row;
    
    
}

#pragma call back functions

//search call back
- (void) didSucceedSearchOneDevice: (NSString *) chDID ip:(NSString *) ip port:(int) port devName:(NSString *) devName macaddress:(NSString *) mac productType:(NSString *) productType
{
    NSLog(@"didSucceedSearchOneDevice -- %s" , __FUNCTION__);
    if (nil == self.m_listAllSearchDevice) {
        return;
    }
    KYLDeviceInfo *oneDevice = [[KYLDeviceInfo alloc] init];
    oneDevice.m_sDID = chDID;
    oneDevice.m_sIP = ip;
    oneDevice.m_iPort = port;
    oneDevice.m_sDeviceName = devName;
    oneDevice.m_sMacAddress = mac ;
    oneDevice.m_sProductType = productType;
    [self.m_listAllSearchDevice addObject:oneDevice];
    [oneDevice release];
    
    //[self performSelectorOnMainThread:@selector(refreshTheTableView2) withObject:nil waitUntilDone:NO];
   
}

- (void) refreshTheTableView
{
    [self hideTheHUD];
    [self.m_ptableView reloadData];
}

- (void) refreshTheTableView2
{
    [self.m_ptableView reloadData];
}





@end

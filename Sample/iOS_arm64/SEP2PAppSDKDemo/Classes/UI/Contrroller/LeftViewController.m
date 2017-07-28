//
//  LeftViewController.m
//  LeftRightSlider
//
//  Created by Zhao Yiqi on 13-11-27.
//  Copyright (c) 2013年 Zhao Yiqi. All rights reserved.
//

#import "LeftViewController.h"
#import "SliderViewController.h"

#import "KYLDeviceInfo.h"
#import "KYLSearchTool.h"
#import "MBProgressHUD.h"


@interface LeftViewController ()<UITableViewDataSource,UITableViewDelegate>
{
    KYLSearchTool *m_pSearchTool;
}

@end

@implementation LeftViewController
@synthesize m_listAllSearchDevice;
@synthesize m_ptableView;

- (void) dealloc
{
    self.m_ptableView = nil;
    if (m_pSearchTool) {
        m_pSearchTool.delegate = nil;
        [m_pSearchTool release];
        m_pSearchTool = nil;
    }
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


- (void)viewDidLoad
{
    [super viewDidLoad];
    
    UIImageView *imgV=[[UIImageView alloc] initWithFrame:self.view.bounds];
    [imgV setImage:[UIImage imageNamed:@"3333"]];
    [self.view addSubview:imgV];
    [imgV release];
    
    UITableView *tableV=[[UITableView alloc] initWithFrame:CGRectMake(0, 200, self.view.frame.size.width, self.view.frame.size.height-200)];
    tableV.backgroundColor=[UIColor clearColor];
    tableV.delegate=self;
    tableV.dataSource=self;
    [self.view addSubview:tableV];
    self.m_ptableView = tableV;
    [tableV release];
    
    [self initData];
	// Do any additional setup after loading the view.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(UITableViewCell*)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    static NSString *cellID=@"cellID";
    UITableViewCell *cell=[tableView dequeueReusableCellWithIdentifier:cellID];
    NSInteger section = indexPath.section;
    NSInteger row = indexPath.row;
    if(cell==nil){
        cell=[[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:cellID] autorelease];
    }
    cell.backgroundColor=[UIColor clearColor];
    [cell setSelectionStyle:UITableViewCellSelectionStyleNone];
    NSString *strContent = nil;
    if (section == 0) {
        if (row == 0) {
            strContent = @"Search Device";
        }
    }
    else
    {
        if (self.m_listAllSearchDevice && [self.m_listAllSearchDevice count] >= row) {
            KYLDeviceInfo *oneDevice  = (KYLDeviceInfo *)[self.m_listAllSearchDevice objectAtIndex:row];
            strContent = oneDevice.m_sDID;
        }
        switch (row) {
            case 0:
                //[[SliderViewController sharedSliderController] showContentControllerWithModel:@"MainViewController"];
                break;
            case 1:
                //[[SliderViewController sharedSliderController] showContentControllerWithModel:@"RightViewController"];
                break;
            default:
                break;
        }
        
    }
    cell.textLabel.text=strContent;

    return cell;
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)aTableView
{
    return 2;
}

-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    int numrow = 0;
    if (section == 0) {
        numrow = 1;
    }
    else
    {
        numrow = [self.m_listAllSearchDevice count];
    }
    return  numrow;
}

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    NSInteger section = indexPath.section;
    NSInteger row = indexPath.row;
    if (section == 0) {
        if (row == 0) {
            [self startSearch];
        }
    }
    else
    {
        KYLDeviceInfo *oneDevice = nil;
        if (self.m_listAllSearchDevice && [self.m_listAllSearchDevice count] > row) {
            oneDevice = [self.m_listAllSearchDevice objectAtIndex:row];
            NSDictionary *dic = [[NSDictionary alloc] initWithObjectsAndKeys:oneDevice,@"device", nil];
            [[NSNotificationCenter defaultCenter] postNotificationName:@"KYL_notication_select_one_device" object:self userInfo:dic];
            [dic release];
        }
        //[[SliderViewController sharedSliderController] showContentControllerWithModel:@"MainViewController" userInfo:oneDevice];
    }
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

- (BOOL) startSearch
{
    NSLog(@"StartSearch device");
    //[self showTheHUD];
    //[self performSelector:@selector(refreshTheTableView) withObject:nil afterDelay:6];
    BOOL m_bResult = false;
    if (m_pSearchTool) {
        // clear all data
        if (self.m_listAllSearchDevice) {
            [self.m_listAllSearchDevice removeAllObjects];
        }
        [m_pSearchTool startSearch];
    }
    return m_bResult;
}

- (BOOL) stopSearch
{
    //[self hideTheHUD];
    BOOL m_bResult = false;
    if (m_pSearchTool) {
        [m_pSearchTool stopSearch];
    }
   
    return m_bResult;
}

#pragma call back functions

//search call back
- (void) didSucceedSearchOneDevice: (NSString *) chDID ip:(NSString *) ip port:(int) port devName:(NSString *) devName macaddress:(NSString *) mac productType:(NSString *) productType
{
    NSLog(@"didSucceedSearchOneDevice -- %s" , __FUNCTION__);
    if ([self isExistTheDevice:chDID ip:ip port:port]) {
        return;
    }
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
    
    [self performSelectorOnMainThread:@selector(refreshTheTableView2) withObject:nil waitUntilDone:NO];
    
}

- (BOOL) isExistTheDevice:(NSString *) did ip:(NSString *) ip port:(int) port
{
    BOOL isExist = NO;
    @synchronized(self)
    {
        for (id obj in self.m_listAllSearchDevice) {
            KYLDeviceInfo *oneDev = (KYLDeviceInfo *) obj;
            if (//[did isEqualToString:oneDev.m_sDID]&&
                [ip isEqualToString:oneDev.m_sIP]
                && port == oneDev.m_iPort) {
                return YES;
            }
        }
    }
    return isExist;
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

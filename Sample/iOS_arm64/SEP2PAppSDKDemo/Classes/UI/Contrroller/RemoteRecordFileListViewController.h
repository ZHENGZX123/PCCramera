//
//  RemoteRecordFileListViewController.h
//  P2PCamera
//
//  Created by Tsang on 12-12-14.
//
//

#define HEADER_HEIGHT 40

#import <UIKit/UIKit.h>

#import "PMCalendar.h"
#import "SDCardRecordInfoCell.h"
#import "KYLCamera.h"

#import "RemotePlaybackViewController.h"


@interface RemoteRecordFileListViewController : UIViewController<UITableViewDataSource,UITableViewDelegate, UINavigationBarDelegate,PMCalendarControllerDelegate, KYLRemoteRecordPlayProtocol, KYLDeviceStatusChangedProtocol,RemotePlaybackViewControllerProtocol>
{
    IBOutlet UINavigationBar *navigationBar;
    IBOutlet UITableView *tableView;
    IBOutlet UIButton *btnShowDateTime;
    NSCondition *m_timerLock;
    NSTimer *m_timer;
    BOOL m_bFinished;
    NSString *m_strDID;
    NSString *m_strName;
    NSMutableArray *m_RecordFileList;
    NSMutableArray *m_listHasRecordDays;
    int selectedSection;
    PMCalendarController *pmCC;
    PMPeriod *m_currentSelectedPeriod;
    int m_nCurrentSelectTime;
    BOOL m_bShowTheTableHeader;
    UISegmentedControl *segmentedControl;
    UIView *pTableHeadView;
    NSInteger m_nSelectedSegmentIndex;
    
    NSInteger m_nCurrentSelectedIndex;
    NSDate *m_selectDayInCalendar;
    
}

@property (nonatomic, retain) UINavigationBar *navigationBar;
@property (nonatomic, retain) UITableView *tableView;
@property (nonatomic, copy) NSString *m_strDID;
@property (nonatomic, copy) NSString *m_strName;
@property (nonatomic, retain) IBOutlet UIButton *btnShowDateTime;
@property (nonatomic, retain) KYLCamera *m_pCameraObj;
@property (nonatomic, retain) PMCalendarController *pmCC;
@property (nonatomic, retain) PMPeriod *m_currentSelectedPeriod;
@property(nonatomic, retain)  UISegmentedControl *segmentedControl;
@property(nonatomic, retain) NSMutableArray *m_listHasRecordDays;
@property(nonatomic, retain)  NSDate *m_selectDayInCalendar;

- (IBAction)showCalendar:(id)sender;

@end

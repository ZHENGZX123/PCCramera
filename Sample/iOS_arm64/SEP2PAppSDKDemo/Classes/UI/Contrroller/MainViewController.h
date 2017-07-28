//
//  MainViewController.h
//  LeftRightSlider
//
//  Created by Zhao Yiqi on 13-11-27.
//  Copyright (c) 2013年 Zhao Yiqi. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "KYLCamera.h"
#import "KYLCameraProtocol.h"
#import "KYLSearchProtocol.h"
#import "KYLSearchTool.h"
#import "SGPopSelectView.h"
#import "KYLCameraMonitor.h"

@class TSLocateView;
@class KYLWifiObject;
@interface MainViewController : UIViewController<UITextFieldDelegate, UITextViewDelegate, UIAlertViewDelegate,UIActionSheetDelegate, KYLCameraProtocol, KYLSearchProtocol,KYLMontiorTouchProtocol>
{
    KYLCamera *m_pCamera;
    NSArray *m_listAllCGICommand;
    
    //TSLocateView *m_pCommandPickerView;
    NSMutableArray *m_listAllSearchDevice;
    
    KYLWifiObject *selectedWifiObject;
    
    int m_nSelectWiflListRow;
    
}
@property (nonatomic, retain)  UIScrollView *m_pscrollView;

@property (nonatomic, retain)  UIButton *btnConnect;

@property (nonatomic, retain)  UIButton *btnStartVideo;

@property (nonatomic, retain)  UIButton *btnStartAudio;

@property (nonatomic, retain)  UIButton *btnStartTalk;
@property (nonatomic, retain)  UIButton *btnStartRecord;

@property (nonatomic, retain)  UIButton *btnAddCamera;

@property (nonatomic, retain)  UIButton *btnSendCommand;

@property (nonatomic, retain)  UIButton *btnLeft;

@property (nonatomic, retain)  UIButton *btnRight;

@property (nonatomic, retain)  UIButton *btnUp;

@property (nonatomic, retain)  UIButton *btnDown;


@property (nonatomic, retain)  UITextField *textFieldDID;
@property (nonatomic, retain)  UITextField *textFieldAccount;
@property (nonatomic, retain)  UITextField *textFieldPassword;

@property (nonatomic, retain)  UITextView *textViewShowLog;
@property (nonatomic, retain)  UILabel *lableShowStatus;

@property (nonatomic, retain)  UIImageView *imgViewShowVideo;
@property (nonatomic, retain) KYLCameraMonitor *pCameraMonitor;

@property (nonatomic, retain) KYLCamera *m_pCamera;
@property (nonatomic, copy) NSString *strDID;

@property (nonatomic, retain) NSMutableArray *m_listAllSearchDevice;

@property (nonatomic, retain) SGPopSelectView *popView;
@property (nonatomic, retain) NSMutableArray *listWifiInfo;
@property (nonatomic, retain) NSMutableArray *listSGPoPSelectData;
@property (nonatomic, retain) KYLWifiObject *selectedWifiObject;

- (IBAction)btnConnectClicked:(id)sender;

- (IBAction)btnStartVideoClicked:(id)sender;

- (IBAction)btnStartAudioClicked:(id)sender;

- (IBAction)btnStartTalkClicked:(id)sender;


- (IBAction)btnSendCommandClicked:(id)sender;

- (IBAction)btnAddCameraClicked:(id)sender;

- (IBAction)btnUpClicked:(id)sender;
- (IBAction)btnDownClicked:(id)sender;
- (IBAction)btnLeftClicked:(id)sender;
- (IBAction)btnRightClicked:(id)sender;


@end

//
//  ViewController1.h
//  LeftRightSlider
//
//  Created by Zhao Yiqi on 13-11-29.
//  Copyright (c) 2013年 heroims. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "ViewController2.h"

#import "KYLSearchProtocol.h"
#import "KYLCamera.h"


@interface ViewController1 : UIViewController<UITableViewDataSource, UITableViewDelegate, KYLSearchProtocol>
{
    UITableView *m_ptableView;
    NSMutableArray *m_listAllSearchDevice;
}

@property (nonatomic, retain) UITableView *m_ptableView;
@property (nonatomic, retain) NSMutableArray *m_listAllSearchDevice;

@property (nonatomic, retain) UIButton *btnSearch;

@property (nonatomic, retain) KYLCamera *m_pCamera;

@end

//
//  LeftViewController.h
//  LeftRightSlider
//
//  Created by Zhao Yiqi on 13-11-27.
//  Copyright (c) 2013年 Zhao Yiqi. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "KYLSearchProtocol.h"

@interface LeftViewController : UIViewController<KYLSearchProtocol>
{
    NSMutableArray *m_listAllSearchDevice;
    UITableView *m_ptableView;
}
@property (nonatomic, retain) NSMutableArray *m_listAllSearchDevice;
@property (nonatomic, retain) UITableView *m_ptableView;

@end

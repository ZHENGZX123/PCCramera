//
//  SliderViewController.h
//  LeftRightSlider
//
//  Created by Zhao Yiqi on 13-11-27.
//  Copyright (c) 2013年 Zhao Yiqi. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface SliderViewController : UIViewController

@property(nonatomic,retain)UIViewController *LeftVC;
@property(nonatomic,retain)UIViewController *RightVC;
@property(nonatomic,retain)UIViewController *MainVC;

@property(nonatomic,assign)float LeftSContentOffset;
@property(nonatomic,assign)float RightSContentOffset;

@property(nonatomic,assign)float LeftSContentScale;
@property(nonatomic,assign)float RightSContentScale;

@property(nonatomic,assign)float LeftSJudgeOffset;
@property(nonatomic,assign)float RightSJudgeOffset;

@property(nonatomic,assign)float LeftSOpenDuration;
@property(nonatomic,assign)float RightSOpenDuration;

@property(nonatomic,assign)float LeftSCloseDuration;
@property(nonatomic,assign)float RightSCloseDuration;

+ (SliderViewController*)sharedSliderController;

- (void)showContentControllerWithModel:(NSString*)className;
- (void)showContentControllerWithModel:(NSString *)className userInfo:(id) userInfo;
- (void)leftItemClick;
- (void)rightItemClick;

@end

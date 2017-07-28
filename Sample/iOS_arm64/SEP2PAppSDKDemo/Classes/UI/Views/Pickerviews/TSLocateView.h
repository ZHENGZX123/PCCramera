//
//  UICityPicker.h
//  DDMates
//
//  Created by ShawnMa on 12/16/11.
//  Copyright (c) 2011 TelenavSoftware, Inc. All rights reserved.
//


#import <QuartzCore/QuartzCore.h>


@interface TSLocateView : UIActionSheet<UIPickerViewDelegate, UIPickerViewDataSource, CAAnimationDelegate> {

    NSArray *m_listCGICommand;
}

@property (retain, nonatomic) IBOutlet UILabel *titleLabel;
@property (retain, nonatomic) IBOutlet UIPickerView *locatePicker;
@property (assign, nonatomic) int m_iSelectRow;
@property (nonatomic, retain) NSArray *m_listCGICommand;

- (id)initWithTitle:(NSString *)title delegate:(id /*<UIActionSheetDelegate>*/)delegate;

- (void)showInView:(UIView *)view;

@end

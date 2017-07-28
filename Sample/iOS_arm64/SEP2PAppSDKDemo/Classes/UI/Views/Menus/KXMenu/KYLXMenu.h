//
//  KYLXMenu.h
//  P2PCamera
//
//
//

#import <Foundation/Foundation.h>

#import "KYLXMenuItem.h"
#import "KYLXMenuOverlay.h"
#import "KYLXMenuView.h"

@class KYLXMenuView;
@interface KYLXMenu : UIView
{
    KYLXMenuView *_menuView;
    BOOL        _observing;
}

+ (void) showMenuInView:(UIView *)view
               fromRect:(CGRect)rect
              menuItems:(NSArray *)menuItems;

+ (void) dismissMenu;

+ (UIColor *) tintColor;
+ (void) setTintColor: (UIColor *) tintColor;

+ (UIFont *) titleFont;
+ (void) setTitleFont: (UIFont *) titleFont;

- (void) showMenuInView2:(UIView *)view
                fromRect:(CGRect)rect
               menuItems:(NSArray *)menuItems;

- (void) dismissMenu2;


@end

//
//  KYLXMenuView.h
//  P2PCamera
//
//
//

#import <UIKit/UIKit.h>

#import "KYLXMenuOverlay.h"
#import "KYLXMenu.h"
#import "KYLXMenuItem.h"


typedef enum {
    
    KxMenuViewArrowDirectionNone,
    KxMenuViewArrowDirectionUp,
    KxMenuViewArrowDirectionDown,
    KxMenuViewArrowDirectionLeft,
    KxMenuViewArrowDirectionRight,
    
} KxMenuViewArrowDirection;


@interface KYLXMenuView : UIView
{
    KxMenuViewArrowDirection    _arrowDirection;
    CGFloat                     _arrowPosition;
    UIView                      *_contentView;
    NSArray                     *_menuItems;

}

@property (nonatomic, retain)  NSArray *menuItems;

- (void) setupFrameInView:(UIView *)view
                 fromRect:(CGRect)fromRect;
- (void)showMenuInView:(UIView *)view
              fromRect:(CGRect)rect
             menuItems:(NSArray *)menuItems;
- (void)dismissMenu:(BOOL) animated;
- (void)performAction:(id)sender;
- (UIView *) mkContentView;

@end

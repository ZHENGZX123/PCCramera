//
//  KYLXMenu.m
//  P2PCamera
//
//
//

#import "KYLXMenu.h"
#import "KYLXMenuView.h"

static KYLXMenu *gMenu = nil;
static UIColor *gTintColor;
static UIFont *gTitleFont;


@implementation KYLXMenu


+ (instancetype) sharedMenu
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        if (gMenu == nil) {
            gMenu = [[KYLXMenu alloc] init];
        }
    });
    //return [gMenu autorelease];
    return gMenu;
}

- (id) init
{
    NSAssert(!gMenu, @"singleton object");
    
    self = [super init];
    if (self) {
        _menuView = nil;
        //[self registerTheNotifications];
    }
    return self;
}

- (void) dealloc
{
    [self removeTheNotifications];
    if (_observing) {
        [[NSNotificationCenter defaultCenter] removeObserver:self];
    }
    if (_menuView) {
        [_menuView release];
        _menuView = nil;
    }
    
    [super dealloc];
}

- (void) showMenuInView:(UIView *)view
               fromRect:(CGRect)rect
              menuItems:(NSArray *)menuItems
{
    NSParameterAssert(view);
    NSParameterAssert(menuItems.count);
    
    if (_menuView) {
        
        [_menuView dismissMenu:NO];
        _menuView = nil;
    }
    
    if (!_observing) {
        
        _observing = YES;
        
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(orientationWillChange:)
                                                     name:UIApplicationWillChangeStatusBarOrientationNotification
                                                   object:nil];
        [self registerTheNotifications];
    }
    
    
    _menuView = [[KYLXMenuView alloc] init];
    [_menuView showMenuInView:view fromRect:rect menuItems:menuItems];
}

- (void) registerTheNotifications
{
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(didReceiveTheHideMenuNotifications:)
                                                 name:@"K_Notification_Hide_Menus"
                                               object:nil];

}

- (void) removeTheNotifications
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"K_Notification_Hide_Menus" object:nil];
}

- (void ) didReceiveTheHideMenuNotifications:(NSNotification *) notification
{
     [self dismissMenu];
}

- (void) dismissMenu
{
    if (_menuView) {
        
        [_menuView dismissMenu:NO];
        _menuView = nil;
    }
    
    if (_observing) {
        
        _observing = NO;
        [[NSNotificationCenter defaultCenter] removeObserver:self];
    }
}

- (void) orientationWillChange: (NSNotification *) n
{
    [self dismissMenu];
}

+ (void) showMenuInView:(UIView *)view
               fromRect:(CGRect)rect
              menuItems:(NSArray *)menuItems
{
    [[self sharedMenu] showMenuInView:view fromRect:rect menuItems:menuItems];
}

+ (void) dismissMenu
{
    [[self sharedMenu] dismissMenu];
}

- (void) showMenuInView2:(UIView *)view
               fromRect:(CGRect)rect
              menuItems:(NSArray *)menuItems
{
    [self showMenuInView:view fromRect:rect menuItems:menuItems];
}

- (void) dismissMenu2
{
    [self  dismissMenu];
}


+ (UIColor *) tintColor
{
    return gTintColor;
}

+ (void) setTintColor: (UIColor *) tintColor
{
    if (tintColor != gTintColor) {
        gTintColor = tintColor;
    }
}

+ (UIFont *) titleFont
{
    return gTitleFont;
}

+ (void) setTitleFont: (UIFont *) titleFont
{
    if (titleFont != gTitleFont) {
        gTitleFont = titleFont;
    }
}

@end

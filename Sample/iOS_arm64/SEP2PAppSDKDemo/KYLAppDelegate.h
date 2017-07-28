//
//  KYLAppDelegate.h
//  SEP2PAppSDKDemo
//


#import <UIKit/UIKit.h>



@interface KYLAppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@property (strong, nonatomic) UINavigationController *rootNavController;

+(KYLAppDelegate*)shareInstance;

@end

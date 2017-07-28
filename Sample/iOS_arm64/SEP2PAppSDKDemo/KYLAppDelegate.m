//
//  KYLAppDelegate.m
//  SEP2PAppSDKDemo
//


#import "KYLAppDelegate.h"

#import "KYLCamera.h"
#import "SliderViewController.h"
#import "LeftViewController.h"
#import "RightViewController.h"
#import <AudioToolbox/AudioToolbox.h>
#import <AudioToolbox/AudioFile.h>
#include <AudioToolbox/AudioQueue.h>
#include <AudioToolbox/AudioServices.h>

@implementation KYLAppDelegate

+(KYLAppDelegate*)shareInstance {
    return[[UIApplication sharedApplication] delegate];
}

- (void)dealloc
{
    [_window release];
    
    [super dealloc];
}

void MyInterruptionListener(void *inClientData, UInt32  inInterruptionState )
{
}

//耳机监听模式
- (void) activeAudioSessionForListenSound
{
    //OSStatus status;
    AudioSessionInitialize(NULL, NULL, MyInterruptionListener, NULL);
    
    UInt32 inDataSize = kAudioSessionCategory_PlayAndRecord;
    AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(inDataSize), &inDataSize);
    
    inDataSize = 1;
    /*status = */AudioSessionSetProperty(kAudioSessionProperty_OverrideCategoryDefaultToSpeaker, sizeof(inDataSize), &inDataSize);
    //NSLog(@"status1:%c%c%c%c",status >> 24 & 0XFF,status>>16 & 0XFF,status>>8&0XFF, status & 0XFF);
    inDataSize = 1;
    /*status = */AudioSessionSetProperty(kAudioSessionProperty_OverrideCategoryMixWithOthers, sizeof(inDataSize), &inDataSize);
    //NSLog(@"status2:%c%c%c%c",status >> 24 & 0XFF,status>>16 & 0XFF,status>>8&0XFF,status & 0XFF);
    inDataSize = 1;
    /*status = */AudioSessionSetProperty(kAudioSessionProperty_OtherMixableAudioShouldDuck, sizeof(inDataSize), &inDataSize);
    //NSLog(@"status3:%c%c%c%c",status >> 24 & 0XFF,status>>16 & 0XFF,status>>8&0XFF,status & 0XFF);
    AudioSessionSetActive(true);
    
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    [self initUI];
    
    [self activeAudioSessionForListenSound];
    return YES;
}

- (void) initUI
{
    [KYLCamera initSDK];
    
    self.window = [[[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]] autorelease];
    // Override point for customization after application launch.
    
    [SliderViewController sharedSliderController].LeftVC=[[[LeftViewController alloc] init] autorelease];
    [SliderViewController sharedSliderController].RightVC=[[[RightViewController alloc] init] autorelease];
    [SliderViewController sharedSliderController].RightSContentOffset=260;
    [SliderViewController sharedSliderController].RightSContentScale=0.6;
    [SliderViewController sharedSliderController].RightSOpenDuration=0.8;
    [SliderViewController sharedSliderController].RightSCloseDuration=0.8;
    [SliderViewController sharedSliderController].RightSJudgeOffset=160;
    
    self.rootNavController = [[[UINavigationController alloc] initWithRootViewController:[SliderViewController sharedSliderController]] autorelease];
    self.window.rootViewController = self.rootNavController;
    
    self.window.backgroundColor = [UIColor whiteColor];
    [self.window makeKeyAndVisible];
}


- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    [KYLCamera deInitializeSDK];
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end

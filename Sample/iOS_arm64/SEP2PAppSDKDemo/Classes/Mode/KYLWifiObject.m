//
//  KYLWifiObject.m
//  SEP2PAppSDKDemo
//
//
//

#import "KYLWifiObject.h"

@implementation KYLWifiObject
@synthesize sSSID;
@synthesize sMac;
@synthesize nAuthType;
@synthesize sSignLevel;
@synthesize sPercent;
@synthesize nMode;
@synthesize reserve;

- (void) dealloc
{
    self.sSSID = nil;
    self.sMac = nil;
    self.sSignLevel = nil;
    self.sPercent = nil;
    [super dealloc];
}

@end

//
//  KYLDeviceInfo.m
//  SEP2PAppSDKDemo
//

#import "KYLDeviceInfo.h"

@implementation KYLDeviceInfo
@synthesize m_sDID;
@synthesize m_sUsername;
@synthesize m_sPassword;
@synthesize m_sMacAddress;
@synthesize m_sDeviceName;
@synthesize m_sProductType;
@synthesize m_sIP;
@synthesize  m_iPort;
@synthesize m_sReserve1;
@synthesize m_sReserve2;
@synthesize m_sReserve3;

- (void) dealloc
{
    self.m_sDID = nil;
    self.m_sUsername = nil;
    self.m_sPassword = nil;
    self.m_sMacAddress = nil;
    self.m_sDeviceName = nil;
    self.m_sProductType = nil;
    self.m_sIP = nil;
    self.m_iPort = 0;
    self.m_sReserve1 = nil;
    self.m_sReserve2 = nil;
    self.m_sReserve3 = nil;
    
    [super dealloc];
}


@end

//
//  KYLDeviceInfo.h
//  SEP2PAppSDKDemo
//

#import <Foundation/Foundation.h>

@interface KYLDeviceInfo : NSObject
{
    NSString *m_sDID;
    NSString *m_sUsername;
    NSString *m_sPassword;
    NSString *m_sMacAddress;
    NSString *m_sDeviceName;
    NSString *m_sProductType;
    NSString *m_sIP;
    int m_iPort;
    NSString *m_sReserve1;
    NSString *m_sReserve2;
    NSString *m_sReserve3;
}


@property (nonatomic, retain) NSString *m_sDID;
@property (nonatomic, retain) NSString *m_sUsername;
@property (nonatomic, retain) NSString *m_sPassword;
@property (nonatomic, retain) NSString *m_sMacAddress;
@property (nonatomic, retain) NSString *m_sDeviceName;
@property (nonatomic, retain) NSString *m_sProductType;
@property (nonatomic, retain) NSString *m_sIP;
@property (nonatomic, assign) int m_iPort;
@property (nonatomic, retain) NSString *m_sReserve1;
@property (nonatomic, retain) NSString *m_sReserve2;
@property (nonatomic, retain) NSString *m_sReserve3;

@end

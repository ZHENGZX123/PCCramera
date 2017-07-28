//
//  KYLWifiObject.h
//  SEP2PAppSDKDemo
//
//
//

#import <Foundation/Foundation.h>

@interface KYLWifiObject : NSObject
{
    NSString *sSSID;
    NSString *sMac;
    int nAuthType;
    NSString *sSignLevel;
    NSString *sPercent;
    int nMode;
    int reserve;
}

@property (nonatomic, retain) NSString *sSSID;
@property (nonatomic, retain) NSString *sMac;
@property (nonatomic, assign) int nAuthType;
@property (nonatomic, retain) NSString *sSignLevel;
@property (nonatomic, retain) NSString *sPercent;
@property (nonatomic, assign) int nMode;
@property (nonatomic, assign) int reserve;


@end

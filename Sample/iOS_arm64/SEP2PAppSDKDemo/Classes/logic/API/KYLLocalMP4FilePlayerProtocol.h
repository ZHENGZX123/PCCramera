//
//  KYLLocalMP4FilePlayerProtocol.h
//  P2PCamera
//
//
//

#ifndef P2PCamera_KYLLocalMP4FilePlayerProtocol_h
#define P2PCamera_KYLLocalMP4FilePlayerProtocol_h


#import <Foundation/Foundation.h>

@protocol KYLLocalMP4FilePlayerProtocol <NSObject>

- (void) didLocalMP4FilePlayerStoped;
- (void) didLocalMP4FilePlayerGetTotalTimeLength:(int) nTotalTime;
- (void) didLocalMP4FilePlayerReceivedPlaybackYUVData: (char*) yuv length:(int)length width:(int)width height:(int)height timestamp:(int) nTimeStamp;
- (void) didLocalMP4FilePlayerReceivedPlaybackMJPEGData: (char*) yuv length:(int)length  timestamp:(int) nTimeStamp;

@end

#endif

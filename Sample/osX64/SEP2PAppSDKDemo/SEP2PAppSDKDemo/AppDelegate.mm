//
//  AppDelegate.m
//  SEP2PAppSDKDemo
//


#import "AppDelegate.h"
#include "SEP2P_API.h"

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Insert code here to initialize your application
    char chDesc[256]={0};
    //get the sdk version information, you can get the max number of device can be connect
	UINT32 nAPIVer=SEP2P_GetSDKVersion(chDesc, sizeof(chDesc));
	char *pVer=(char *)&nAPIVer;
    NSLog(@"SEP2P_API ver=%d.%d.%d.%d, %s\n", *(pVer+3), *(pVer+2), *(pVer+1), *(pVer+0), chDesc);
    
    
    ST_InitStr stInitStr;
	memset(&stInitStr, 0, sizeof(stInitStr));
	strcpy(stInitStr.chInitStr, "EFGBFFBJKFJKGGJBENHHFOEHGPJJDJNBGNFLEIDOEFJFPJPBHNBKGPPGHAOAMMLOAJNDPPCCLDNLFNGNMHNMNFBOMM");
	strcpy(stInitStr.chPrefix, "VIEW");
	SEP2P_Initialize(&stInitStr, 1);
    NSLog(@"%s %s", __FILE__, __FUNCTION__);
    
    
    usleep(1000000);
    SEP2P_DeInitialize();
}

@end

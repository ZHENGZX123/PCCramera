//
//  KYLSearchTool.m
//  SEP2PAppSDKDemo
//


#import "KYLSearchTool.h"

#import "SEP2P_API.h"
#import "SEP2P_Define.h"
#import "SEP2P_Error.h"

@implementation KYLSearchTool
@synthesize delegate;

- (void) dealloc
{
    self.delegate = nil;
    [super dealloc];
}

- (id) init
{
    self = [super init];
    if (self) {

    }
    return self;
}

/*!
 @method
 @abstract startSearch.
 @discussion start search all device in LAN network.
 @result >=0: means that excute successfully, other wise means excute failed .
 */
- (int ) startSearch
{
    int m_iRet = SEP2P_StartSearch(OnLANSearchCallback, self);
    if (m_iRet==ERR_SEP2P_SUCCESSFUL) {
        NSLog(@"%s the function: startSearchAllCameraInLAN() excute succeed", __FILE__);
    }
    else
    {
        NSLog(@"%s the function: startSearchAllCameraInLAN() excute failed", __FILE__);
    }
    return m_iRet;
}


/*!
 @method
 @abstract stopSearch.
 @discussion stop search all device in LAN network.
 @result >=0: means that excute successfully, other wise means excute failed . .
 */
- (int) stopSearch
{
    int m_iRet = SEP2P_StopSearch();;
    if (m_iRet==ERR_SEP2P_SUCCESSFUL) {
        NSLog(@"%s the function: stopSearch() excute succeed", __FILE__);
    }
    else
    {
        NSLog(@"%s the function: stopSearch() excute failed", __FILE__);
    }
    
    return m_iRet;
}


/*!
 @method
 @abstract OnLANSearchCallback.
 @discussion The static function used for received the LAN network search result call back.
 @result >=0: means that excute successfully, other wise means excute failed . .
 */

static INT32 OnLANSearchCallback(
                                 CHAR*	pData,
                                 UINT32  nDataSize,
                                 VOID*	pUserData)
{
    NSLog(@"OnLANSearchCallback ---");
    KYLSearchTool *pThis = (KYLSearchTool *) pUserData;
    [pThis didReceivedLanSearchCallBack:pData dataSize:nDataSize];
    return 0L;
}



/*!
 @method
 @abstract didReceivedLanSearchCallBack.
 @discussion didReceivedLanSearchCallBack:(CHAR *) pData dataSize:(UINT32) nDataSize。 The  function used for deal with the result from call back function, it return the result to the object that implement the delegate.
 @null .
 */

- (void) didReceivedLanSearchCallBack:(CHAR *) pData dataSize:(UINT32) nDataSize
{
    SEARCH_RESP *pSearchResp=(SEARCH_RESP *)pData;
	NSString *strIP = [[NSString alloc] initWithCString:pSearchResp->szIpAddr encoding:NSUTF8StringEncoding];
    NSString *strDID = [[NSString alloc] initWithCString:pSearchResp->szDeviceID encoding:NSUTF8StringEncoding];
    char chMac[24] ={0};
    sprintf(chMac, "%02X:%02X:%02X:%02X:%02X:%02X", 
                    pSearchResp->szMacAddr[0]&0xFF, pSearchResp->szMacAddr[1]&0xFF, 
                    pSearchResp->szMacAddr[2]&0xFF, pSearchResp->szMacAddr[3]&0xFF,
                    pSearchResp->szMacAddr[4]&0xFF, pSearchResp->szMacAddr[5]&0xFF);
    NSString *strDevName = [[NSString alloc] initWithCString:pSearchResp->szDevName encoding:NSUTF8StringEncoding];
    NSString *strProductType = [[NSString alloc] initWithCString:pSearchResp->product_type encoding:NSUTF8StringEncoding];
    int port = pSearchResp->nPort;
    NSLog(@"search one device DID =%@, IP=%@:%d, mac=%s, producttype=%@",strDID, strIP,port, chMac,strProductType);
    NSString *strMac=[[NSString alloc] initWithFormat:@"%s", chMac];
    if(delegate && [delegate respondsToSelector:@selector(didSucceedSearchOneDevice:ip:port:devName:macaddress:productType:)])
    {
        [delegate didSucceedSearchOneDevice:strDID ip:strIP port:port devName:strDevName macaddress:strMac productType:strProductType];
    }
    
    [strIP release];
    [strDID release];
    [strMac release];
    [strProductType release];
}





@end

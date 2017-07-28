//
//  KYLXMenuItem.h
//  P2PCamera
//
//
//

#import <Foundation/Foundation.h>

@interface KYLXMenuItem : CALayer
{
    
}
@property (readwrite, nonatomic, retain) UIImage *image;
@property (readwrite, nonatomic, retain) NSString *title;
@property (readwrite, nonatomic, assign) id target;
@property (readwrite, nonatomic) SEL action;
@property (readwrite, nonatomic, retain) UIColor *foreColor;
@property (readwrite, nonatomic) NSTextAlignment alignment;
@property (nonatomic, retain) NSDictionary *userInfo;
@property (nonatomic, retain) UIFont *textFont;

+ (instancetype) menuItem:(NSString *) title
                    image:(UIImage *) image
                   target:(id)target
                   action:(SEL) action;

- (id) init:(NSString *) title
      image:(UIImage *) image
     target:(id)target
     action:(SEL) action;

- (id) init:(NSString *) title
      image:(UIImage *) image
     target:(id)target
     action:(SEL) action userinfo:(NSDictionary *) userDic;

- (BOOL) enabled;
- (void) performAction;


@end

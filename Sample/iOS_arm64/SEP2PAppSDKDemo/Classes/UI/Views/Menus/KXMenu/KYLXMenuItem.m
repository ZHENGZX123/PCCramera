//
//  KYLXMenuItem.m
//  P2PCamera
//
//
//

#import "KYLXMenuItem.h"


@implementation KYLXMenuItem
@synthesize userInfo;
@synthesize image = _image;
@synthesize title = _title;
@synthesize foreColor = _foreColor;
@synthesize alignment = _alignment;
@synthesize textFont;

- (void) dealloc
{
    self.textFont = nil;
    self.image = nil;
    self.title = nil;
    self.foreColor = nil;
    self.alignment = nil;
    self.userInfo = nil;
    [super dealloc];
}

+ (instancetype) menuItem:(NSString *) title
                    image:(UIImage *) image
                   target:(id)target
                   action:(SEL) action
{
    return [[[KYLXMenuItem alloc] init:title
                              image:image
                             target:target
                             action:action] autorelease];
}

- (id) init:(NSString *) title
      image:(UIImage *) image
     target:(id)target
     action:(SEL) action
{
    NSParameterAssert(title.length || image);
    
    self = [super init];
    if (self) {
        
        self.title = title;
        self.image = image;
        _target = target;
        _action = action;
    }
    return self;
}

- (id) init:(NSString *) title
      image:(UIImage *) image
     target:(id)target
     action:(SEL) action userinfo:(NSDictionary *) userDic
{
    NSParameterAssert(title.length || image);
    
    self = [super init];
    if (self) {
        
        self.title = title;
        self.image = image;
        _target = target;
        _action = action;
        self.userInfo = userDic;
    }
    return self;
}


- (BOOL) enabled
{
    return _target != nil && _action != NULL;
}

- (void) performAction
{
    __strong id target = self.target;
    
    if (target && [target respondsToSelector:_action]) {
        
        [target performSelectorOnMainThread:_action withObject:self waitUntilDone:YES];
    }
}

- (NSString *) description
{
    return [NSString stringWithFormat:@"<%@ #%p %@>", [self class], self, _title];
}


@end

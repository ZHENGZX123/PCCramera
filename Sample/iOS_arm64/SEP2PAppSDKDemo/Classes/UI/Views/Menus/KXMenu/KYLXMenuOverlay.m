//
//  KYLXMenuOverlay.m
//  P2PCamera
//
//
//

#import "KYLXMenuOverlay.h"
#import "KYLXMenuView.h"

@implementation KYLXMenuOverlay
// - (void) dealloc { NSLog(@"dealloc %@", self); }

- (void)dealloc
{
    [super dealloc];
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        self.backgroundColor = [UIColor clearColor];
        self.opaque = NO;
    }
    return self;
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    UIView *touched = [[touches anyObject] view];
    if (touched == self) {
        
        for (UIView *v in self.subviews) {
            if ([v isKindOfClass:[KYLXMenuView class]]
                && [v respondsToSelector:@selector(dismissMenu:)]) {
                
                [v performSelector:@selector(dismissMenu:) withObject:@(YES)];
            }
        }
    }
}

@end

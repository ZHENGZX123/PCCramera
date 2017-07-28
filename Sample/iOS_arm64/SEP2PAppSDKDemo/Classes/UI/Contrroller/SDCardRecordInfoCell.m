//
//  SDCardRecordInfoCell.m
//  P2PCamera
//
//
//

#import "SDCardRecordInfoCell.h"

@implementation SDCardRecordInfoCell
@synthesize l_fileNameTitle;
@synthesize l_fileTimeLenTitle;
@synthesize l_fileSizeTitle;
@synthesize l_fileNameValue;
@synthesize l_fileTimeLenValue;
@synthesize l_fileSizeValue;

- (void)awakeFromNib {
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}
- (void) dealloc
{
    self.l_fileNameTitle = nil;
    self.l_fileTimeLenTitle = nil;
    self.l_fileSizeTitle = nil;
    self.l_fileNameValue = nil;
    self.l_fileTimeLenValue = nil;
    self.l_fileSizeValue = nil;
    [super dealloc];
}

@end

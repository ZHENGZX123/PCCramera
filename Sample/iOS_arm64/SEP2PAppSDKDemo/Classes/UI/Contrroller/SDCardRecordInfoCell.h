//
//  SDCardRecordInfoCell.h
//  P2PCamera
//
//
//

#import <UIKit/UIKit.h>

@interface SDCardRecordInfoCell : UITableViewCell
{
    
    IBOutlet UILabel *l_fileNameTitle;
    IBOutlet UILabel *l_fileTimeLenTitle;
    IBOutlet UILabel *l_fileSizeTitle;
    IBOutlet UILabel *l_fileNameValue;
    IBOutlet UILabel *l_fileTimeLenValue;
    IBOutlet UILabel *l_fileSizeValue;
    
}

@property (nonatomic, retain) IBOutlet UILabel *l_fileNameTitle;
@property (nonatomic, retain) IBOutlet UILabel *l_fileTimeLenTitle;
@property (nonatomic, retain) IBOutlet UILabel *l_fileSizeTitle;
@property (nonatomic, retain) IBOutlet UILabel *l_fileNameValue;
@property (nonatomic, retain) IBOutlet UILabel *l_fileTimeLenValue;
@property (nonatomic, retain) IBOutlet UILabel *l_fileSizeValue;
@end

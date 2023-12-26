#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@class MarsFontInfo;
@class MarsTextInfo;

@interface FontRenderer : NSObject

- (BOOL)loadFonts:(NSArray<MarsFontInfo*>*)fonts Data:(NSDictionary<NSString*, NSData*>*)fontDatas;

- (UIImage*)drawText:(MarsTextInfo*)textInfo onImage:(UIImage*)image;

- (void)clean;

@end

NS_ASSUME_NONNULL_END

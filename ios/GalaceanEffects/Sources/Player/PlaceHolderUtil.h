#import <Foundation/Foundation.h>
#import <UIKit/UIImage.h>

NS_ASSUME_NONNULL_BEGIN

@interface MNPlaceHolderUtil : NSObject

+ (UIImage*)loadPlaceHolderFromLocal:(NSString*)url
                              Bitmap:(UIImage*)bitmap
                               Error:(NSString*_Nullable*_Nullable)errMsg;

+ (void)loadPlaceHolderWithUrl:(NSString*)url
                        Bitmap:(UIImage*)bitmap
                        Source:(NSString*)source
                      Complete:(void(^)(UIImage* bitmap, NSError* error))complete;

@end

NS_ASSUME_NONNULL_END

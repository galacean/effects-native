//
//  ImageParser.h
//
//  Created by Zongming Liu on 2022/3/1.
//

#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import <Accelerate/Accelerate.h>
#import <UIKit/UIKit.h>


NS_ASSUME_NONNULL_BEGIN

@interface ImageParser : NSObject

+(void)parseUIImageToBitmap:(vImage_Buffer *)dest_buffer image:(UIImage*)image;

@end

NS_ASSUME_NONNULL_END

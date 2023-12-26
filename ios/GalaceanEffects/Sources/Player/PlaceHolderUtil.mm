#import "PlaceHolderUtil.h"
#import "MarsAPLog.h"
#import "CommonUtil.h"
#import "MarsConstants.h"
#import "MarsNativeResourceLoader.h"
#import "MarsTaskScheduleUtil.h"

#define TAG (@"PlaceHolderUtil")

@implementation MNPlaceHolderUtil

+ (UIImage*)loadPlaceHolderFromLocal:(NSString*)url Bitmap:(UIImage*)bitmap Error:(NSString**)errMsg {
    if (bitmap) {
        // 如果有传入图片，直接使用图片
        return bitmap;
    }
    if (!url.length) {
        MNLogInfo(@"%@:loadPlaceHolder no resource", TAG);
        return nil;
    }
    @try {
        // 优先尝试从包内加载
        NSString* path = [MarsNativeResourceLoader loadFilePathFromLocal:url];
        if (!path) {
            // 包内找不到再看多媒体缓存有没有
            path = [MarsNativeResourceLoader loadFilePathFromCache:url];
        }
        if (path.length) {
            NSData* bytes = [NSData dataWithContentsOfFile:path];
            if (!bytes.length) {
                MNLogInfo(@"%@:loadPlaceHolder readFile fail", TAG);
                *errMsg = @"readFile fail";
                return nil;
            }
            UIImage* bmp = [UIImage imageWithData:bytes];
            if (!bmp) {
                MNLogInfo(@"%@:loadPlaceHolder decodeImage fail", TAG);
                *errMsg = @"decodeImage fail";
                return nil;
            }
            return bmp;
        }
    } @catch (NSException* exception) {
        MNLogInfo(@"%@:loadPlaceHolder..e %@", TAG, exception);
        *errMsg = exception.description;
        return nil;
    }
    return nil;
}

+ (void)loadPlaceHolderWithUrl:(NSString*)url
                        Bitmap:(UIImage*)bitmap
                        Source:(NSString*)source
                      Complete:(void(^)(UIImage* bitmap, NSError* error))complete {
    // 尝试从本地加载
    NSString* errMsg = nil;
    UIImage* localBmp = [MNPlaceHolderUtil loadPlaceHolderFromLocal:url Bitmap:bitmap Error:&errMsg];
    if (localBmp) {
        MNLogInfo(@"%@:loadPlaceHolderFromLocal,%@", TAG, url);
        [MNPlaceHolderUtil callbackOnUIThread:complete Bitmap:localBmp Error:nil];
        return;
    } else if (errMsg.length) {
        [MNPlaceHolderUtil callbackOnUIThread:complete Bitmap:nil Error:[CommonUtil makeError:TAG Code:0 Msg:errMsg]];
        return;
    }
    
    if (!url || url.length == 0) {
        // 没有设置placeHolder，不处理
        [MNPlaceHolderUtil callbackOnUIThread:complete Bitmap:nil Error:nil];
        return;
    }
    
    // 不是本地路径，加载url
    [MarsNativeResourceLoader loadBitmap:url BizType:source Complete:^(UIImage* img, NSError* error) {
        MNLogInfo(@"%@:loadBitmap,%@", TAG, url);
        [MNPlaceHolderUtil callbackOnUIThread:complete Bitmap:img Error:error];
    }];
}

#pragma mark - private method

+ (void)callbackOnUIThread:(void(^)(UIImage* bitmap, NSError* error))callback Bitmap:(UIImage*)bitmap Error:(NSError*)error {
    [MarsTaskScheduleUtil postToUIThread:^{
        if (error) {
            callback(nil, error);
            return;
        }
        callback(bitmap, nil);
    } Delayd:0];
}

@end

#undef TAG

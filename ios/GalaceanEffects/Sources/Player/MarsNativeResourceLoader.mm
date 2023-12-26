#import "MarsNativeResourceLoader.h"
#import "MarsConstants.h"
#import "CommonUtil.h"
#import "FileOperation.h"
#import "MarsDownloadUtil.h"
#import "MarsTaskScheduleUtil.h"
#import "MarsAPLog.h"
#include "mars_player_mgr.h"

#define TAG @"MarsNativeResourceLoader"

@implementation MarsNativeVideoResource

- (instancetype)initWithFilePath:(NSString*)filePath Transparent:(BOOL)transparent HardDecoder:(BOOL)hardDecoder PlayerIdx:(int)playerIdx Complete:(void(^)(BOOL success, NSError* error))complete {
    if (self = [super init]) {
        if (!filePath || !filePath.length) {
            complete(NO, [CommonUtil makeError:@"VideoRes" Code:0 Msg:@"file Path is nil"]);
        } else {
            self.videoContext = mn::MarsPlayerManager::CreateVideoContextAndPrepare(
                playerIdx, filePath.UTF8String, [CommonUtil getMD5String:filePath].UTF8String,
                transparent, hardDecoder,
                [self, complete](bool success, const std::string& msg) {
                    if (success) {
                        complete(YES, nil);
                    } else {
                        complete(NO, [CommonUtil makeError:@"VideoRes" Code:0 Msg:[NSString stringWithUTF8String:msg.c_str()]]);
                    }
                }
            );
        }
    }
    return self;
}

@end

@implementation MarsNativeImageResource

- (instancetype)initWithUrl:(NSString*)url {
    if (self = [super init]) {
        self.key = url;
        self.realUrl = url;
        self.isVideo = NO;
    }
    return self;
}

@end

@implementation MarsNativeResourceLoader

+ (NSString*)loadFilePathFromLocal:(NSString*)url {
    if ([url hasPrefix:RES_PATH_ASSETS_PREFIX]) {
        // 包内路径
        NSString* relativePath = [url substringFromIndex:[RES_PATH_ASSETS_PREFIX length]];
        return [[[NSBundle mainBundle].resourceURL URLByAppendingPathComponent:relativePath] path];
    }
    return nil;
}

+ (NSString*)loadFilePathFromCache:(NSString*)url {
    return [MarsDownloadUtil getDownloadCachePath:url];
}

+ (void)loadBitmap:(NSString*)url
           BizType:(NSString*)bizType
          Complete:(void(^)(UIImage* img, NSError* error))complete {
    [MarsDownloadUtil downloadFile:url BizType:bizType Complete:^(NSData* data, NSError* error) {
        if (!complete) {
            return;
        }
        if (!data || error) {
            complete(nil, error);
            return;
        }
        UIImage* img = [UIImage imageWithData:data];
        if (img) {
            complete(img, nil);
        } else {
            complete(nil, [CommonUtil makeError:@"MarsNativeResourceLoader" Code:0 Msg:@"decode image fail"]);
        }
    }];
}

+ (void)loadImages:(NSArray<MarsNativeImageResource*>*)urlList
           DirPath:(NSString*)dirPath
         Variables:(NSDictionary* _Nullable)variables
   VariablesBitmap:(NSDictionary* _Nullable)variablesBitmap
         PlayerIdx:(int)playerIdx
           BizType:(NSString*)bizType
          Complete:(void(^)(NSError* error))complete {
    if (!urlList || urlList.count == 0) {
        [MarsTaskScheduleUtil postToNormalThread:^{
            complete(nil);
        }];
        return;
    }
    @try {
        // 0: 图片数量，1: 已下载数量，2: 是否成功
        NSMutableArray* temp = [[NSMutableArray alloc] initWithObjects:@(urlList.count), @(0), @(1), nil];
        for (int i = 0; i < urlList.count; i++) {
            MarsNativeImageResource* res = [urlList objectAtIndex:i];
            
            // 检查&替换动态参数
            if (variables) {
                NSString* tmp = [variables objectForKey:res.key];
                if (tmp) {
                    res.realUrl = tmp;
                }
            }
            if (variablesBitmap) {
                UIImage* tmp = [variablesBitmap objectForKey:res.key];
                if (tmp) {
                    res.bitmap = tmp;
                }
            }
            
            [MarsNativeResourceLoader loadImageInternal:res DirPath:dirPath PlayerIdx:playerIdx BizType:bizType Complete:^(NSError *error) {
                @synchronized (temp) {
                    if ([[temp objectAtIndex:2] intValue] == 0) {
                        // 之前已经下载失败，不再处理
                        return;
                    }
                    if (error) {
                        [temp setObject:@(0) atIndexedSubscript:2];
                        if (complete) {
                            complete(error ? error : [CommonUtil makeError:@"ResourceLoader" Code:0 Msg:@"loadImageFail"]);
                        }
                        return;
                    }
                    int loaded = [[temp objectAtIndex:1] intValue] + 1;
                    [temp setObject:@(loaded) atIndexedSubscript:1];
                    MNLogInfo(@"ResourceLoader.loadImages:%@ success", res.realUrl);
                    if (loaded == [[temp objectAtIndex:0] intValue]) {
                        // 下载数量和总数一样，返回成功
                        if (complete) {
                            complete(nil);
                        }
                    }
                }
            }];
        }
    } @catch (NSException *exception) {
        if (complete) {
            complete([CommonUtil makeError:@"ResourceLoader" Code:0 Msg:exception.reason]);
        }
    } @finally {
        MNLogInfo(@"ResourceLoader.loadImages:%lu", urlList.count);
    }
}

+ (void)loadFonts:(NSArray<NSString*>*)urlList
          DirPath:(NSString*)dirPath
          BizType:(NSString*)bizType
         Complete:(void(^)(NSDictionary<NSString*, NSData*>* fontDataMap, NSError* error))complete {
    if (!urlList || urlList.count == 0) {
        [MarsTaskScheduleUtil postToNormalThread:^{
            complete(nil, [CommonUtil makeError:@"ResourceLoader" Code:0 Msg:@"empty urlList"]);
        }];
        return;
    }
    
    NSMutableDictionary<NSString*, NSData*> *fontDataMap = [NSMutableDictionary dictionary];
    @try {
        // 0: 字体数量，1: 已加载数量，2: 是否成功
        NSMutableArray* temp = [[NSMutableArray alloc] initWithObjects:@(urlList.count), @(0), @(1), nil];
        for (int i = 0; i < urlList.count; i++) {
            NSString* url = [urlList objectAtIndex:i];
            
            [MarsNativeResourceLoader loadFontInternal:url DirPath:dirPath BizType:bizType Complete:^(NSData* data, NSError *error) {
                @synchronized (temp) {
                    if ([[temp objectAtIndex:2] intValue] == 0) {
                        // 之前已经加载失败，不再处理
                        return;
                    }
                    if (error) {
                        [temp setObject:@(0) atIndexedSubscript:2];
                        if (complete) {
                            complete(nil, [CommonUtil makeError:@"ResourceLoader" Code:0 Msg:@"loadFontsFail"]);
                        }
                        return;
                    }
                    
                    [fontDataMap setObject:data forKey:url];
                    int loaded = [[temp objectAtIndex:1] intValue] + 1;
                    [temp setObject:@(loaded) atIndexedSubscript:1];
                    MNLogInfo(@"ResourceLoader.loadFonts:%@ success", url);
                    
                    if (loaded == [[temp objectAtIndex:0] intValue]) {
                        // 加载数量和总数一样，返回成功
                        if (complete) {
                            complete(fontDataMap, nil);
                        }
                    }
                }
            }];
        }
    } @catch (NSException *exception) {
        if (complete) {
            complete(nil, [CommonUtil makeError:@"ResourceLoader.loadFonts" Code:0 Msg:exception.reason]);
        }
    } @finally {
        MNLogInfo(@"ResourceLoader.loadFonts:%lu", urlList.count);
    }
}

+ (void)loadZip:(NSString*)url
        BizType:(NSString*)bizType
 ValidTimeStamp:(NSTimeInterval)validTimeStamp
            MD5:md5
       Complete:(void(^)(NSString* dirPath, NSError* error))complete {
    NSString* urlMd5 = [CommonUtil getMD5String:url];
    NSString* dstDir = [NSString stringWithFormat:@"%@%@", [MNFileOperation getMarsDir], urlMd5];
    
    NSTimeInterval timeStamp = [[NSDate date] timeIntervalSince1970];
    if (validTimeStamp > timeStamp) {
        // 如果传入的时间戳晚于当前时间，使用传入的
        timeStamp = validTimeStamp;
    }
    if ([MNFileOperation checkDirExist:dstDir UpdateTimeStamp:YES Time:timeStamp]) {
        complete(dstDir, nil);
        return;
    }
    [MNFileOperation downloadAndUnzip:url BizType:bizType DstPath:dstDir Time:timeStamp MD5:md5 Complete:complete];
}

#pragma mark - private

+ (void)loadImageInternal:(MarsNativeImageResource*)res
                  DirPath:(NSString*)dirPath
                PlayerIdx:(int)playerIdx
                  BizType:(NSString*)bizType
                 Complete:(void(^)(NSError* error))complete {
    NSString* url = res.realUrl;
    if (!url.length) {
        [MarsTaskScheduleUtil postToNormalThread:^{
            complete([CommonUtil makeError:TAG Code:0 Msg:@"url is null"]);
        }];
        return;
    }
    
    if (res.bitmap) {
        // 已有图片, 无需加载
        [MarsTaskScheduleUtil postToNormalThread:^{
            complete(nil);
        }];
        return;
    }
    
    // 文本节点底图
    if ([url hasPrefix:@"text://"]) {
        NSArray* components = [url componentsSeparatedByString:@"_"];
        if (components.count == 3) {
            NSNumberFormatter *formatter = [[NSNumberFormatter alloc] init];
            NSNumber *widthNumber = [formatter numberFromString:components[1]];
            NSNumber *heightNumber = [formatter numberFromString:components[2]];
            if (widthNumber && heightNumber) {
                float width = [widthNumber floatValue];
                float height = [heightNumber floatValue];
                res.bitmap = [MarsNativeResourceLoader generateTransparentImageWithWidth:width Height:height];
                [MarsTaskScheduleUtil postToNormalThread:^{
                    complete(nil);
                }];
            } else {
                [MarsTaskScheduleUtil postToNormalThread:^{
                    complete([CommonUtil makeError:TAG
                                              Code:0
                                               Msg:@"url is text://xx_xx_xx, but failed to parse width and height."]);
                }];
            }
        } else {
            [MarsTaskScheduleUtil postToNormalThread:^{
                complete([CommonUtil makeError:TAG 
                                          Code:0
                                           Msg:@"url is wrong, text://xx_xx_xx expected."]);
            }];
        }
        return;
    }
    
    if (![url hasPrefix:@"https://"] && ![url hasPrefix:@"http://"]) {
        [MarsTaskScheduleUtil postToNormalThread:^{
            NSString* filePath = [NSString stringWithFormat:@"%@/%@", dirPath, url];
            if (res.isVideo) {
                if ([[NSFileManager defaultManager] fileExistsAtPath:filePath]) {
                    res.videoRes = [[MarsNativeVideoResource alloc] initWithFilePath:filePath
                                                                         Transparent:res.isTransparentVideo
                                                                         HardDecoder:res.videoHardDecoder
                                                                           PlayerIdx:playerIdx
                                                                            Complete:^(BOOL success, NSError* error) {
                        if (success) {
                            complete(nil);
                        } else {
                            complete(error);
                        }
                    }];
                } else {
                    complete([CommonUtil makeError:TAG Code:0 Msg:@"read video file failed"]);
                }
                return;
            }
            NSData* data = [NSData dataWithContentsOfFile:filePath];
            if (data) {
                res.data = data;
                complete(nil);
            } else {
                complete([CommonUtil makeError:TAG Code:0 Msg:@"read file failed"]);
            }
        }];
        return;
    }
    
    [MarsDownloadUtil downloadFile:url BizType:bizType Complete:^(NSData* data, NSError* error) {
        if (error) {
            complete(error);
            return;
        }
        if (res.isVideo) {
            NSString* filePath = [MarsDownloadUtil getDownloadCachePath:res.realUrl];
            res.videoRes = [[MarsNativeVideoResource alloc] initWithFilePath:filePath
                                                                 Transparent:res.isTransparentVideo
                                                                 HardDecoder:res.videoHardDecoder
                                                                   PlayerIdx:playerIdx
                                                                    Complete:^(BOOL success, NSError* error) {
                if (success) {
                    complete(nil);
                } else {
                    complete(error);
                }
            }];
            return;
        }
        if (data) {
            res.data = data;
            complete(nil);
        } else {
            complete(error ? error : [CommonUtil makeError:TAG Code:0 Msg:@"download file failed"]);
        }
    }];
}

+ (void)loadFontInternal:(NSString*)url
                 DirPath:dirPath
                 BizType:bizType
                Complete:(void(^)(NSData* data, NSError* error))complete {
    if (!url.length) {
        [MarsTaskScheduleUtil postToNormalThread:^{
            complete(nil, [CommonUtil makeError:TAG Code:0 Msg:@"url is null"]);
        }];
        return;
    }
    
    if (![url hasPrefix:@"https://"] && ![url hasPrefix:@"http://"]) {
        [MarsTaskScheduleUtil postToNormalThread:^{
            NSString* filePath = [NSString stringWithFormat:@"%@/%@", dirPath, url];
            NSData* data = [NSData dataWithContentsOfFile:filePath];
            complete(data, (data == nil) ? [CommonUtil makeError:TAG Code:0 Msg:@"read file failed"] : nil);
        }];
        return;
    }
    
    [MarsDownloadUtil downloadFile:url BizType:bizType Complete:^(NSData* data, NSError* error) {
        complete(data, error);
    }];
}

+ (UIImage *)generateTransparentImageWithWidth:(CGFloat)width Height:(CGFloat)height {
    UIGraphicsBeginImageContextWithOptions(CGSizeMake(width, height), NO, 0.0);
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextSetFillColorWithColor(context, [UIColor clearColor].CGColor);
    CGContextFillRect(context, CGRectMake(0, 0, width, height));
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return image;
}

@end

#undef TAG

//
//  MultiMediaServiceAdapter.m
//  MarsNative
//
//  Created by changxing on 2023/2/8.
//  Copyright © 2023 Alipay. All rights reserved.
//

#import "MarsDownloadUtil.h"
#import "CommonUtil.h"
#import "MarsTaskScheduleUtil.h"
#import "GEUnzipProtocol.h"
#import "MarsConfigUtil.h"
#import "MarsAPLog.h"

#pragma mark - MarsDownloadService

@interface MarsDownloadService : NSObject
@end

static MarsDownloadService* sDownloadService = nil;
static NSString* sCacheDir = nil;
static NSString* sToken = @"DownloadService";

@implementation MarsDownloadService

+ (MarsDownloadService*)shared {
    @synchronized (sToken) {
        if (!sDownloadService) {
            sDownloadService = [[MarsDownloadService alloc] init];
        }
        return sDownloadService;
    }
}

- (void)downloadFile:(NSString*)url Complete:(void(^)(NSData* data, NSError* error))complete {
    [MarsTaskScheduleUtil postToNormalThread:^{
        @synchronized (sToken) {
            NSError* err = nil;
            NSData* data = nil;
            @try {
                NSString* filePath = [self getCacheFilePath:url];
                data = [NSData dataWithContentsOfFile:filePath];
                MNLogInfo(@"downloadFile %@ cache:%@,data:%@", url, filePath, data);
                if (!data) {
                    data = [NSData dataWithContentsOfURL:[NSURL URLWithString:url]];
                    MNLogInfo(@"downloadFile download:%@,data:%@", url, data);
                    BOOL success = [data writeToFile:filePath atomically:YES];
                    MNLogInfo(@"downloadFile save file result:%d", success);
                }
            } @catch (NSException *exception) {
                err = [CommonUtil makeError:sToken Code:0 Msg:exception.reason];
            } @finally {
                if (complete) {
                    if (err) {
                        complete(nil, err);
                    } else if (!data) {
                        complete(nil, [CommonUtil makeError:sToken Code:0 Msg:@"data is nil"]);
                    } else {
                        complete(data, nil);
                    }
                }
            }
        }
    }];
}

- (void)unzip:(NSString*)zipPath To:(NSString*)targetDir Complete:(void(^)(NSString* dirPath, NSError* error))complete {
    [MarsTaskScheduleUtil postToNormalThread:^{
        @synchronized (sToken) {
            NSFileManager* fm = [NSFileManager defaultManager];
            if ([fm fileExistsAtPath:targetDir]) {
                MNLogInfo(@"unzip %@ target:%@,already exist", zipPath, targetDir);
                complete(targetDir, nil);
                return;
            }
            // 做成协议，外部传进来:
//            MarsZipArchive* zip = [[MarsZipArchive alloc] init];
            id<GEUnzipProtocol>  zip = [GEUnzipManager shared].unzipDelegate;
            //[[MarsZipArchive alloc] init];
            if ([zip UnzipOpenFile:zipPath]) {
                if ([zip UnzipFileTo:targetDir overWrite:YES]) {
                    MNLogInfo(@"unzip success");
                    complete(targetDir, nil);
                } else {
                    MNLogInfo(@"unzip fail");
                    complete(nil, [CommonUtil makeError:sToken Code:0 Msg:@"unzip file fail"]);
                }
                [zip UnzipCloseFile];
            } else {
                MNLogInfo(@"open zip fail");
                complete(nil, [CommonUtil makeError:sToken Code:0 Msg:@"open zip file fail"]);
            }
        }
    }];
}

- (NSString*)getCacheFilePath:(NSString*)url {
    return [NSString stringWithFormat:@"%@%@.tmp", [MarsDownloadService getCacheDir], [CommonUtil getMD5String:url]];
}

+ (NSString*)getCacheDir {
    if (sCacheDir == nil) {
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString* docDir = [paths objectAtIndex:0];
        sCacheDir = [docDir stringByAppendingFormat:@"/mars-caches/"];
        
        NSFileManager* fm = [NSFileManager defaultManager];
        if (![fm fileExistsAtPath:sCacheDir]) {
            [fm createDirectoryAtPath:sCacheDir withIntermediateDirectories:YES attributes:nil error:nil];
        }
    }
    return sCacheDir;
}

@end

#pragma mark - MultiMediaServiceAdapter

@implementation MarsDownloadUtil

+ (void)downloadFile:(NSString*)url BizType:(NSString*)bizType Complete:(void(^)(NSData* data, NSError* error))complete {
    [[MarsDownloadService shared] downloadFile:url Complete:complete];
}

+ (void)downloadZip:(NSString*)url BizType:(NSString*)bizType MD5:(NSString*)md5 TempDir:(NSString*)tempDir Complete:(void(^)(NSString* dirPath, NSError* error))complete {
    [[MarsDownloadService shared] downloadFile:url Complete:^(NSData *data, NSError *error) {
        if (error) {
            complete(nil, error);
        } else {
            [[MarsDownloadService shared] unzip:[[MarsDownloadService shared] getCacheFilePath:url] To:tempDir Complete:complete];
        }
    }];
}

+ (NSString*)getDownloadCachePath:(NSString*)url {
    return [[MarsDownloadService shared] getCacheFilePath:url];
}

@end

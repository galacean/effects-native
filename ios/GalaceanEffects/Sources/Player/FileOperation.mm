#import "FileOperation.h"
#import "MarsAPLog.h"
#import "MarsDownloadUtil.h"
#import "CleanUtil.h"
#import "MarsMonitorUtil.h"

static const NSString* sLockToken = @"file_token";

static NSString* sMarsDir;

static const NSMutableDictionary* sLoadingMap = [[NSMutableDictionary alloc] init];

typedef void (^MNDownloadZipCallback)(NSString* dstPath, NSError* error);

@interface MNFileOperation ()

+ (void)updateTimpStampLocked:(NSString*)dirPath Time:(NSTimeInterval)timeStamp;

+ (NSTimeInterval)getTimeStampLocked:(NSString*)dirPath;

+ (void)dispatchDownloadCallbackLocked:(NSString*)key DstPath:(NSString*)dstPath Error:(NSError*)err;

@end

@implementation MNFileOperation

+ (BOOL)checkDirExist:(NSString*)dirPath UpdateTimeStamp:(BOOL)needUpdate Time:(NSTimeInterval)timeStamp {
    @synchronized (sLockToken) {
        BOOL isDictionary = NO;
        // 检查dirPath是否存在并且是目录
        if ([[NSFileManager defaultManager] fileExistsAtPath:dirPath isDirectory:&isDictionary]) {
            if (isDictionary) {
                NSArray* files = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:dirPath error:nil];
                if (files.count > 0) {
                    // dirPath下面有文件，认为是合法的资源
                    MNLogInfo(@"ResourceLoader.loadZip:already exist:%@,%lu", dirPath, files.count);
                    if (needUpdate) {
                        // 更新时间戳
                        [MNFileOperation updateTimpStampLocked:dirPath Time:timeStamp];
                    }
                    if (sLoadingMap.count == 0) {
                        // 尝试清理缓存
                        [MNCleanUtil tryCleanMarsFiles];
                    }
                    return YES;
                }
            }
            // 不是目录，删除
            [[NSFileManager defaultManager] removeItemAtPath:dirPath error:nil];
        }
    }
    return NO;
}

+ (void)downloadAndUnzip:(NSString*)zipUrl
                 BizType:(NSString*)bizType
                 DstPath:(NSString*)dstPath
                    Time:(NSTimeInterval)timeStamp
                     MD5:(NSString*)md5
                Complete:(void(^)(NSString* dstPath, NSError* error))complete {
    @synchronized (sLockToken) {
        { // 如果正在下载，存入回调队列
            NSMutableArray* arr = [sLoadingMap objectForKey:zipUrl];
            if (arr && arr.count) {
                MNLogInfo(@"MNFileOperation duplicate loading,skip,%@", zipUrl);
                [arr addObject:complete];
                return;
            }
        }
        // 下载后的资源先放到临时目录
        NSString* tmpDir = [NSString stringWithFormat:@"%@_tmp", dstPath];
        if ([[NSFileManager defaultManager] fileExistsAtPath:tmpDir]) {
            NSError* err = nil;
            if (![[NSFileManager defaultManager] removeItemAtPath:tmpDir error:&err]) {
                // 如果不能删除旧的临时目录，返回异常
                MNLogInfo(@"MNFileOperation remove old tmpDir fail:%@,%@", dstPath, err);
                [MNFileOperation dispatchDownloadCallbackLocked:zipUrl DstPath:dstPath Error:err];
                return;
            }
        }
        MNLogInfo(@"MNFileOperation start download");
        [sLoadingMap setObject:[[NSMutableArray alloc] initWithObjects:complete, nil] forKey:zipUrl];
        // 开始下载zip
        [MarsDownloadUtil downloadZip:zipUrl BizType:bizType MD5:md5 TempDir:tmpDir Complete:^(NSString * _Nonnull dirPath, NSError * _Nonnull error) {
            @synchronized (sLockToken) {
                MNLogInfo(@"ResourceLoader.loadZip:finish download:%@,%@", dirPath, error);
                if (!error) {
                    NSError* err = nil;
                    // 把临时目录拷贝到资源目录
                    bool success = [[NSFileManager defaultManager] moveItemAtPath:dirPath toPath:dstPath error:&err];
                    if (!success) {
                        MNLogInfo(@"ResourceLoader.loadZip:moveItem fail,%@", err);
                        [MNFileOperation dispatchDownloadCallbackLocked:zipUrl DstPath:dstPath Error:err];
                        return;
                    } else {
                        [MNFileOperation updateTimpStampLocked:dstPath Time:timeStamp];
                        [MNFileOperation dispatchDownloadCallbackLocked:zipUrl DstPath:dstPath Error:nil];
                    }
                } else {
                    [MNFileOperation dispatchDownloadCallbackLocked:zipUrl DstPath:dstPath Error:error];
                }
            }
        }];
    }
}

+ (BOOL)deleteDirIfExpired:(NSString*)dirPath ExpiredInterval:(NSTimeInterval)expiredInterval Error:(NSError**)err {
    @synchronized (sLockToken) {
        if ([[NSDate date] timeIntervalSince1970] - [MNFileOperation getTimeStampLocked:dirPath] >= expiredInterval) {
            MNLogInfo(@"MNFileOperation delete,%@", dirPath);
            NSError* tmpErr = nil;
            [[NSFileManager defaultManager] removeItemAtPath:dirPath error:&tmpErr];
            if (tmpErr) {
                if (err) {
                    *err = tmpErr;
                }
                MNLogInfo(@"MNFileOperation delete err:%@", tmpErr);
            } else {
                return YES;
            }
        } else {
            MNLogInfo(@"MNFileOperation skip,%@", dirPath);
        }
    }
    return NO;
}

+ (NSString*)getMarsDir {
    if (sMarsDir == nil) {
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString* docDir = [paths objectAtIndex:0];
        sMarsDir = [docDir stringByAppendingFormat:@"/marsnative/"];
    }
    return sMarsDir;
}

#pragma mark - private method

+ (void)dispatchDownloadCallbackLocked:(NSString*)key DstPath:(NSString*)dstPath Error:(NSError*)err {
    NSMutableArray* arr = [sLoadingMap objectForKey:key];
    if (arr) {
        for (int i = 0; i < arr.count; i++) {
            MNDownloadZipCallback callback = [arr objectAtIndex:i];
            callback(dstPath, err);
        }
    }
    [sLoadingMap removeObjectForKey:key];
    if (sLoadingMap.count == 0) {
        [MNCleanUtil tryCleanMarsFiles];
    }
}

#pragma mark - timeStamp

+ (void)updateTimpStampLocked:(NSString*)dirPath Time:(NSTimeInterval)timeStamp {
    NSString* filePath = [MNFileOperation getTimeStampFilePath:dirPath];
    if (!filePath) {
        return;
    }
    
    NSFileManager* defaultManager = [NSFileManager defaultManager];
    if ([defaultManager fileExistsAtPath:filePath]) {
        NSError* error = nil;
        NSDictionary* attr = [defaultManager attributesOfItemAtPath:filePath error:&error];
        if (error) {
            MNLogInfo(@"MNFileOperation attributesOfItemAtPath fail,%@", error);
            [MarsMonitorUtil monitorMarsNativeClean:@"record" Opt:@"update" Succ:NO ErrFile:[filePath lastPathComponent] ErrMsg:@"get attributesOfItemAtPath fail"];
            return;
        }
        NSMutableDictionary* d = [NSMutableDictionary dictionaryWithDictionary:attr ?:@{}];
        [d setObject:[NSDate dateWithTimeIntervalSince1970:timeStamp] forKey:NSFileModificationDate];
        [defaultManager setAttributes:d.copy ofItemAtPath:filePath error:&error];
        if (error) {
            MNLogInfo(@"MNFileOperation setAttributes fail,%@", error);
            [MarsMonitorUtil monitorMarsNativeClean:@"record" Opt:@"update" Succ:NO ErrFile:[filePath lastPathComponent] ErrMsg:error.description];
            return;
        }
        MNLogInfo(@"MNFileOperation updateLastVisitTimeFile,%@,%lf", dirPath, timeStamp);
    }
}

+ (NSTimeInterval)getTimeStampLocked:(NSString*)dirPath {
    NSString* filePath = [MNFileOperation getTimeStampFilePath:dirPath];
    if (!filePath) {
        return [[NSDate date] timeIntervalSince1970];
    }
    
    NSFileManager* defaultManager = [NSFileManager defaultManager];
    if ([defaultManager fileExistsAtPath:filePath]) {
        NSError* error = nil;
        NSDictionary* attr = [defaultManager attributesOfItemAtPath:filePath error:&error];
        if (error) {
            MNLogInfo(@"MNFileOperation attributesOfItemAtPath fail,%@", error);
            [MarsMonitorUtil monitorMarsNativeClean:@"record" Opt:@"read" Succ:NO ErrFile:[filePath lastPathComponent] ErrMsg:@"get attributesOfItemAtPath fail"];
            return [[NSDate date] timeIntervalSince1970];
        }
        NSDate* date = [attr objectForKey:NSFileModificationDate];
        if (!date) {
            MNLogInfo(@"MNFileOperation attributesOfItemAtPath empty");
            [MarsMonitorUtil monitorMarsNativeClean:@"record" Opt:@"read" Succ:NO ErrFile:[filePath lastPathComponent] ErrMsg:@"get attributesOfItemAtPath empty"];
            return [[NSDate date] timeIntervalSince1970];
        }
        return [date timeIntervalSince1970];
    } else {
        MNLogInfo(@"MNFileOperation getFileLastVisitTime not exist,%@", dirPath);
        return [[NSDate date] timeIntervalSince1970];
    }
}

+ (NSString*)getTimeStampFilePath:(NSString*)dirPath {
    if ([MNCleanUtil disabled]) {
        return nil;
    }
    NSString* filePath = [NSString stringWithFormat:@"%@/lastVisitTime", dirPath];
    NSFileManager* defaultManager = [NSFileManager defaultManager];
    if (![defaultManager fileExistsAtPath:filePath]) {
        BOOL success = [defaultManager createFileAtPath:filePath contents:nil attributes:nil];
        if (!success) {
            [MarsMonitorUtil monitorMarsNativeClean:@"record" Opt:@"create" Succ:NO ErrFile:[dirPath lastPathComponent] ErrMsg:@"createNewFile fail"];
            return nil;
        }
    }
    return filePath;
}

@end

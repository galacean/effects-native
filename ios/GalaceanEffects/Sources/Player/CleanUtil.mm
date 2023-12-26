#import "CleanUtil.h"
#import "CommonUtil.h"
#import "MarsAPLog.h"
#import "MarsTaskScheduleUtil.h"
#import "MarsConfigUtil.h"
#import "FileOperation.h"
#import "MarsStorageUtil.h"
#import "MarsMonitorUtil.h"

// 关闭清理的开关，默认不关闭
static NSString* MN_CLEAN_CLOSE_SWITCH = @"mn_Purgeable_Clean_Disable";
// 文件过期时间间隔
static NSString* MN_CLEAN_EXPIRED_SWITCH = @"mn_Purgeable_Date_Diff";
// 检查时间间隔
static NSString* MN_CLEAN_INTERVAL_SWITCH = @"mn_Purgeable_Runtime_Diff";

static BOOL sCleanSwitchClosed; // YES关闭清理缓存，默认NO

static NSTimeInterval sLastCleanTimestamp; // 上次清理缓存时间
static NSTimeInterval sExpiredInterval; // 缓存过期秒数
static NSTimeInterval sCleanInterval; // 清理缓存时间间隔秒数

@implementation MNCleanUtil

+ (void)tryCleanMarsFiles {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        // 初始化开关值
        sCleanSwitchClosed = [[[MarsConfigUtil shareInstance] configForKey:MN_CLEAN_CLOSE_SWITCH] isEqualToString:@"true"];
        if (!sCleanSwitchClosed) {
            // 如果没有关闭清理功能，读取上次清理的时间
            sLastCleanTimestamp = [MarsStorageUtil doubleForKey:[MarsStorageUtil KEY_LAST_CLEAN_TIMESTAMP] defaultValue:0];
            if (sLastCleanTimestamp == 0) {
                // 没有记录，默认从当前时间开始计算
                sLastCleanTimestamp = [[NSDate date] timeIntervalSince1970];
                [MarsStorageUtil setDouble:sLastCleanTimestamp forKey:[MarsStorageUtil KEY_LAST_CLEAN_TIMESTAMP]];
            }
            NSInteger expiredTimeIntervalSwitch = [[MarsConfigUtil shareInstance] intValueForKey:MN_CLEAN_EXPIRED_SWITCH defaultVal:3];
            NSInteger cleanTimeIntervalSwitch = [[MarsConfigUtil shareInstance] intValueForKey:MN_CLEAN_INTERVAL_SWITCH defaultVal:2];
            sExpiredInterval = expiredTimeIntervalSwitch * 24 * 3600;
            sCleanInterval = cleanTimeIntervalSwitch * 24 * 3600;
        }
        MNLogInfo(@"CleanUtil config:%d,%lf,%lf,%lf", sCleanSwitchClosed, sExpiredInterval, sCleanInterval, sLastCleanTimestamp);
    });
    
    NSTimeInterval currentTime = [[NSDate date] timeIntervalSince1970];
    if (sCleanSwitchClosed || (currentTime - sLastCleanTimestamp < sCleanInterval)) {
        // 开关关闭或未到清理时间
        MNLogInfo(@"CleanUtil tryCleanMarsFiles skip,%lf,%lf", currentTime - sLastCleanTimestamp, sCleanInterval);
        return;
    }
    [MarsTaskScheduleUtil postToNormalThread:^{
        // 获取mars资源总目录
        NSString* dir = [MNFileOperation getMarsDir];
        NSError* err = nil;
        NSFileManager* defaultManager = [NSFileManager defaultManager];
        NSArray* dirContents = [defaultManager contentsOfDirectoryAtPath:dir error:&err];
        if (err) {
            // 读取目录失败
            MNLogInfo(@"CleanUtil readDir fail,%@", err);
            [MarsMonitorUtil monitorMarsNativeClean:@"clean" Opt:@"read marsDir" Succ:NO ErrFile:@"marsnative" ErrMsg:err.description];
            return;
        }
        NSString* errFile = nil;
        NSString* errMsg = nil;
        // 遍历mars资源目录下的目录
        for (int i = 0; i < dirContents.count; i++) {
            NSString* fileName = [dirContents objectAtIndex:i];
            // 对于_tmp目录，会生成一个时间戳文件并返回当前时间戳，不会误删
            NSString* e = [MNCleanUtil cleanMarsDir:dir File:fileName];
            if (e && e.length) {
                errMsg = e;
                errFile = fileName;
            }
        }
        if (errMsg) {
            MNLogInfo(@"CleanUtil tryCleanMarsFiles fail,%@,%@", errFile, errMsg);
            [MarsMonitorUtil monitorMarsNativeClean:@"clean" Opt:@"clean" Succ:NO ErrFile:errFile ErrMsg:errMsg];
        } else {
            MNLogInfo(@"CleanUtil tryCleanMarsFiles success");
            [MarsMonitorUtil monitorMarsNativeClean:@"clean" Opt:@"clean" Succ:YES ErrFile:nil ErrMsg:nil];
        }
    }];
    sLastCleanTimestamp = currentTime;
    [MarsStorageUtil setDouble:sLastCleanTimestamp forKey:@"MarsNativePurgeTime"];
}

+ (BOOL)disabled {
    return sCleanSwitchClosed;
}

#pragma mark - private method

+ (NSString*)cleanMarsDir:(NSString*)dirPath File:(NSString*)fileName {
    NSString* filePath = [NSString stringWithFormat:@"%@%@", dirPath, fileName];
    MNLogInfo(@"CleanUtil cleanMars,%@", fileName);
    
    NSError* err;
    [MNFileOperation deleteDirIfExpired:filePath ExpiredInterval:sExpiredInterval Error:&err];
    if (err) {
        return err.description;
    }
    return nil;
}

@end

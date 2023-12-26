#import "MarsDataBase.h"
#import "MarsAPLog.h"
#import "MarsDataJsonBin.h"

#define TAG (@"MarsDataCreator")

// json+bin
#define FILE_MARS_JSON (@"mars.json")
#define FILE_MARS_BIN (@"mars.bin")

@implementation MarsImageInfo
@end
@implementation MarsTextContent
@end
@implementation MarsTextInfo
@end
@implementation MarsFontInfo
@end

@implementation MarsDataBase

+ (MarsDataBase*)createMarsData:(NSString*)dirPath {
    MarsDataBase* ret = nil;
    @try {
        if ([dirPath length] == 0) {
            // 资源目录为空
            MNLogInfo(@"%@:createMarsData:dirPath empty", TAG);
            return nil;
        }
        BOOL isDict = NO;
        BOOL dirExist = [[NSFileManager defaultManager] fileExistsAtPath:dirPath isDirectory:&isDict];
        if (!dirExist || !isDict) {
            // 资源目录不存在或不是目录
            MNLogInfo(@"%@:createMarsData:dirPath invalid,%d", TAG, dirExist);
            return nil;
        }
        NSString* marsJsonPath = [dirPath stringByAppendingPathComponent:FILE_MARS_JSON];
        if ([[NSFileManager defaultManager] fileExistsAtPath:marsJsonPath]) {
            // 存在mars.json
            MNLogInfo(@"%@:createMarsData:mars.json exist", TAG);
            NSString* marsBinPath = [dirPath stringByAppendingPathComponent:FILE_MARS_BIN];
            if (![[NSFileManager defaultManager] fileExistsAtPath:marsBinPath]) {
                // 不存在mars.bin
                MNLogInfo(@"%@:createMarsData:mars.bin not exist", TAG);
                return nil;
            }
            // 存在mars.json + mars.bin
            ret = [[MarsDataJsonBin alloc] initWithJsonPath:marsJsonPath BinPath:marsBinPath];
            ret.dirPath = dirPath;
            return ret;
        }
        MNLogInfo(@"%@:createMarsData:invalid", TAG);
        return nil;
    } @catch (NSException* exception) {
        MNLogInfo(@"%@:createMarsData:err %@", TAG, exception);
        return nil;
    }
}

- (MarsDataType)getType {
    return MarsDataType::UNKNOWN;
}

- (float)getAspect {
    float w = self.previewSize.width;
    float h = self.previewSize.height;
    if (w == 0 || h == 0) {
        return 1;
    }
    return w / h;
}

- (CGSize)getPreviewSize {
    return self.previewSize;
}

@end

#undef TAG

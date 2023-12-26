#import "MarsNativeBuilder.h"
#import "MarsNativePlayerImpl.h"
#import "PlaceHolderUtil.h"
#import "CommonUtil.h"
#import "MarsConstants.h"
#import "MarsAPLog.h"
#import "MarsTaskScheduleUtil.h"
#import "MarsNativeResourceLoader.h"
#import "MarsDataBase.h"
#import "CleanUtil.h"
#import "MarsMonitorUtil.h"

#define TAG @"MarsNativeBuilder"

@interface MarsNativeBuilder()

@property (nonatomic, strong) NSString* source;

@property (nonatomic, strong) NSString* url;

@property (nonatomic, strong) MarsNativePlayerImpl* player;

@property (nonatomic, assign) bool initialized;

@property (nonatomic, assign) bool initCallbackCalled;

@end

@implementation MarsNativeBuilder

- (instancetype)initWithSource:(NSString *)source Url:(NSString *)url {
    if (self = [super init]) {
        [MarsAPLogger Create];
        
        self.source = source;
        self.url = url;
        self.initialized = false;
        self.initCallbackCalled = false;
        
        self.repeatCount = 0;
        self.validTimestamp = -1;
        self.fixTickTime = YES;
        self.showPlaceHolderFirst = YES;
    }
    return self;
}

- (MarsNativePlayer*)build:(NSError**)err {
    if (!self.player) {
        if (![NSThread isMainThread]) {
            if (err) {
                MNLogInfo(@"%@:build:sub thread", TAG);
                *err = [CommonUtil makeError:@"MarsNativeBuilder" Code:0 Msg:@"build:sub thread"];
            }
            return nil;
        }
        self.player = [[MarsNativePlayerImpl alloc] initWithBuilder:self];
        MNLogInfo(@"build player success %p %@", self.player, self.url);
    }
    return self.player;
}

- (void)initPlayerWithOnShowPlaceHolder:(void(^ _Nullable)())onShowPlaceHolder Complete:(void(^)(bool success, NSError* _Nullable error))complete {
    if (!complete) {
        MNLogInfo(@"%@:initPlayer no callback", TAG);
        return;
    }
    if (!self.url) {
        MNLogInfo(@"%@:initPlayer empty url", TAG);
        complete(false, [CommonUtil makeError:TAG Code:ERROR_CREATE_PLAYER_FAIL Msg:@"initPlayer:empty url"]);
        return;
    }
    if (self.initialized) {
        MNLogInfo(@"%@:initPlayer duplicate init", TAG);
        complete(false, [CommonUtil makeError:TAG Code:ERROR_CREATE_PLAYER_FAIL Msg:@"initPlayer:duplicate init"]);
        return;
    }
    self.initialized = true;
    if (!self.player) {
        MNLogInfo(@"%@:initPlayer player is null", TAG);
        complete(false, [CommonUtil makeError:TAG Code:ERROR_CREATE_PLAYER_FAIL Msg:@"initPlayer:player is null"]);
        return;
    }
    if (![NSThread isMainThread]) {
        MNLogInfo(@"%@:initPlayer sub thread", TAG);
        complete(false, [CommonUtil makeError:TAG Code:ERROR_CREATE_PLAYER_FAIL Msg:@"initPlayer:sub thread"]);
        return;
    }
    
    [MarsTaskScheduleUtil postToNormalThread:^{
        // 1. 先检查placeHolder
        @try {
            __weak MarsNativeBuilder* weakSelf = self;
            [MNPlaceHolderUtil loadPlaceHolderWithUrl:self.placeHolderUrl
                                               Bitmap:self.placeHolderBitmap
                                               Source:self.source
                                             Complete:^(UIImage * _Nonnull bitmap, NSError * _Nonnull error) {
                __strong MarsNativeBuilder* thiz = weakSelf;
                if (!thiz) {
                    MNLogInfo(@"%@:initPlayer loadPlaceHolder builder freed,skip", TAG);
                    return;
                }
                thiz.placeHolderBitmap = bitmap;
                if (thiz.placeHolderBitmap) {
                    [thiz.player setPlaceHolderBitmap:thiz.placeHolderBitmap];
                    if (onShowPlaceHolder) {
                        onShowPlaceHolder();
                    }
                }
                if (error) {
                    [thiz notifyFailEvent:[CommonUtil makeError:TAG Code:ERROR_LOAD_BITMAP_FAIL Msg:error.description]
                                 Complete:complete];
                }
            }];
            
            // 2. 检查是否降级
            NSString* downgradeReason = [self.player checkDowngrade];
            if (downgradeReason) {
                [self notifyFailEvent:[CommonUtil makeError:TAG
                                                       Code:ERROR_DOWNGRADE
                                                        Msg:[NSString stringWithFormat:@"downgrade:%@", downgradeReason]]
                             Complete:complete];
                return;
            }
        } @catch (NSException *exception) {
            MNLogInfo(@"%@:initPlayer loadPlaceHolder..e %@", TAG, exception);
            [self notifyFailEvent:[CommonUtil makeError:TAG Code:ERROR_LOAD_BITMAP_FAIL Msg:exception.description]
                         Complete:complete];
            return;
        }
        
        // 3. 加载动画资源
        NSString* localDir = [MarsNativeResourceLoader loadFilePathFromLocal:self.url];
        if (localDir.length) {
            [self loadMarsDataFromDir:localDir Complete:complete];
            return;
        }
        [MarsNativeResourceLoader loadZip:self.url
                                  BizType:self.source
                           ValidTimeStamp:(self.validTimestamp / 1000.0) // 毫秒转成秒
                                      MD5:self.md5
                                 Complete:^(NSString * _Nonnull dirPath, NSError * _Nonnull error) {
            if (error) {
                MNLogInfo(@"%@:loadZip..e:%@", TAG, error);
                [self notifyFailEvent:[CommonUtil makeError:TAG Code:ERROR_LOAD_ZIP_FAIL Msg:error.description]
                             Complete:complete];
                return;
            }
            [self loadMarsDataFromDir:dirPath Complete:complete];
        }];
    }];
    
    MNLogInfo(@"%@:initPlayer running", TAG);
}

- (NSString*)getSource {
    return self.source;
}

- (NSString*)getUrl {
    return self.url;
}

//- (void)dealloc {
//    MNLogInfo(@"%@ dealloc", TAG);
//}

#pragma mark - private method

- (void)loadMarsDataFromDir:(NSString*)dirPath Complete:(void(^)(bool success, NSError* error))complete {
    MNLogInfo(@"%@:loadMarsDataFromDir %@", TAG, dirPath);
    MarsDataBase* marsData = [MarsDataBase createMarsData:dirPath];
    if (!marsData) {
        [self notifyFailEvent:[CommonUtil makeError:TAG Code:ERROR_CREATE_MARS_DATA_FAIL Msg:@"create mars data fail"]
                     Complete:complete];
        return;
    }
    [self.player initWithMarsData:marsData Complete:^(bool success, NSError * _Nonnull err) {
        if (success) {
            [self notifySuccessEvent:complete];
        } else {
            [self notifyFailEvent:[CommonUtil makeError:TAG Code:ERROR_CREATE_PLAYER_FAIL Msg:err.description]
                         Complete:complete];
        }
    }];
}

- (void)notifySuccessEvent:(void(^)(bool success, NSError* error))complete {
    NSString* sourceId = [self.player getSourceId];
    NSString* source = self.source;
    [MarsTaskScheduleUtil postToUIThread:^{
        if (self.initCallbackCalled) {
            MNLogInfo(@"%@:notifySuccessEvent duplicate", TAG);
            return;
        }
        self.initCallbackCalled = true;
        
        MNLogInfo(@"%@:notifySuccessEvent %@", TAG, self.player);
        [MarsMonitorUtil monitorMarsNativeStart:sourceId Source:source IsMarsPlayer:true];
        if (complete) {
            complete(true, nil);
        }
    } Delayd:0];
}

- (void)notifyFailEvent:(NSError*)error Complete:(void(^)(bool success, NSError* error))complete {
    NSString* sourceId = [MarsNativePlayerImpl getFormattedSourceId:self.url];
    [MarsTaskScheduleUtil postToUIThread:^{
        if (self.initCallbackCalled) {
            MNLogInfo(@"%@:notifyFailEvent duplicate", TAG);
            return;
        }
        self.initCallbackCalled = true;
        
        MNLogInfo(@"%@:notifyFailEvent %@ %@", TAG, error, self.player);
        [MarsMonitorUtil monitorMarsNativeError:sourceId
                                        errorType:@"init_error"
                                         errorMsg:error.description];
        if (complete) {
            complete(false, error);
        }
        MarsNativePlayerImpl* player = self.player;
        if (player) {
            [player setDowngrade];
        }
    } Delayd:0];
}

@end

#undef TAG

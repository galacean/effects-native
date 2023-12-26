#import "MarsNativePlayerImpl.h"
#import "MarsNativeBuilder.h"
#import "MarsConstants.h"
#import "MNTextureView.h"
#import "MNSampleBufferView.h"
#import "MarsTaskScheduleUtil.h"
#import "MarsAPLog.h"
#import "MarsEventEmitter.h"
#import "MarsSurfaceIOS.h"
#import "MarsConfigUtil.h"
#import "MarsDataJsonBin.h"
#import "CommonUtil.h"
#import "MarsNativeResourceLoader.h"
#import "ImageParser.h"
#import "MarsMonitorUtil.h"
#import "MarsDowngradeUtil.h"
#import "ImageProcessor.h"
#import "ImageParser.h"
#import <Accelerate/Accelerate.h>
#include "mars_player_mgr.h"
#include "scene_data_decoder_new.hpp"
#include "video_image_data.hpp"

static volatile int sPlayerIndex = 0;

static const NSString* sLockToken = @"event_token";

static NSObject* sCompleteIdxMutex = [[NSObject alloc] init];
static int sCompleteIdx = 0;

typedef void (^MNInitCompletion)(BOOL success, NSError* err);

typedef void (^MNPlayCompletion)(BOOL success, NSError* err);

typedef void (^MNFirstScreenCompletion)();

#define TAG @"Player"

#pragma mark - MNPlayerEventDelegate interface

@interface MNPlayerEventDelegate : NSObject<MarsEventDelegate>

@property (nonatomic, weak) MarsNativePlayerImpl* player;

- (instancetype)initWithPlayer:(MarsNativePlayerImpl*)player;

- (void)onEventType:(int)type Msg:(NSString*)msg;

@end

#pragma mark - MNPlayerPlayCompleteHolder

@interface MNPlayerPlayCompleteHolder : NSObject

@property (nonatomic, strong) MNPlayCompletion completionBlock;

@property (nonatomic, strong) NSString* token;

- (instancetype)initWithCompleteBlock:(MNPlayCompletion)block;

@end

@implementation MNPlayerPlayCompleteHolder

- (instancetype)initWithCompleteBlock:(MNPlayCompletion)block {
    if (self = [super init]) {
        @synchronized (sCompleteIdxMutex) {
            self.token = [NSString stringWithFormat:@"p%d", sCompleteIdx];
            ++sCompleteIdx;
        }
        self.completionBlock = block;
    }
    return self;
}

@end

#pragma mark - MarsNativePlayerImpl interface

@interface MarsNativePlayerImpl() <MarsEventDelegate>

@property (nonatomic, strong) NSString* source;

/// formatted
@property (nonatomic, strong) NSString* url;

@property (nonatomic, strong) NSString* scene;

@property (nonatomic, assign) int deviceLevel;

@property (nonatomic, assign) int nativePlayer;

@property (nonatomic, assign) void* sceneDataPtr;

@property (nonatomic, strong) UIView* marsView;

@property (nonatomic, assign) int repeatCount;

@property (nonatomic, assign) int frameCount;

@property (nonatomic, assign) bool isDowngrade;

@property (nonatomic, strong) UIImageView* placeHolder;

@property (nonatomic, assign) BOOL placeHolderVisiable;

@property (nonatomic, assign) float aspect;

@property (nonatomic, assign) float duration;

@property (nonatomic, assign) BOOL fixTickTime;

@property (nonatomic, strong) MarsDataBase* marsData;

/// 动态参数
@property (nonatomic, strong) NSDictionary* variables;

@property (nonatomic, strong) NSDictionary* variablesBitmap;

@property (nonatomic, strong) id<MarsNativePlayerEventListener> eventListener;

@property (nonatomic, strong) NSObject* ptrMutex;

@property (nonatomic, strong) MNInitCompletion initCallback;

@property (nonatomic, strong) MNFirstScreenCompletion firstScreenCallback;

@property (nonatomic, strong) MNPlayerPlayCompleteHolder* playCallbackHolder;

@property (nonatomic, strong) MNPlayerEventDelegate* eventDelegate;

@property (nonatomic, strong) NSMutableArray* extensions;

@end

#pragma mark - MNPlayerEventDelegate @implementation

@implementation MNPlayerEventDelegate

- (instancetype)initWithPlayer:(MarsNativePlayerImpl*)player {
    if (self = [super init]) {
        _player = player;
    }
    return self;
}

- (void)onEventType:(int)type Msg:(NSString*)msg {
    __strong MarsNativePlayerImpl* strongPlayer = _player;
    if (strongPlayer) {
        [strongPlayer onEventType:type Msg:msg];
    }
}

@end

#pragma mark - MarsNativePlayerImpl @implementation

@implementation MarsNativePlayerImpl

- (instancetype)initWithBuilder:(MarsNativeBuilder*)builder {
    if (self = [super initWithFrame:CGRectMake(0, 0, 1, 1)]) {
        self.source = [builder getSource];
        self.url = [MarsNativePlayerImpl getFormattedSourceId:[builder getUrl]];
        self.scene = builder.scene;
        self.repeatCount = builder.repeatCount;
        self.variables = builder.variables;
        self.variablesBitmap = builder.variablesBitmap;
        
        self.deviceLevel = DEVICE_LEVEL_HIGH;
        self.nativePlayer = -1;
        self.sceneDataPtr = nullptr;
        self.frameCount = 0;
        self.isDowngrade = false;
        self.placeHolderVisiable = builder.showPlaceHolderFirst;
        self.aspect = 1;
        self.duration = 0;
        self.ptrMutex = [[NSObject alloc] init];
        self.extensions = [[NSMutableArray alloc] init];
        self.fixTickTime = builder.fixTickTime;
        
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didEnterBackground:) name:UIApplicationDidEnterBackgroundNotification object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didEnterForeground:) name:UIApplicationWillEnterForegroundNotification object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(willResignActive:) name:UIApplicationWillResignActiveNotification object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didBecomeActive:) name:UIApplicationDidBecomeActiveNotification object:nil];
    }
    return self;
}

- (NSString*)checkDowngrade {
    NSString* downgradeReason = nil;
    // 先检查总降级开关
    BOOL shouldDowngrade = [[MarsConfigUtil shareInstance] forceDowngradeMarsNative];
    downgradeReason = @"forceDowngradeMN";
    if (!shouldDowngrade) {
        // 检查资源纬度降级开关
        if (!self.scene.length) {
            // 没有场景码，按资源降级
            shouldDowngrade = [[MarsConfigUtil shareInstance] forceDowngradeByResId:self.url];
            downgradeReason = [NSString stringWithFormat:@"forceDowngradeByResId,%@", self.url];
        } else {
            // 有场景码，按场景降级
            shouldDowngrade = [[MarsConfigUtil shareInstance] forceDowngradeByResScene:self.scene];
            downgradeReason = [NSString stringWithFormat:@"forceDowngradeByResScene,%@", self.scene];
        }
    }
    if (!shouldDowngrade) {
        // 没有开关降级，检查统一降级
        MarsDowngradeResult* downgradeResult = [MarsDowngradeUtil getDowngradeResult:self.url];
        if ([downgradeResult getDowngrade]) {
            // 统一降级降级了
            shouldDowngrade = YES;
            downgradeReason = [NSString stringWithFormat:@"getDowngradeResult,%@", [downgradeResult getReason]];
        } else {
            // 没降级，获取设备等级
            self.deviceLevel = [MarsDowngradeUtil getDeviceLevel];
            MNLogInfo(@"%@:getDowngradeResult,deviceLevel %d", TAG, self.deviceLevel);
        }
    }
    
    if (shouldDowngrade) {
        [MarsTaskScheduleUtil postToUIThread:^{
            [self setDowngrade];
        } Delayd:0];
        return downgradeReason;
    }
    return nil;
}

- (void)initWithMarsData:(MarsDataBase*)marsData Complete:(void(^)(bool, NSError*))complete {
    MNLogInfo(@"%@:initWithMarsData,type=%lu", TAG, [marsData getType]);
    
    if (self.isDowngrade) {
        return;
    }
    
    self.marsData = marsData;
    
    __weak MarsNativePlayerImpl* weakSelf = self;
    [MarsTaskScheduleUtil postToUIThread:^{
        __strong MarsNativePlayerImpl* thiz = weakSelf;
        if (!thiz) {
            return;
        }
        @synchronized (thiz.ptrMutex) {
            thiz.aspect = [marsData getAspect];
            thiz.duration = marsData.duration;
            thiz.frameCount = (int) (marsData.duration * 30);
            thiz.initCallback = complete;
            
            thiz.nativePlayer = [MarsNativePlayerImpl generatePlayerIndex];
            self.eventDelegate = [[MNPlayerEventDelegate alloc] initWithPlayer:thiz];
            [MarsEventEmitter registerPlayerId:thiz.nativePlayer Listener:self.eventDelegate];
            mn::MarsPlayerConstructor options;
            options.surface = new mn::MarsSurfaceIOS();
            {
                mn::SystemSurface system_surface;
                system_surface.surface_type = mn::kSurfaceTypeGL;
                
                
                CGFloat screenScale = [[UIScreen mainScreen] scale];
                if (screenScale == 3 && [[MarsConfigUtil shareInstance] forceDowngradeScreenDpi:self.url]) {
                    screenScale = 1.5;
                }
                if ([[MarsConfigUtil shareInstance] useAVSimpleBufferLayer:[self getSourceId]]) {
                    thiz.marsView = [[MNSampleBufferView alloc] initWithFrame:CGRectMake(0, 0, marsData.previewSize.width, marsData.previewSize.height) Scale:screenScale];
                } else {
                    thiz.marsView = [[MNTextureView alloc] initWithFrame:CGRectMake(0, 0, marsData.previewSize.width, marsData.previewSize.height) Scale:screenScale];
                }
                [thiz addSubview:thiz.marsView];
                
                system_surface.surface = (__bridge void*)thiz.marsView;
                options.surface->Create(system_surface, thiz.marsView.frame.size.width, thiz.marsView.frame.size.height);
            }
            options.lock_tick_event = true;
            options.config.enable_will_pause = true;
            options.config.render_level = [[MarsConfigUtil shareInstance] getRenderLevelWithDefaultVal:thiz.deviceLevel];
            options.config.fix_tick_time = self.fixTickTime;
            mn::MarsPlayerManager::CreateNew(thiz.nativePlayer, options, true);
            for (int i = 0; i < thiz.extensions.count; i++) {
                NSString* name = nil;
                void* plugin = [thiz.extensions[i] getCustomPlugin:&name];
                if (plugin) {
                    mn::MarsPlayerManager::AddPlugin(thiz.nativePlayer, plugin, name.UTF8String);
                }
            }
            
            [thiz tryLoadImages:marsData];
            
            mn::MarsPlayerManager::SetRepeatCount(thiz.nativePlayer, thiz.repeatCount);
            
            if ([marsData getType] == MarsDataType::JSON_BIN) {
                thiz.sceneDataPtr = mn::SceneDataDecoderNew::CreateNewByPath([((MarsDataJsonBin*) marsData) getBinPath].UTF8String);
            }
            MNLogInfo(@"%@:initWithMarsData:mSceneDataPtr %p", TAG, thiz.sceneDataPtr);
            if (!thiz.sceneDataPtr) {
                complete(false, [CommonUtil makeError:TAG Code:0 Msg:@"create scene data fail"]);
                return;
            }
            for (int i = 0; i < thiz.extensions.count; i++) {
                NSError* error = nil;
                [thiz.extensions[i] onSceneDataCreated:thiz.sceneDataPtr Error:&error];
                if (error) {
                    complete(false, error);
                    return;
                }
            }
        }
    } Delayd:0];
}

- (void)setPlaceHolderBitmap:(UIImage*)bitmap {
    MNLogInfo(@"%@:setPlaceHolder,%@", TAG, bitmap);
    if (!self.placeHolder) {
        self.placeHolder = [[UIImageView alloc] initWithFrame:CGRectMake(0, 0, self.frame.size.width, self.frame.size.height)];
        [self.placeHolder setContentMode:UIViewContentModeScaleToFill];
        [self addSubview:self.placeHolder];
        [self.placeHolder setHidden:(!self.placeHolderVisiable)];
    }
    [self.placeHolder setImage:bitmap];
}

- (void)setDowngrade {
    MNLogInfo(@"%@:setDowngrade %@", TAG, self);
    self.isDowngrade = true;
    [self showPlaceHolder];
    [self destroy];
}

- (NSString*)getSourceId {
    return self.url;
}

- (void)play {
    [self playWithStartFrame:0 EndFrame:self.frameCount RepeatCount:self.repeatCount Complete:nil];
}

- (void)playWithComplete:(void(^)(bool, NSError*))complete {
    [self playWithStartFrame:0 EndFrame:self.frameCount RepeatCount:self.repeatCount Complete:complete];
}

- (void)playWithRepeatCount:(int)repeatCount Complete:(void(^)(bool, NSError*))complete {
    [self playWithStartFrame:0 EndFrame:self.frameCount RepeatCount:repeatCount Complete:complete];
}

- (void)playWithStartFrame:(int)startFrame
                  EndFrame:(int)endFrame
               RepeatCount:(int)repeatCount
                  Complete:(void(^)(bool success, NSError* error))complete {
    if (self.isDowngrade) {
        return;
    }
    MNLogInfo(@"%@:play %@", TAG, self);
    
    self.repeatCount = repeatCount;
    
    self.playCallbackHolder = [[MNPlayerPlayCompleteHolder alloc] initWithCompleteBlock:complete];
    mn::MarsPlayerManager::SetRepeatCount(self.nativePlayer, self.repeatCount);
    mn::MarsPlayerManager::PlayMarsFrameControl(self.nativePlayer, startFrame, endFrame, self.playCallbackHolder.token.UTF8String);
}

- (void)stop {
    if (self.isDowngrade) {
        return;
    }
    MNLogInfo(@"%@:stop %@", TAG, self);
    mn::MarsPlayerManager::StopMars(self.nativePlayer);
}

- (void)pause {
    if (self.isDowngrade) {
        return;
    }
    MNLogInfo(@"%@:pause %@", TAG, self);
    mn::MarsPlayerManager::OnEvent(self.nativePlayer, NATIVE_EVENT_PAUSE);
}

- (void)resume {
    if (self.isDowngrade) {
        return;
    }
    MNLogInfo(@"%@:resume %@", TAG, self);
    mn::MarsPlayerManager::OnEvent(self.nativePlayer, NATIVE_EVENT_RESUME);
}

- (void)destroy {
    @synchronized (self.ptrMutex) {
        MNLogInfo(@"%@:destroy,%d %@", TAG, self.nativePlayer, self);
        if (self.nativePlayer != -1) {
            mn::MarsPlayerManager::DestroyMars(self.nativePlayer);
            [MarsEventEmitter unregisterPlayerId:self.nativePlayer];
            self.eventDelegate.player = nil;
            self.nativePlayer = -1;
        }
        if (self.sceneDataPtr) {
            delete ((mn::SceneDataDecoder*) self.sceneDataPtr);
            self.sceneDataPtr = nullptr;
        }
        self.marsView = nil;
        self.initCallback = nil;
        self.playCallbackHolder = nil;
        
        // 销毁插件
        for (int i = 0; i < self.extensions.count; i++) {
            [self.extensions[i] onDestroy];
        }
        [self.extensions removeAllObjects];
    }
}

- (float)getAspect {
    return self.aspect;
}

- (CGSize)getPreviewSize {
    return [self.marsData getPreviewSize];
}

- (float)getDuration {
    return self.duration;
}

- (void)addExtension:(id<MarsNativeExtension>)extension {
    [self.extensions addObject:extension];
}

- (void)onFirstScreen:(void(^)())callback {
    self.firstScreenCallback = callback;
}

- (BOOL)updateVariable:(NSString*)key WithImage:(UIImage*)image {
    if (!image) {
        MNLogInfo(@"update variable: image is null");
        return NO;
    }
    
    NSArray<MarsImageInfo*>* imgs = self.marsData.images;
    int templateIdx = -1;
    for (int i = 0; i < imgs.count; i++) {
        MarsImageInfo* img = [imgs objectAtIndex:i];
        if ([img.url isEqualToString:key]) {
            templateIdx = img.templateIdx;
            break;
        }
    }
    if (templateIdx == -1) {
        MNLogInfo(@"update variable: tempalteIdx is -1");
        return NO;
    }
    
    vImage_Buffer dest_buffer = {0};
    [ImageParser parseUIImageToBitmap:&dest_buffer image:image];
    ImageRawData* raw_data = new ImageRawData((uint8_t*)dest_buffer.data, dest_buffer.width, dest_buffer.height);
    raw_data->premultiply_alpha = true;
    
    return mn::MarsPlayerManager::UpdateMarsImage(self.nativePlayer, templateIdx, raw_data);
}

+ (int)generatePlayerIndex {
    int ret;
    @synchronized (sLockToken) {
        ret = ++sPlayerIndex;
    }
    return ret;
}

- (void)dealloc {
    MNLogInfo(@"%@:dealloc", TAG);
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [self destroy];
}

#pragma mark - private method

- (void)hidePlaceHolder {
    if (self.placeHolder) {
        [self.placeHolder setHidden:YES];
    }
    self.placeHolderVisiable = NO;
}

- (void)showPlaceHolder {
    if (self.placeHolder) {
        [self.placeHolder setHidden:NO];
    }
    self.placeHolderVisiable = YES;
}

- (void)tryLoadImages:(MarsDataBase*)marsData {
    NSArray<MarsImageInfo*>* images = marsData.images;
    if (!images || images.count == 0) {
        MNLogInfo(@"%@:tryLoadImages skip", TAG);
        [MarsTaskScheduleUtil postToNormalThread:^{
            [self setupImages:[[MarsImageProcessor alloc] initWithResList:nil]];
        }];
        return;
    }
    NSMutableArray<MarsNativeImageResource*>* resList = [[NSMutableArray alloc] init];
    // 是否出现过templateIdx，如果出现过，后面应全是数据模版；如果后面出现非数据模版，应忽略
    bool hasTemplateIdx = false;
    for (int i = 0; i < images.count; i++) {
        MarsImageInfo* info = [images objectAtIndex:i];
        NSString* url = info.url;
        if (info.astc.length) {
            url = info.astc;
        }
        MarsNativeImageResource* res = [[MarsNativeImageResource alloc] initWithUrl:url];
        res.isVideo = info.isVideo;
        res.isTransparentVideo = info.isTransparentVideo;
        if (res.isVideo) {
            res.videoHardDecoder = [[MarsConfigUtil shareInstance] enableVideoHardDecoder:url];
        }
        if (info.templateIdx >= 0) {
            hasTemplateIdx = true;
            res = [resList objectAtIndex:info.templateIdx];
            if (!res) {
                continue;
            }
            res.key = info.url; // 如果设置了数据模版，替换掉之前的res的key
        }
        if (!hasTemplateIdx) {
            [resList addObject:res];
        }
    }
    MNLogInfo(@"%@:tryLoadImages count:%lu", TAG, resList.count);
    [MarsNativeResourceLoader loadImages:resList
                                 DirPath:marsData.dirPath
                               Variables:self.variables
                         VariablesBitmap:self.variablesBitmap
                               PlayerIdx:self.nativePlayer
                                 BizType:self.source
                                Complete:^(NSError * _Nonnull error) {
        if (error) {
            MNLogInfo(@"%@:tryLoadImages..e:%@", TAG, error);
            [MarsTaskScheduleUtil postToNormalThread:^{
                // 下载图片失败
                [self setupImages:nil];
            }];
            return;
        }
        MNLogInfo(@"%@:tryLoadImages success", TAG);
        [MarsTaskScheduleUtil postToNormalThread:^{
            MarsImageProcessor* processor = [[MarsImageProcessor alloc] initWithResList:resList];
            if (!marsData.fonts || !marsData.fonts.count) {
                // 没有字体，跳过
                [self setupImages:processor];
            } else {
                [self tryLoadFonts:marsData DirPath:marsData.dirPath Images:processor];
            }
        }];
    }];
}

- (void)tryLoadFonts:(MarsDataBase*)marsData DirPath:(NSString*)dirPath Images:(MarsImageProcessor*)imageProcessor {
    NSMutableArray<NSString*>* urlList = [[NSMutableArray alloc] init];
    NSArray<MarsFontInfo*>* fonts = marsData.fonts;
    for (int i = 0; i < fonts.count; i++) {
        MarsFontInfo* font = [fonts objectAtIndex:i];
        if (font.url && font.url.length > 0) {
            [urlList addObject:font.url];
        }
    }
    [imageProcessor setTexts:marsData.texts Fonts:marsData.fonts];
    if (!urlList.count) {
        // 没有线上字体文件，跳过
        [self setupImages:imageProcessor];
        return;
    }
    
    [MarsNativeResourceLoader loadFonts:urlList
                                DirPath:dirPath
                                BizType:self.source
                               Complete:^(NSDictionary<NSString *, NSData *> * fontDataMap, NSError * error) {
        if (error) {
            MNLogInfo(@"%@:tryLoadFonts..e:%@", TAG, error);
            [MarsTaskScheduleUtil postToNormalThread:^{
                [self setupImages:nil];
            }];
            return;
        }
        
        MNLogInfo(@"%@:tryLoadFonts success", TAG);
        [MarsTaskScheduleUtil postToNormalThread:^{
            [imageProcessor setFontDataMap:fontDataMap];
            [self setupImages:imageProcessor];
        }];
    }];
}

- (void)setupImages:(MarsImageProcessor*)imageProcessor {
    @synchronized (self.ptrMutex) {
        if (!self.sceneDataPtr) {
            MNLogInfo(@"%@:setupImages ptr is null", TAG);
            return;
        }
        if (!imageProcessor) {
            MNLogInfo(@"%@:setupImages processor is null", TAG);
            if (self.initCallback) {
                self.initCallback(false, [CommonUtil makeError:TAG Code:0 Msg:@"download images fail"]);
            }
            return;
        }
        NSDictionary<NSString*, MarsImageDataInfo*>* imageDataMap = [imageProcessor processWithVariables:self.variables];
        if (!imageDataMap) {
            MNLogInfo(@"%@:setupImages dataMap is null", TAG);
            if (self.initCallback) {
                self.initCallback(false, [CommonUtil makeError:TAG Code:0 Msg:@"load images fail"]);
            }
            return;
        }
        mn::SceneDataDecoder* scene_data_decoder = (mn::SceneDataDecoder*) self.sceneDataPtr;
        for (NSString* key in imageDataMap) {
            MarsImageDataInfo* info = [imageDataMap objectForKey:key];
            NSData* data = info.data;
            if (info.isVideo) {
                // 视频纹理
                mn::VideoImageRawData* video_data = new mn::VideoImageRawData((mn::VideoContext*) info.videoRes.videoContext);
                info.videoRes.videoContext = nullptr;
                scene_data_decoder->SetImageData(key.UTF8String, video_data);
            } else if (info.isKtx) {
                // 压缩纹理直接塞进gl
                uint8_t* image_buffer = (uint8_t*)malloc(data.length);
                memcpy(image_buffer, data.bytes, data.length);
                ImageRawData* image_raw_data = new ImageRawData(image_buffer, data.length);
                scene_data_decoder->SetImageData(key.UTF8String, image_raw_data);
            } else {
                uint8_t* image_buffer = (uint8_t*)malloc(data.length);
                memcpy(image_buffer, data.bytes, data.length);
                ImageRawData* image_raw_data = new ImageRawData(image_buffer, info.width, info.height);
                image_raw_data->premultiply_alpha = true;
                scene_data_decoder->SetImageData(key.UTF8String, image_raw_data);
            }
        }
        [imageProcessor releaseData];
        mn::MarsPlayerManager::SetSceneData(self.nativePlayer, (mn::SceneDataDecoder*) self.sceneDataPtr);
        self.sceneDataPtr = nullptr;
        if (self.initCallback) {
            self.initCallback(true, nil);
            self.initCallback = nil;
        }
    }
}

// todo: 检查eventEmitter持有会不会有循环引用
- (void)onEventType:(int)type Msg:(nonnull NSString *)msg {
    MNLogInfo(@"%@:onEvent %d,%@", TAG, type, msg);
    __weak MarsNativePlayerImpl* weakSelf = self;
    NSString* url = self.url;
    switch (type) {
        case PLATFORM_EVENT_STATISTICS: {
            [MarsTaskScheduleUtil postToNormalThread:^{
                if (!msg || msg.length == 0) {
                    MNLogInfo(@"%@:onEvent1 empty message", TAG);
                    return;
                }
                NSArray* arr = [msg componentsSeparatedByString:@"_"];
                if (arr.count < 2) {
                    MNLogInfo(@"%@:onEvent1 invalid message %@", TAG, msg);
                    return;
                }
                bool supportKtx = [[arr objectAtIndex:0] isEqualToString:@"true"];
                int glesVersion = [[arr objectAtIndex:1] intValue];
                [MarsMonitorUtil monitorMarsNativeStatistics:url supportCompressedTexture:supportKtx glesVersion:glesVersion];
            }];
        }
            break;
            
        case PLATFORM_EVENT_START: {
            [MarsTaskScheduleUtil postToUIThread:^{
                __strong MarsNativePlayerImpl* thiz = weakSelf;
                if (!thiz) {
                    return;
                }
                [thiz hidePlaceHolder];
                if (thiz.firstScreenCallback) {
                    thiz.firstScreenCallback();
                    thiz.firstScreenCallback = nil;
                }
            } Delayd:0];
        }
            break;
            
        case PLATFORM_EVENT_THREAD_START: {
            [MarsDowngradeUtil writeResourceIdBegin:self.url ThreadName:msg];
        }
            break;
            
        case PLATFORM_EVENT_THREAD_END:{
            [MarsDowngradeUtil writeResourceIdFinish:self.url ThreadName:msg];
        }
            break;
            
        case PLATFORM_EVENT_ANIMATION_END: {
            [MarsTaskScheduleUtil postToUIThread:^{
                __strong MarsNativePlayerImpl* thiz = weakSelf;
                if (!thiz) {
                    return;
                }
                MNPlayCompletion callback = thiz.playCallbackHolder.completionBlock;
                if (callback && [thiz.playCallbackHolder.token isEqualToString:msg]) {
                    callback(true, nil);
                }
            } Delayd:0];
        }
            break;
            
        case PLATFORM_EVENT_INTERACT_MESSAGE_BEGIN:
        case PLATFORM_EVENT_INTERACT_MESSAGE_END: {
            [MarsTaskScheduleUtil postToUIThread:^{
                __strong MarsNativePlayerImpl* thiz = weakSelf;
                if (!thiz) {
                    return;
                }
                id<MarsNativePlayerEventListener> listener = thiz.eventListener;
                if (listener && [listener respondsToSelector:@selector(onMessageItem:Phrase:)]) {
                    [listener onMessageItem:msg
                                     Phrase:(type == PLATFORM_EVENT_INTERACT_MESSAGE_BEGIN) ? @"MESSAGE_ITEM_PHRASE_BEGIN" : @"MESSAGE_ITEM_PHRASE_END"];
                }
            } Delayd:0];
        }
            break;
            
        case PLATFORM_EVENT_EGL_INIT_ERROR:
        case PLATFORM_EVENT_RUNTIME_ERROR: {
            [MarsTaskScheduleUtil postToUIThread:^{
                MarsNativePlayerImpl* thiz = weakSelf;
                if (!thiz) {
                    return;
                }
                MNPlayCompletion callback = thiz.playCallbackHolder.completionBlock;
                if (callback) {
                    callback(false, [CommonUtil makeError:TAG Code:-1 Msg:msg]);
                }
                [self setDowngrade];
            } Delayd:0];
            [MarsTaskScheduleUtil postToNormalThread:^{
                [MarsMonitorUtil monitorMarsNativeError:url errorType:@"runtime_error" errorMsg:msg];
            }];
        }
            break;
            
        default:
            break;
    }
}

- (void)layoutSubviews {
    CGSize size = self.frame.size;
    if (self.marsView) {
        int width = size.width;
        int height = size.height;
        [self.marsView setFrame:CGRectMake(0, 0, width, height)];
        if (width == 0) {
            width = 1;
        }
        if (height == 0) {
            height = 1;
        }
        mn::MarsPlayerManager::OnSurfaceResize(self.nativePlayer, width, height);
    }
    if (self.placeHolder) {
        [self.placeHolder setFrame:CGRectMake(0, 0, size.width, size.height)];
    }
}

+ (NSString*)getFormattedSourceId:(NSString*)url {
    if (url.length > 32 && ([url hasPrefix:@"http://"] || [url hasPrefix:@"https://"])) {
        if ([url containsString:@"A*"]) {
            NSRange range = [url rangeOfString:@"A*"];
            return [url substringWithRange:NSMakeRange(range.location, MIN(32, url.length - range.location))];
        }
    }
    return url;
}

#pragma mark - AppEvent

- (void)willMoveToWindow:(UIWindow *)newWindow {
    // todo: 不在window上时不渲染
    MNLogInfo(@"%@:willMoveToWindow %@", TAG, newWindow);
}

- (void)didEnterBackground:(NSNotification *)sender {
    MNLogInfo(@"%@:didEnterBackground", TAG);
    mn::MarsPlayerManager::OnEvent(self.nativePlayer, 1);
}

- (void)didEnterForeground:(NSNotification *)sender {
    MNLogInfo(@"%@:didEnterForeground", TAG);
    mn::MarsPlayerManager::OnEvent(self.nativePlayer, 2);
}

- (void)willResignActive:(NSNotification *)sender {
    MNLogInfo(@"%@:willResignActive", TAG);
    mn::MarsPlayerManager::OnEvent(self.nativePlayer, 1);
}

- (void)didBecomeActive:(NSNotification *)sender {
    MNLogInfo(@"%@:didBecomeActive", TAG);
    mn::MarsPlayerManager::OnEvent(self.nativePlayer, 2);
}

@end

#pragma mark - MarsNativePlayer

@implementation MarsNativePlayer

- (void)play {
    @throw [NSException exceptionWithName:@"no implement" reason:@"no implement" userInfo:nil];
}

- (void)playWithComplete:(void(^)(bool success, NSError* error))complete {
    @throw [NSException exceptionWithName:@"no implement" reason:@"no implement" userInfo:nil];
}

- (void)playWithRepeatCount:(int)repeatCount
                   Complete:(void(^)(bool success, NSError* error))complete {
    @throw [NSException exceptionWithName:@"no implement" reason:@"no implement" userInfo:nil];
}

- (void)playWithStartFrame:(int)startFrame
                  EndFrame:(int)endFrame
               RepeatCount:(int)repeatCount
                  Complete:(void(^)(bool success, NSError* error))complete {
    @throw [NSException exceptionWithName:@"no implement" reason:@"no implement" userInfo:nil];
}

- (void)stop {
    @throw [NSException exceptionWithName:@"no implement" reason:@"no implement" userInfo:nil];
}

- (void)pause {
    @throw [NSException exceptionWithName:@"no implement" reason:@"no implement" userInfo:nil];
}

- (void)resume {
    @throw [NSException exceptionWithName:@"no implement" reason:@"no implement" userInfo:nil];
}

- (void)destroy {
    @throw [NSException exceptionWithName:@"no implement" reason:@"no implement" userInfo:nil];
}

- (float)getAspect {
    @throw [NSException exceptionWithName:@"no implement" reason:@"no implement" userInfo:nil];
    return 1;
}

- (CGSize)getPreviewSize {
    @throw [NSException exceptionWithName:@"no implement" reason:@"no implement" userInfo:nil];
    return CGSizeMake(0.0f, 0.0f);
}

- (float)getDuration {
    @throw [NSException exceptionWithName:@"no implement" reason:@"no implement" userInfo:nil];
    return 0;
}

- (void)setEventListener:(id<MarsNativePlayerEventListener>)listener {
    @throw [NSException exceptionWithName:@"no implement" reason:@"no implement" userInfo:nil];
}

- (void)addExtension:(id<MarsNativeExtension>)extension {
    @throw [NSException exceptionWithName:@"no implement" reason:@"no implement" userInfo:nil];
}

- (void)onFirstScreen:(void(^)())callback {
    @throw [NSException exceptionWithName:@"no implement" reason:@"no implement" userInfo:nil];
}

- (BOOL)updateVariable:(NSString*)key WithImage:(UIImage*)image {
    @throw [NSException exceptionWithName:@"no implement" reason:@"no implement" userInfo:nil];
    return NO;
}

@end

#undef TAG

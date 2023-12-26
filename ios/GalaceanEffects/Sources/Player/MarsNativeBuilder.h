#import <UIKit/UIKit.h>
#import <MarsNativePlayer.h>

NS_ASSUME_NONNULL_BEGIN

@interface MarsNativeBuilder : NSObject

@property (nonatomic, strong) NSString* placeHolderUrl; // 降级图url 或 djangoId 或 本地路径 或 包内路径

@property (nonatomic, strong) UIImage* placeHolderBitmap; // 降级图bitmap

@property (nonatomic, strong) NSString* scene; // 业务场景码，用于针对某个业务场景单独降级

@property (nonatomic, strong) NSDictionary* variables; // 可变参数表

@property (nonatomic, strong) NSDictionary* variablesBitmap; // 可变参数表，用于传UIImage

@property (nonatomic, assign) int repeatCount; // 默认为0，只播放1次；小于0则循环播放

@property (nonatomic, assign) long validTimestamp;

@property (nonatomic, strong) NSString* md5;

@property (nonatomic, assign) BOOL fixTickTime;

@property (nonatomic, assign) BOOL showPlaceHolderFirst;

- (instancetype)initWithSource:(NSString*)source Url:(NSString*)url;

- (MarsNativePlayer*)build:(NSError**)err;

- (void)initPlayerWithOnShowPlaceHolder:(void(^ _Nullable)())onShowPlaceHolder Complete:(void(^)(bool success, NSError* _Nullable error))complete;

- (NSString*)getSource;

- (NSString*)getUrl;

@end

NS_ASSUME_NONNULL_END

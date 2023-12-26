#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class MarsNativeVideoResource;

@interface MarsImageDataInfo : NSObject

@property (nonatomic, assign) BOOL isKtx;
@property (nonatomic, assign) BOOL isVideo;
@property (nonatomic, strong) NSData* data;
@property (nonatomic, strong) MarsNativeVideoResource* videoRes;
@property (nonatomic, assign) int width;
@property (nonatomic, assign) int height;

@end

@interface MarsImageProcessor : NSObject

- (instancetype)initWithResList:(NSArray* _Nullable)resList;

- (void)setTexts:(NSArray*)texts Fonts:(NSArray*)fonts;

- (void)setFontDataMap:(NSDictionary<NSString*, NSData*>*)fontDataMap;

- (NSDictionary<NSString*, MarsImageDataInfo*>*)processWithVariables:(NSDictionary*)variables;

- (void)releaseData;

@end

NS_ASSUME_NONNULL_END

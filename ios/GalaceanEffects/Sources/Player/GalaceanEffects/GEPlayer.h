#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface GEPlayerParams : NSObject

@property (nonatomic, strong) NSString *url;

@property (nonatomic, strong) UIImage *downgradeImage;

@property (nonatomic, strong) NSDictionary *variables;

@property (nonatomic, strong) NSDictionary *variablesBitmap;

@end

@interface GEPlayer : UIView

- (instancetype)initWithParams:(GEPlayerParams *)params;

- (void)loadScene:(void(^)(bool success, NSString *errorMsg))callback;

- (void)playWithRepeatCount:(int)repeatCount
                   Callback:(void(^ _Nullable)(bool success, NSString *errorMsg))callback;

- (void)playWithFromFrame:(int)fromFrame
                  ToFrame:(int)toFrame
              RepeatCount:(int)repeatCount
                 Callback:(void(^ _Nullable)(bool success, NSString *errorMsg))callback;

- (void)pause;

- (void)resume;

- (void)stop;

- (void)destroy;

- (float)getAspect;

- (int)getFrameCount;

@end

NS_ASSUME_NONNULL_END

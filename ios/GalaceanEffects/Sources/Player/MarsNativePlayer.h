#ifndef MarsNativePlayer_h
#define MarsNativePlayer_h

#import <UIKit/UIKit.h>

@protocol MarsNativePlayerEventListener <NSObject>

@optional
/// 监听动画message
/// - Parameters:
///   - itemName: 元素id
///   - phrase: 事件类型，MESSAGE_ITEM_PHRASE_BEGIN元素创建，MESSAGE_ITEM_PHRASE_END元素销毁
- (void)onMessageItem:(NSString*)itemName Phrase:(NSString*)phrase;

@end

@protocol MarsNativeExtension <NSObject>

@required
- (BOOL)onSceneDataCreated:(void*)sceneData Error:(NSError**)error;

- (void*)getCustomPlugin:(NSString**)name;

- (void)onDestroy;

@end

@interface MarsNativePlayer : UIView

- (void)play;

- (void)playWithComplete:(void(^)(bool success, NSError* error))complete;

- (void)playWithRepeatCount:(int)repeatCount
                   Complete:(void(^)(bool success, NSError* error))complete;

- (void)playWithStartFrame:(int)startFrame
                  EndFrame:(int)endFrame
               RepeatCount:(int)repeatCount
                  Complete:(void(^)(bool success, NSError* error))complete;

- (void)stop;

- (void)pause;

- (void)resume;

- (void)destroy;

- (float)getAspect;

- (CGSize)getPreviewSize;

- (float)getDuration;

- (void)setEventListener:(id<MarsNativePlayerEventListener>)listener;

- (void)addExtension:(id<MarsNativeExtension>)extension;

- (void)onFirstScreen:(void(^)())callback;

- (BOOL)updateVariable:(NSString*)key WithImage:(UIImage*)image;

@end

#endif /* MarsNativePlayer_h */

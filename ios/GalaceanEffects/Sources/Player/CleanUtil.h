#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface MNCleanUtil : NSObject

/// 尝试清理mars资源目录
+ (void)tryCleanMarsFiles;

/// 是否关闭清理能力
+ (BOOL)disabled;

@end

NS_ASSUME_NONNULL_END

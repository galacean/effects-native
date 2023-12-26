#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

// 文件操作收口
@interface MNFileOperation : NSObject

/// 检查本地资源目录是否存在
/// - Parameters:
///   - dirPath: 资源目录绝对路径
///   - needUpdate: 是否更新时间戳
+ (BOOL)checkDirExist:(NSString*)dirPath UpdateTimeStamp:(BOOL)needUpdate Time:(NSTimeInterval)timeStamp;

/// 下载zip
/// - Parameters:
///   - zipUrl: zip url
///   - bizType: bizType，埋点用
///   - dstPath: 资源目录绝对路径
///   - complete: 回调
+ (void)downloadAndUnzip:(NSString*)zipUrl
                 BizType:(NSString*)bizType
                 DstPath:(NSString*)dstPath
                    Time:(NSTimeInterval)timeStamp
                     MD5:(NSString*)md5
                Complete:(void(^)(NSString* dstPath, NSError* error))complete;

/// 删除超期没有访问的资源。如果未超期，不处理。返回是否删除了资源
/// - Parameters:
///   - dirPath: 资源目录绝对路径
///   - err: 错误
+ (BOOL)deleteDirIfExpired:(NSString*)dirPath ExpiredInterval:(NSTimeInterval)expiredInterval Error:(NSError**)err;

+ (NSString*)getMarsDir;

@end

NS_ASSUME_NONNULL_END

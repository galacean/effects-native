//
//  MultiMediaServiceAdapter.h
//  MarsNative
//
//  Created by changxing on 2023/2/8.
//  Copyright © 2023 Alipay. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface MarsDownloadUtil : NSObject

/// 异步下载文件
/// - Parameters:
///   - url: 资源地址
///   - bizType: 业务埋点
///   - complete: 回调
+ (void)downloadFile:(NSString*)url BizType:(NSString*)bizType Complete:(void(^)(NSData* _Nullable data, NSError* _Nullable error))complete;

/// 异步下载zip并解压
/// - Parameters:
///   - url: 资源地址
///   - bizType: 业务埋点
///   - md5: md5，如果传空则不进行校验
///   - tempDir: 保存目录地址，建议创建一个临时目录，下载成功后再进行拷贝
///   - complete: 回调
+ (void)downloadZip:(NSString*)url BizType:(NSString*)bizType MD5:(NSString*)md5 TempDir:(NSString*)tempDir Complete:(void(^)(NSString* dirPath, NSError* error))complete;

/// 获取多媒体缓存的资源路径
/// - Parameter url: 资源地址
+ (NSString*)getDownloadCachePath:(NSString*)url;

@end

NS_ASSUME_NONNULL_END

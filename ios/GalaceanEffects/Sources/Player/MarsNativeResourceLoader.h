#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface MarsNativeVideoResource : NSObject

@property (nonatomic, assign, nullable) void* videoContext;

- (instancetype)initWithFilePath:(NSString*)filePath Transparent:(BOOL)transparent HardDecoder:(BOOL)hardDecoder PlayerIdx:(int)playerIdx Complete:(void(^)(BOOL success, NSError* error))complete;

@end

@interface MarsNativeImageResource : NSObject

- (instancetype)initWithUrl:(NSString*)url;

@property (nonatomic, strong) NSString* key;

@property (nonatomic, strong) NSString* realUrl;

@property (nonatomic, assign) BOOL isVideo;

@property (nonatomic, assign) BOOL isTransparentVideo;

@property (nonatomic, assign) BOOL videoHardDecoder;

@property (nonatomic, strong) MarsNativeVideoResource* videoRes;

@property (nonatomic, strong) NSData* data;

@property (nonatomic, strong) UIImage* bitmap;

@end

@interface MarsNativeResourceLoader : NSObject

// 获取本地文件目录
+ (NSString*)loadFilePathFromLocal:(NSString*)url;

// 获取多媒体缓存路径
+ (NSString*)loadFilePathFromCache:(NSString*)url;

// 获取线上图片
+ (void)loadBitmap:(NSString*)url
           BizType:(NSString*)bizType
          Complete:(void(^)(UIImage* img, NSError* error))complete;

// 获取线上图片列表的内容
+ (void)loadImages:(NSArray<MarsNativeImageResource*>*)urlList
           DirPath:(NSString*)dirPath
         Variables:(NSDictionary* _Nullable)variables
   VariablesBitmap:(NSDictionary* _Nullable)variablesBitmap
         PlayerIdx:(int)playerIdx
           BizType:(NSString*)bizType
          Complete:(void(^)(NSError* error))complete;

+ (void)loadFonts:(NSArray<NSString*>*)urlList
          DirPath:(NSString*)dirPath
          BizType:(NSString*)bizType
         Complete:(void(^)(NSDictionary<NSString*, NSData*>* fontDataMap, NSError* error))complete;

// 获取线上zip解压后的路径
+ (void)loadZip:(NSString*)url
        BizType:(NSString*)bizType
 ValidTimeStamp:(NSTimeInterval)validTimeStamp
            MD5:(NSString*)md5
       Complete:(void(^)(NSString* dirPath, NSError* error))complete;

@end

NS_ASSUME_NONNULL_END

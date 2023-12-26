#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface MarsImageInfo : NSObject

@property (nonatomic, strong, nonnull) NSString* url;

@property (nonatomic, strong, nullable) NSString* astc;

@property (nonatomic, assign) int templateIdx; // 如果是数据模版，这里指向要替换的图片

@property (nonatomic, assign) BOOL isVideo;

@property (nonatomic, assign) BOOL isTransparentVideo;

@end

@interface MarsTextContent : NSObject

@property (nonatomic, assign) float x;
@property (nonatomic, assign) float y;
@property (nonatomic, strong) NSString* name;
@property (nonatomic, strong) NSString* text;
@property (nonatomic, assign) float maxWidth;
@property (nonatomic, assign) int fontIdx;
@property (nonatomic, assign) int align;
@property (nonatomic, assign) uint32_t color;
@property (nonatomic, assign) int overflow;
@property (nonatomic, strong) NSString* ellipsisText;

@end

@interface MarsTextInfo : NSObject

@property (nonatomic, assign) int imgIdx;
@property (nonatomic, assign) float x; // 暂时无用
@property (nonatomic, assign) float y; // 暂时无用
@property (nonatomic, assign) float width;
@property (nonatomic, assign) float height;
@property (nonatomic, strong) NSMutableArray<MarsTextContent*>* texts;

@end

@interface MarsFontInfo : NSObject

@property (nonatomic, strong) NSString* url;
@property (nonatomic, assign) int weight;
@property (nonatomic, assign) float size;
@property (nonatomic, assign) float letterSpace;
@property (nonatomic, strong) NSString* family;
@property (nonatomic, assign) int style;

@end

enum MarsDataType {
    JSON_BIN,
    UNKNOWN,
};

@interface MarsDataBase : NSObject

@property (nonatomic, strong) NSString* dirPath;

@property (nonatomic, strong) NSString* version; // 数据版本（for客户端）

@property (nonatomic, assign) CGSize previewSize; // 为了获取aspect

@property (nonatomic, assign) float duration; // 持续时间

@property (nonatomic, strong) NSArray<MarsImageInfo*>* images; // 图片列表

@property (nonatomic, strong) NSArray<MarsTextInfo*>* texts; // 文字列表
@property (nonatomic, strong) NSArray<MarsFontInfo*>* fonts; // 字体列表

@property (nonatomic, assign) bool valid; // 是否是合法的数据

+ (MarsDataBase*)createMarsData:(NSString*)dirPath;

- (MarsDataType)getType;

- (float)getAspect;

- (CGSize)getPreviewSize;

@end

NS_ASSUME_NONNULL_END

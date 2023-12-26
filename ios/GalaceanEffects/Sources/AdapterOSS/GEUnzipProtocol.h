#ifndef GEUnzipProtocol_h
#define GEUnzipProtocol_h

#import <Foundation/Foundation.h>

@protocol GEUnzipProtocol <NSObject>

-(BOOL) UnzipOpenFile:(NSString*) zipFile;
-(BOOL) UnzipFileTo:(NSString*) path overWrite:(BOOL) overwrite;
-(BOOL) UnzipCloseFile;

@end

@interface GEUnzipManager : NSObject

@property (nonatomic, strong) id<GEUnzipProtocol> unzipDelegate;

+(GEUnzipManager*) shared;

@end

#endif /* GEUnzipProtocol_h */

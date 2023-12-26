#import <Foundation/Foundation.h>
#import "MarsDataBase.h"

NS_ASSUME_NONNULL_BEGIN

@interface MarsDataJsonBin : MarsDataBase

- (instancetype)initWithJsonPath:(NSString*)jsonPath BinPath:(NSString*)binPath;

- (NSString*)getBinPath;

@end

NS_ASSUME_NONNULL_END

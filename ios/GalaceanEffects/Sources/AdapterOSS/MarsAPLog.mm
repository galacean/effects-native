#import "MarsAPLog.h"
#ifndef PLAYGROUND
// #import <APLog/APLog.h>
#endif
#include <sys/time.h>

static char s_time_buf_[80];
static char* format_time() {
    timeval te{};
    gettimeofday(&te, NULL);
    int32_t milliseconds = te.tv_usec / 1000;

    tm* gm_time = localtime(&te.tv_sec);
    strftime(s_time_buf_, sizeof(s_time_buf_), "%Y-%m-%d %H:%M:%S", gm_time);
    static size_t length = strlen(s_time_buf_);
    sprintf(s_time_buf_ + length, ".%03d", milliseconds);
    return s_time_buf_;
}

@implementation MarsAPLogger

- (void)debugLog:(NSString*)content {
    NSThread* thread = [NSThread currentThread];
    NSString* threadName = thread.name;
    if (!threadName.length) {
        threadName = [NSString stringWithFormat:@"T-%p", thread];
        [thread setName:threadName];
    }
    NSLog(@"%@ %s [MN::] %@", threadName, format_time(), content);
}

- (void)debug:(NSString*) tag Msg:(NSString*) msg {
    [self debugLog:[NSString stringWithFormat:@"INFO %@:%@", tag, msg]];
#ifndef PLAYGROUND
//    APLogInfo(@"[MN::]", @"%@", msg);
#endif
}

- (void)error:(NSString*) tag Msg:(NSString*) msg {
    [self debugLog:[NSString stringWithFormat:@"ERROR %@:%@", tag, msg]];
#ifndef PLAYGROUND
//    APLogInfo(@"[MN::]", @"%@", msg);
#endif
}

+ (void)Create {
    // todo: refactor，分离出adapter
    if ([MarsLogUtil getLogger] == nil) {
        [MarsLogUtil setLogger:[[MarsAPLogger alloc] init]];
    }
}

@end

void MNLogInfo(NSString* format, ...) {
    va_list args;
    va_start(args, format);
    NSString* logString = [[NSString alloc] initWithFormat:format arguments:args];
    va_end(args);

    NSThread* thread = [NSThread currentThread];
    NSString* threadName = thread.name;
    if (!threadName.length) {
        threadName = [NSString stringWithFormat:@"T-%p", thread];
        [thread setName:threadName];
    }
    NSLog(@"%@ %s [MN::PLM] %@", threadName, format_time(), logString);

#ifndef PLAYGROUND
//    APLogInfo(@"[MN::PLM]", @"%@", logString);
#endif
}

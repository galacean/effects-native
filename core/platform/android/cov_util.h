#ifndef MN_COV_UTIL_H_
#define MN_COV_UTIL_H_

#ifdef BUILD_PLAYGROUND
class CovUtil {
public:
    static void InitLLVMCov(const char* path);

    static int FlushLLVMCov();
};
#endif

#endif

#include "cov_util.h"

#ifdef BUILD_UNITTEST

#include <stdlib.h>
extern "C" {
    void __llvm_profile_set_filename(const char *Name);
    void __llvm_profile_initialize_file(void);
    int __llvm_profile_write_file(void);
}

void CovUtil::InitLLVMCov(const char* path) {
    __llvm_profile_set_filename(path);
    __llvm_profile_initialize_file();
}

int CovUtil::FlushLLVMCov() {
    return __llvm_profile_write_file();
}

#endif
#ifndef CROSS_PLATFORM_LIBS_H
#define CROSS_PLATFORM_LIBS_H

// CROSS-PLATFORM HEADERS

#include "conditions.h"

#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

// PLATFORM SPECIFIC HEADERS

#if win_defined
#include "../windows/libs.h"
#elif linux_defined
#include "../linux/libs.h"
#endif

// FUNCTIONS
void change_contrast(
        const vector<string> &input,
        const vector<string> &output,
        double factor = 1.0,
        int numThreads = 1
);

#endif //CROSS_PLATFORM_LIBS_H

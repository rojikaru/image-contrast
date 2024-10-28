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

#include "windows/libs.h"

#elif linux_defined
#include "linux/libs.h"
#endif

// FUNCTIONS
void change_contrast_many(
    const vector<string> &input,
    const vector<string> &output,
    double factor = 1.0,
    int numThreads = 1
);

extern "C" {
IMAGE_CONTRAST_LIB_API
void change_contrast(
    const char *input,
    const char *output,
    double factor = 1.0,
    int numThreads = 1
);

#ifdef win_defined
__declspec(dllexport)
void CALLBACK ChangeContrast(
        HWND hwnd,
        HINSTANCE hinst,
        LPSTR lpszCmdLine,
        int nCmdShow
);
#elif defined(linux_defined)
    // IMAGE_CONTRAST_LIB_API
    // void change_image_contrast(
    //     const char *input,
    //     const char *output,
    //     double factor = 1.0,
    //     int numThreads = 1
    // ) IMAGE_CONTRAST_ALIAS("change_contrast");
#endif
}

#endif //CROSS_PLATFORM_LIBS_H

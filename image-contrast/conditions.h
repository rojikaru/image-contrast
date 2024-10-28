#ifndef IMAGE_CONTRAST_CONDITIONS_H
#define IMAGE_CONTRAST_CONDITIONS_H

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
#define win_defined 1
#elif defined(__unix__) || defined(__unix)
#define linux_defined 1
#elif defined(__APPLE__) || defined(__MACH__)
#define apple_defined 1
#endif

#ifdef win_defined
    #ifdef IMAGE_CONTRAST_DLL_EXPORTS
    #define IMAGE_CONTRAST_LIB_API __declspec(dllexport)
    #elif defined(IMAGE_CONTRAST_DLL_IMPORTS)
    #define IMAGE_CONTRAST_LIB_API __declspec(dllimport)
    #else
    #define IMAGE_CONTRAST_LIB_API
    #endif
#else
    #if defined(IMAGE_CONTRAST_DLL_EXPORTS) || defined(IMAGE_CONTRAST_DLL_IMPORTS)
    #define IMAGE_CONTRAST_LIB_API __attribute__((visibility("default")))
    #else
    #define IMAGE_CONTRAST_LIB_API
    #endif
    #define IMAGE_CONTRAST_ALIAS(name) __attribute__((alias(name)))
#endif

#endif //IMAGE_CONTRAST_CONDITIONS_H

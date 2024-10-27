#ifndef CONDITIONS_H
#define CONDITIONS_H

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
#define win_defined 1
#elif defined(__unix__) || defined(__unix)
#define linux_defined 1
#elif defined(__APPLE__) || defined(__MACH__)
#define apple_defined 1
#endif

#ifdef win_defined
#define LIB_API __declspec(dllimport)
#elif linux_defined
#define LIB_API __attribute__((visibility("default")))
#endif

#endif //CONDITIONS_H

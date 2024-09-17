#ifndef CONDITIONS_H
#define CONDITIONS_H

#define win_defined defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
#define unix_defined defined(__unix__) || defined(__unix) || defined(__APPLE__) || defined(__MACH__)

#endif //CONDITIONS_H

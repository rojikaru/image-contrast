#ifndef LIBS_H
#define LIBS_H

#include "conditions.h"
#include <iostream>
#include <string>

#if win_defined
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

using namespace std;
using ImageContrastFunc = void (*)(const char *, const char *, double, int);

#endif //LIBS_H

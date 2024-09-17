#ifndef CROSS_PLATFORM_LIBS_H
#define CROSS_PLATFORM_LIBS_H

// CROSS-PLATFORM HEADERS

#include "conditions.h"

// PLATFORM SPECIFIC HEADERS

#if win_defined
#include "../windows/libs.h"
#elif unix_defined
#include "../unix/libs.h"
#endif

#endif //CROSS_PLATFORM_LIBS_H

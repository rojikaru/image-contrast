#ifndef CROSS_PLATFORM_LIBS_H
#define CROSS_PLATFORM_LIBS_H

// CROSS-PLATFORM HEADERS

#include "conditions.h"

// PLATFORM SPECIFIC HEADERS

#if win_defined
#include "../windows/libs.h"
#elif linux_defined
#include "../linux/libs.h"
#endif

// FUNCTIONS

std::string browse_folder(const std::string &saved_path = "");
void execute_fetch(const char *folder);
std::vector<std::string> catalog_folders(const std::string& catalog);

#endif //CROSS_PLATFORM_LIBS_H

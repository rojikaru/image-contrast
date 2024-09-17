#include "libs.h"

std::vector<std::string> catalog_folders(const std::string& catalog) {
    DIR *dir = opendir(catalog.c_str());
    struct dirent *entry = readdir(dir);
    std::vector<std::string> folders;

    while (entry != nullptr)
    {
        if (entry->d_type == DT_DIR)
        {
            std::string folderName = entry->d_name;
            if (folderName == "." || folderName == "..") continue;
            folders.push_back(catalog + "/" + folderName);
        }

        entry = readdir(dir);
    }

    closedir(dir);
}

void git_fetch_folder() {

}

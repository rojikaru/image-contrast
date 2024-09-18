#include "common/header.h"
using namespace std;

int main() {
    // Select folder
    std::string folder = browse_folder();
    if (folder.empty()) {
        std::cout << "Empty folder selected.\n";
        return -1;
    }
    std::cout << "Selected catalog: " << folder << std::endl;

    // Execute fetch for each subfolder
    auto folders = catalog_folders(folder);
    for (const auto &f : folders) {
        std::cout << "Folder: " << f << std::endl;
        execute_fetch(f.c_str());
    }

    return 0;
}

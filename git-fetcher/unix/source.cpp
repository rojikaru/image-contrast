#include "libs.h"

std::vector<std::string> catalog_folders(const std::string &catalog) {
    DIR *dir = opendir(catalog.c_str());
    struct dirent *entry = readdir(dir);
    std::vector<std::string> folders;

    while (entry != nullptr) {
        if (entry->d_type == DT_DIR) {
            std::string folderName = entry->d_name;
            if (folderName == "." || folderName == "..") continue;
            folders.push_back(catalog + "/" + folderName);
        }

        entry = readdir(dir);
    }

    closedir(dir);
    return folders;
}

// Function to execute a 'git fetch' in the specified folder
void execute_fetch(const char *folder) {
    pid_t pid = fork(); // Fork a child process

    if (pid < 0) {
        std::cerr << "Fork failed.\n";
        return;
    }

    // Child process
    if (pid == 0) {
        if (chdir(folder) != 0) {
            std::cerr << "Failed to change directory to: " << folder << "\n";
            exit(1);
        }

        char *const command[] = {strdup("git"), strdup("fetch"), strdup("origin"), nullptr};
        if (execvp(command[0], command) == -1) {
            std::cerr << "Execute failed.\n";
            exit(1);
        }

        return;
    }

    // Parent process

    // Suspending the child process
    kill(pid, SIGSTOP);

    char prompt;
    std::cout << "Do you wish to fetch this folder? (y/n) ";
    std::cin >> prompt;

    if (prompt != 'y') {
        // If 'n' is chosen, kill the child process
        kill(pid, SIGKILL);
        // Wait for child process to terminate
        waitpid(pid, nullptr, 0);
        std::cout << "Process terminated.\n";
    } else {
        // Resume the child process
        kill(pid, SIGCONT);
        // Wait for the child process to finish
        waitpid(pid, nullptr, 0);
    }
}

// Function to browse folders (using Linux terminal or a placeholder)
std::string browse_folder(const std::string &saved_path = "") {
    std::string folder;
    std::cout << "Enter folder path to fetch (default: " << saved_path << "): ";
    std::getline(std::cin, folder);
    if (folder.empty()) {
        return saved_path;
    }
    return folder;
}
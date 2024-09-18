#include "../common/conditions.h"
#if win_defined

#include "libs.h"

void execute_fetch(const char *folder) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    // Allocate memory full of zeros.
    ZeroMemory(&si, sizeof(si));
    // Set the size (count of bytes) of the structure.
    si.cb = sizeof(si);
    // Allocate memory full of zeros.
    ZeroMemory(&pi, sizeof(pi));

    // strdup kinda works as a non-const C-string constructor.
    char *command = strdup("git fetch origin");

    // Start the child process.
    if (!CreateProcess(nullptr,             // No module name (use command line)
                       command,             // Command line
                       nullptr,             // Process handle not inheritable
                       nullptr,             // Thread handle not inheritable
                       FALSE,               // Set handle inheritance to FALSE
                       CREATE_SUSPENDED,    // Creation flags
                       nullptr,             // Use parent's environment block
                       folder,              // Starting directory
                       &si,                 // Pointer to STARTUPINFO structure
                       &pi)                 // Pointer to PROCESS_INFORMATION structure
            ) {
        std::cout << "CreateProcess failed (" << GetLastError() << ").\n";
        return;
    }

    // Resume the child process.
    char prompt;
    std::cout << "Do you wish to fetch this folder? (y/n) ";
    std::cin >> prompt;

    if (prompt != 'y') {
        // Terminate the child process and close its handles
        TerminateProcess(pi.hProcess, 0);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        std::cout << "Process terminated.\n";

        return;
    }
    // else
    ResumeThread(pi.hThread);

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {

    if (uMsg == BFFM_INITIALIZED) {
//        std::string tmp = (const char *) lpData;
//        std::cout << "path: " << tmp << std::endl;
        SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
    }

    return 0;
}

std::string browse_folder(const std::string &saved_path = "") {
    TCHAR path[MAX_PATH];

    const char *path_param = saved_path.c_str();

    // the dialog that is used to browse for a folder
    BROWSEINFO bi = {nullptr};
    bi.lpszTitle = ("Browse for folder...");
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    bi.lpfn = BrowseCallbackProc;
    bi.lParam = (LPARAM) path_param;

    // show the dialog (pidl is the pointer to the item identifier list)
    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

    // if a folder was selected, convert it to a string and return it
    if (pidl != nullptr) {
        // get the name of the folder and put it in path
        SHGetPathFromIDList(pidl, path);

        // free memory used
        IMalloc *imalloc = nullptr;
        if (SUCCEEDED(SHGetMalloc(&imalloc))) {
            imalloc->Free(pidl);
            imalloc->Release();
        }

        return {path};
    }

    return "";
}

std::vector<std::string> catalog_folders(const std::string& catalog) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    // Prepare the search pattern (append "\*" to the directory path)
    std::string searchPath = catalog + "\\*";

    // Find the first file/folder in the directory
    hFind = FindFirstFile(searchPath.c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        std::cerr << "FindFirstFile failed (" << GetLastError() << ").\n";
        return {};
    }

    std::vector<std::string> folders;

    do {
        // Check if it's a directory
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // Skip "." and ".." directories
            std::string folderName = findFileData.cFileName;
            if (folderName == "." || folderName == "..") continue;
            folders.push_back(catalog + "\\" + folderName);
        }
    } while (FindNextFile(hFind, &findFileData) != 0); // Continue searching

    // Close the search handle
    FindClose(hFind);

    return folders;
}

#endif

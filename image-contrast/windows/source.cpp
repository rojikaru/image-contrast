#include "../conditions.h"

#if win_defined

#include "libs.h"
#include "../image_contrast.h"

struct ThreadArgs {
    Mat *img;
    int startY;
    int endY;
    double factor;
};

CRITICAL_SECTION logCs;

void initLog() {
    InitializeCriticalSection(&logCs);
}

void destroyLog() {
    DeleteCriticalSection(&logCs);
}

DWORD WINAPI adjust_contrast_thread(LPVOID lpParameter) {
    auto *args = static_cast<ThreadArgs *>(lpParameter);

    Mat *img = args->img;
    int startY = args->startY;
    int endY = args->endY;
    double factor = args->factor;
    int width = img->cols;

    for (int y = startY; y < endY; ++y) {
        for (int x = 0; x < width; ++x) {
            auto &color = img->at<Vec3b>(y, x);

            // Adjust RGB values based on the contrast factor
            color[2] = clamp(int((color[2] - 128) * factor) + 128, 0, 255); // R
            color[1] = clamp(int((color[1] - 128) * factor) + 128, 0, 255); // G
            color[0] = clamp(int((color[0] - 128) * factor) + 128, 0, 255); // B
        }
    }

    // Log the thread completion
    EnterCriticalSection(&logCs);
    cout << "Thread completed: " << GetCurrentThreadId() << endl;
    LeaveCriticalSection(&logCs);

    delete args;
    return 0;
}

void adjust_contrast_multi_threaded(Mat *img, double factor, int numThreads = 1) {
    if (numThreads < 1) {
        throw invalid_argument("numThreads must be greater than 0");
    }

    int height = img->rows;
    int rowsPerThread = height / numThreads;

    // Create threads
    vector<HANDLE> threads(numThreads);
    for (int i = 0; i < numThreads; ++i) {
        threads[i] = CreateThread(
                nullptr,                    // Default security attributes
                0,                          // Default stack size
                adjust_contrast_thread,     // Thread function
                new ThreadArgs{
                        img,
                        i * rowsPerThread,
                        (i == numThreads - 1) ? height : (i + 1) * rowsPerThread,
                        factor,
                },                          // Thread parameters
                0,                          // Default creation flags
                nullptr                     // Don't need the thread identifier
        );
    }

    // Wait for all threads to complete
    WaitForMultipleObjects(numThreads, threads.data(), TRUE, INFINITE);

    // Close thread handles
    for (HANDLE hThread: threads) {
        CloseHandle(hThread);
    }
}

// Load the image from a file
Mat read_img(const string &path) {
    HANDLE hFile = CreateFileA(path.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Failed to open input file");
    }

    DWORD fileSize = GetFileSize(hFile, nullptr);
    if (fileSize == INVALID_FILE_SIZE) {
        CloseHandle(hFile);
        throw std::runtime_error("Failed to get file size");
    }

    if (fileSize == 0) {
        CloseHandle(hFile);
        throw std::runtime_error("Input file is empty");
    }

    HANDLE hMapping = CreateFileMapping(hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
    if (!hMapping) {
        CloseHandle(hFile);
        throw std::runtime_error("Failed to create file mapping");
    }

    void *map = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
    if (!map) {
        CloseHandle(hMapping);
        CloseHandle(hFile);
        throw std::runtime_error("Failed to map input file");
    }

    Mat img = imdecode(Mat(1, fileSize, CV_8UC1, map), IMREAD_COLOR);

    // Clean up
    UnmapViewOfFile(map);
    CloseHandle(hMapping);
    CloseHandle(hFile);

    if (img.empty()) {
        throw std::runtime_error("Failed to load input image");
    }

    return img;
}

// Write the image to a file
void write_img(const string &path, const Mat &img) {
    vector<uchar> buf;

    // Encode the image to the buffer
    string ext = path.substr(path.find_last_of('.'));
    if (!imencode(ext == ".jpg" || ext == ".jpeg" ? ".jpeg" : ".png", img, buf)) {
        throw std::runtime_error("Failed to encode image");
    }

    HANDLE hFile = CreateFileA(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Failed to open output file");
    }

    HANDLE hMapping = CreateFileMapping(hFile, nullptr, PAGE_READWRITE, 0, buf.size(), nullptr);
    if (!hMapping) {
        CloseHandle(hFile);
        throw std::runtime_error("Failed to create file mapping");
    }

    void *map = MapViewOfFile(hMapping, FILE_MAP_WRITE, 0, 0, 0);
    if (!map) {
        CloseHandle(hMapping);
        CloseHandle(hFile);
        throw std::runtime_error("Failed to map output file");
    }

    // Copy the encoded image data to the mapped memory
    memcpy(map, buf.data(), buf.size());

    // Clean up
    UnmapViewOfFile(map);
    CloseHandle(hMapping);
    CloseHandle(hFile);
}

void inner_change_contrast(
    const string &input,
    const string &output,
    double factor,
    int numThreads = 1
) {
    if (factor <= 0 || factor > 2) {
        throw invalid_argument("factor must be in range (0, 2]");
    }

    Mat img = read_img(input);
    adjust_contrast_multi_threaded(&img, factor, numThreads);
    write_img(output, img);
}

void change_contrast_many(
    const vector<string> &input,
    const vector<string> &output,
    double factor,
    int numThreads
) {
    if (input.size() != output.size()) {
        throw invalid_argument("input and output size mismatch");
    }

    initLog();

    vector<HANDLE> threads(input.size());
    vector<tuple<string, string, double, int>> args(input.size());

    for (size_t i = 0; i < input.size(); ++i) {
        args[i] = make_tuple(input[i], output[i], factor, numThreads);
        threads[i] = CreateThread(
                nullptr,
                0,
                [](LPVOID param) -> DWORD {
                    auto args = *static_cast<tuple<string, string, double, int> *>(param);
                    inner_change_contrast(
                        get<0>(args),
                        get<1>(args),
                        get<2>(args),
                        get<3>(args)
                        );
                    return 0;
                },
                &args[i],
                0,
                nullptr
        );
    }

    // Wait for all threads to finish
    WaitForMultipleObjects(threads.size(), threads.data(), TRUE, INFINITE);

    // Close thread handles
    for (HANDLE hThread: threads) {
        CloseHandle(hThread);
    }

    destroyLog();
}

void change_contrast(
    const char *input,
    const char *output,
    double factor,
    int numThreads
) {
    change_contrast_many(vector{string(input)}, vector{string(output)}, factor, numThreads);
}

void CALLBACK ChangeContrast(
    HWND hwnd,
    HINSTANCE hinst,
    LPSTR lpszCmdLine,
    int nCmdShow
) {
    // Parse the command line arguments
    vector<string> args;
    string arg;
    istringstream iss(lpszCmdLine);
    while (iss >> arg) {
        args.push_back(arg);
    }

    if (args.size() < 4) {
        cerr << "Usage: ChangeContrast <input> <output> <factor> <numThreads>" << endl;
        return;
    }

    change_contrast_many(
        vector{args[0]},
        vector{args[1]},
        stod(args[2]),
        stoi(args[3])
    );
}

#endif

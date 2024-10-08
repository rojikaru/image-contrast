#include "../common/conditions.h"

#if win_defined

#include "libs.h"

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

void inner_change_contrast(
    const string &input,
    const string &output,
    double factor,
    int numThreads = 1
) {
    if (factor <= 0 || factor > 2) {
        throw invalid_argument("factor must be in range (0, 2]");
    }

    // Load the image from file using OpenCV
    Mat img = imread(input);
    if (img.empty()) {
        throw invalid_argument("Failed to load input image");
    }

    // Adjust contrast using multiple threads
    adjust_contrast_multi_threaded(&img, factor, numThreads);

    // Save the adjusted image
    imwrite(output, img);
}

void change_contrast(
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
    const string &input,
    const string &output,
    double factor,
    int numThreads = 1
) {
    change_contrast(vector{input}, vector{output}, factor, numThreads);
}

#endif

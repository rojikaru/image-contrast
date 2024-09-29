#include "../common/conditions.h"

#if win_defined

#include "libs.h"

struct ThreadArgs {
    Bitmap *bmp;
    int startY;
    int endY;
    double factor;
};

DWORD WINAPI adjust_contrast_thread(LPVOID lpParameter) {
    auto *args = static_cast<ThreadArgs *>(lpParameter);
    Bitmap *bmp = args->bmp;
    int startY = args->startY;
    int endY = args->endY;
    double factor = args->factor;

    UINT width = bmp->GetWidth();
    for (int y = startY; y < endY; ++y) {
        for (int x = 0; x < width; ++x) {
            Color color;
            bmp->GetPixel(x, y, &color);

            // Adjust RGB values based on the contrast factor
            int r = clamp(int((color.GetR() - 128) * factor) + 128, 0, 255);
            int g = clamp(int((color.GetG() - 128) * factor) + 128, 0, 255);
            int b = clamp(int((color.GetB() - 128) * factor) + 128, 0, 255);

            // Update the pixel color
            bmp->SetPixel(x, y, Color(color.GetA(), r, g, b));
        }
    }

    return 0;
}

void adjust_contrast_multi_threaded(Bitmap *bmp, double factor, int numThreads = 1) {
    if (numThreads < 1) {
        throw invalid_argument("numThreads");
    }

    int height = int(bmp->GetHeight());
    int rowsPerThread = height / numThreads;

    vector<HANDLE> threads(numThreads);
    vector<ThreadArgs> args(numThreads);

    for (int i = 0; i < numThreads; ++i) {
        args[i] = ThreadArgs{
                bmp,
                i * rowsPerThread,
                (i == numThreads - 1) ? height : (i + 1) * rowsPerThread,
                factor
        };
        threads[i] = CreateThread(
                nullptr,                    // Default security attributes
                0,                          // Default stack size
                adjust_contrast_thread,     // Thread function
                &args[i],                   // Thread parameters
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

void change_contrast(
        const wstring &input,
        const wstring &output,
        double factor,
        int numThreads = 1
) {
    if (factor <= 0 || factor > 2) {
        throw invalid_argument("factor");
    }

    // Load an image from file
    auto bmp = Bitmap::FromFile(input.c_str());
    if (bmp->GetLastStatus() != Ok) {
        throw invalid_argument("input");
    }

    // Adjust contrast
    adjust_contrast_multi_threaded(bmp, factor, numThreads);

    CLSID formatGuid;
    CLSIDFromString(L"{557cf401-1a04-11d3-9a73-0000f81ef32e}", &formatGuid);
    bmp->Save(
            output.c_str(),
            &formatGuid,
            nullptr
    );

    // Clean up
    delete bmp;
}

void change_contrast(
        const vector<wstring> &input,
        const vector<wstring> &output,
        double factor,
        int numThreads
) {
    if (input.size() != output.size()) {
        throw invalid_argument("input/output");
    }

    // Initialize GDI+
    GdiplusStartupInput gdiPlusStartupInput;
    ULONG_PTR gdiPlusToken;
    GdiplusStartup(&gdiPlusToken, &gdiPlusStartupInput, nullptr);

    // Create processing threads
    vector<HANDLE> threads(input.size());
    vector<tuple<wstring, wstring, double, int>> args(input.size());
    vector<int> processPriority{THREAD_PRIORITY_ABOVE_NORMAL, THREAD_PRIORITY_NORMAL, THREAD_PRIORITY_BELOW_NORMAL};
    for (size_t i = 0; i < input.size(); ++i) {
        args[i] = make_tuple(input[i], output[i], factor, numThreads);
        threads[i] = CreateThread(
                nullptr,
                0,
                [](LPVOID param) -> DWORD {
                    auto args = *static_cast<tuple<wstring, wstring, double, int> *>(param);
                    change_contrast(get<0>(args), get<1>(args), get<2>(args), get<3>(args));
                    return 0;
                },
                &args[i],
                0,
                nullptr
        );
        // Change the priority of the thread
        SetThreadPriority(threads[i], processPriority[i % 3]);
    }

    // Wait for all threads to finish
    WaitForMultipleObjects(threads.size(), threads.data(), TRUE, INFINITE);

    // Close thread handles
    for (auto &thread: threads) {
        CloseHandle(thread);
    }

    GdiplusShutdown(gdiPlusToken);
}

#endif

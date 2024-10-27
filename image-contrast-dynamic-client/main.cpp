#include "common/header.h"

int main() {
    const char *fn_name = "change_image_contrast";
    ImageContrastFunc change_contrast;

#if win_defined
    HMODULE hModule = LoadLibrary("image_contrast.dll");
    if (hModule == nullptr) {
        cerr << "Failed to load the common library" << endl;
        return 1;
    }

    change_contrast = (ImageContrastFunc) GetProcAddress(hModule, fn_name);
    if (change_contrast == nullptr) {
        cerr << "Failed to load the function" << endl;
        return 1;
    }

#elif linux_defined
    void *handle = dlopen("libimage_contrast.so", RTLD_LAZY);
    if (handle == nullptr) {
        cerr << "Failed to load the common library" << endl;
        return 1;
    }

    // Clear any existing error
    dlerror();

    change_contrast = (ImageContrastFunc) dlsym(handle, fn_name);
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
        cerr << "Failed to load the function: " << dlsym_error << endl;
        return 1;
    }
#endif

    int numThreads;
    double factor;
    string input, output;

    cout << "Enter the number of threads: ";
    cin >> numThreads;
    cout << "Enter the contrast factor: ";
    cin >> factor;
    cout << "Enter the input image path: ";
    cin >> input;
    cout << "Enter the output image path: ";
    cin >> output;

    change_contrast(input.c_str(), output.c_str(), factor, numThreads);

    return 0;
}

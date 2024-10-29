#include "../conditions.h"

#if linux_defined

#include "libs.h"
#include "../image_contrast.h"

struct ThreadArgs {
    Mat *img;
    int startY;
    int endY;
    double factor;
};

pthread_mutex_t logMutex;

void destroyMutex(pthread_mutex_t *mutex) {
    pthread_mutex_destroy(mutex);
}

void *adjust_contrast_thread(void *lpParameter) {
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

    // Add logs after exit
    pthread_mutex_lock(&logMutex);
    cout << "Thread " << pthread_self() << " contrast thread finished successfully" << endl;
    pthread_mutex_unlock(&logMutex);

    pthread_exit(nullptr);
}

void adjust_contrast_multi_threaded(Mat *img, double factor, int numThreads = 1) {
    if (numThreads < 1) {
        throw invalid_argument("numThreads must be greater than 0");
    }

    int height = img->rows;
    int rowsPerThread = height / numThreads;

    vector<pthread_t> threads(numThreads);
    vector<ThreadArgs> args(numThreads);

    for (int i = 0; i < numThreads; ++i) {
        args[i] = ThreadArgs{
            img,
            i * rowsPerThread,
            (i == numThreads - 1) ? height : (i + 1) * rowsPerThread,
            factor
        };
        pthread_create(&threads[i], nullptr, adjust_contrast_thread, &args[i]);
    }

    // Wait for all threads to complete
    for (auto &thread: threads) {
        pthread_join(thread, nullptr);
    }
}

// Load the image from memory-mapped file
Mat read_img(const string &path) {
    const int fd = open(path.c_str(), O_RDONLY);
    if (fd == -1)
        throw std::runtime_error("Failed to open input file");

    struct stat sb{};
    if (fstat(fd, &sb) == -1) {
        close(fd);
        throw std::runtime_error("Failed to get file size");
    }

    if (sb.st_size == 0) {
        close(fd);
        throw std::runtime_error("Input file is empty");
    }

    void *map = mmap(nullptr, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) {
        close(fd);
        throw std::runtime_error("Failed to map input file");
    }

    Mat img = imdecode(
        Mat(1, sb.st_size, CV_8UC1, map),
        IMREAD_COLOR
    );

    // Clean up
    munmap(map, sb.st_size);
    close(fd);

    if (img.empty())
        throw std::runtime_error("Failed to load input image");

    return img;
}

// Unmap the memory-mapped file and write the image
void write_img(const string &path, const Mat &img) {
    vector<uchar> buf;

    // Encode the image to the buffer
    if (
        const string ext = path.substr(path.find_last_of('.'));
        !imencode(ext == ".jpg" || ext == ".jpeg" ? ".jpeg" : ".png", img, buf)
    ) {
        throw std::runtime_error("Failed to encode image");
    }

    const int fd = open(path.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1)
        throw std::runtime_error("Failed to open output file");

    // Set the size of the file to the size of the encoded image
    if (ftruncate(fd, buf.size()) == -1) {
        close(fd);
        throw std::runtime_error("Failed to set file size");
    }

    void *map = mmap(nullptr, buf.size(), PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        close(fd);
        throw std::runtime_error("Failed to map output file");
    }

    // Copy the encoded image data to the mapped memory
    memcpy(map, buf.data(), buf.size());

    // Clean up
    munmap(map, buf.size());
    close(fd);
}

void inner_change_contrast(
    const string &input,
    const string &output,
    double factor,
    int numThreads = 1
) {
    if (factor <= 0 || factor > 2)
        throw invalid_argument("factor must be in range (0, 2]");
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

    logMutex = PTHREAD_MUTEX_INITIALIZER;

    vector<pthread_t> threads(input.size());
    vector<tuple<string, string, double, int> > args(input.size());

    for (size_t i = 0; i < input.size(); ++i) {
        args[i] = make_tuple(input[i], output[i], factor, numThreads);
        pthread_create(&threads[i], nullptr, [](void *param) -> void *{
            auto t_args = *static_cast<tuple<string, string, double, int> *>(param);
            inner_change_contrast(
                get<0>(t_args),
                get<1>(t_args),
                get<2>(t_args),
                get<3>(t_args)
            );
            pthread_exit(nullptr);
        }, &args[i]);
    }

    // Wait for all threads to finish
    for (auto &thread: threads) {
        pthread_join(thread, nullptr);
    }

    destroyMutex(&logMutex);
}

extern "C" {
void change_contrast(
    const char *input,
    const char *output,
    double factor,
    int numThreads
) {
    change_contrast_many(vector{string(input)}, vector{string(output)}, factor, numThreads);
}

void change_image_contrast(
    const char *input,
    const char *output,
    double factor,
    int numThreads
)
IMAGE_CONTRAST_LIB_API
IMAGE_CONTRAST_ALIAS("change_contrast");
}

#endif

#include "common/header.h"

int main() {
    fs::remove_all("../outputs");
    fs::create_directory("../outputs");

    constexpr int numThreads = 90;
    constexpr double factor = .7;

    // change_contrast(
    //     "../inputs/input3.jpg",
    //     "../outputs/output3.jpg",
    //     factor,
    //     numThreads
    // );

    change_contrast(
            {
                    "../inputs/input1.jpg",
                    "../inputs/input2.jpg",
                    "../inputs/input3.jpg"
            },
            {
                    "../outputs/output1.jpg",
                    "../outputs/output2.jpg",
                    "../outputs/output3.jpg"
            },
            factor,
            numThreads
    );

    return 0;
}

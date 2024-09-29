#include "common/header.h"


int main() {
    fs::remove_all("../outputs");
    fs::create_directory("../outputs");

    constexpr int numThreads = 1;
    constexpr double factor = 1.7;

    change_contrast(
            {
                    L"../inputs/input1.jpg",
                    L"../inputs/input2.jpg",
                    L"../inputs/input3.jpg"
            },
            {
                    L"../outputs/output1.jpg",
                    L"../outputs/output2.jpg",
                    L"../outputs/output3.jpg"
            },
            factor,
            numThreads
    );

    return 0;
}

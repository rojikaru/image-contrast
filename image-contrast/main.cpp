#include "common/header.h"

int main() {
    fs::remove_all("../outputs");
    fs::create_directory("../outputs");

    int numThreads;
    double factor;

    cout << "Enter the number of threads: ";
    cin >> numThreads;
    cout << "Enter the contrast factor: ";
    cin >> factor;

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

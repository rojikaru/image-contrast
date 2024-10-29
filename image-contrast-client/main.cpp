#include "common/header.h"
using namespace std;

int main()
{
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

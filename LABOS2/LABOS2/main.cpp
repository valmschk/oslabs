#define NOMINMAX
#include <windows.h>
#include <iostream>
#include "data.h"
#include "minmax.h"
#include "average.h"

HANDLE hConsoleMutex;

int main() {
    hConsoleMutex = CreateMutex(NULL, FALSE, NULL);
    if (hConsoleMutex == NULL) return 1;

    int n;
    std::cout << "Enter array size: ";
    if (!(std::cin >> n) || n <= 0) return 1;

    Data data(n);

    std::cout << "Enter array elements: ";
    for (int i = 0; i < n; ++i) {
        int val;
        std::cin >> val;
        data.push(val, i); 
    }

    HANDLE hMinMax = CreateThread(NULL, 0, min_max_thread, &data, 0, NULL);
    HANDLE hAverage = CreateThread(NULL, 0, average_thread, &data, 0, NULL);

    if (hMinMax != NULL && hAverage != NULL) {
        WaitForSingleObject(hMinMax, INFINITE);
        WaitForSingleObject(hAverage, INFINITE);

        for (int i = 0; i < n; ++i) {
            if (data.arr[i] == data.min || data.arr[i] == data.max) {
                data.arr[i] = static_cast<int>(data.average);
            }
        }

        std::cout << "Modified array: ";
        for (int i = 0; i < n; ++i) {
            std::cout << data.arr[i] << " ";
        }
        std::cout << std::endl;
    }

    if (hMinMax) CloseHandle(hMinMax);
    if (hAverage) CloseHandle(hAverage);
    if (hConsoleMutex) CloseHandle(hConsoleMutex);

    return 0;
}
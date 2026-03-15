#include <windows.h>
#include <iostream>
#include "data.h"
#include "minmax.h"
#include "average.h"

HANDLE hConsoleMutex;

int main() {
    hConsoleMutex = CreateMutex(NULL, FALSE, NULL);

    int n;
    std::cout << "Enter size: ";
    std::cin >> n;

    Data data(n);
    std::cout << "Enter elements: ";
    for (int i = 0; i < n; ++i) {
        int v;
        std::cin >> v;
        data.push(v, i);
    }

    HANDLE h1 = CreateThread(NULL, 0, min_max_thread, &data, 0, NULL);
    HANDLE h2 = CreateThread(NULL, 0, average_thread, &data, 0, NULL);

    WaitForSingleObject(h1, INFINITE);
    WaitForSingleObject(h2, INFINITE);

    for (int i = 0; i < n; ++i) {
        if (data.arr[i] == data.min || data.arr[i] == data.max) {
            data.arr[i] = static_cast<int>(data.average);
        }
    }

    std::cout << "Result: ";
    for (int i = 0; i < n; ++i) {
        std::cout << data.arr[i] << " ";
    }
    std::cout << std::endl;

    CloseHandle(h1);
    CloseHandle(h2);
    CloseHandle(hConsoleMutex);

    return 0;
}
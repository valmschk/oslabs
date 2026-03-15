#include "minmax.h"
#include "data.h"
#include <iostream>

extern HANDLE hConsoleMutex;

DWORD WINAPI min_max_thread(LPVOID param) {
    Data* data = static_cast<Data*>(param);
    if (!data || data->size <= 0) return 1;

    int cur_min = data->arr[0];
    int cur_max = data->arr[0];

    for (int i = 0; i < data->size; i++) {
        if (data->arr[i] < cur_min) cur_min = data->arr[i];
        Sleep(7); 
        if (data->arr[i] > cur_max) cur_max = data->arr[i];
        Sleep(7);
    }

    data->min = cur_min;
    data->max = cur_max;

    WaitForSingleObject(hConsoleMutex, INFINITE);
    std::cout << "[min_max] Min: " << data->min << ", Max: " << data->max << std::endl;
    ReleaseMutex(hConsoleMutex);

    return 0;
}
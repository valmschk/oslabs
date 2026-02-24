#define NOMINMAX
#include <windows.h>
#include "data.h"
#include <iostream>

DWORD WINAPI min_max_thread(LPVOID param) {
    Data* data = static_cast<Data*>(param);
    if (!data || data->size <= 0) return 1;

    int current_min = data->arr[0];
    int current_max = data->arr[0];

    for (int i = 0; i < data->size; i++) {
        if (data->arr[i] < current_min) current_min = data->arr[i];
        Sleep(7); 
        if (data->arr[i] > current_max) current_max = data->arr[i];
        Sleep(7); 
    }

    data->min = current_min;
    data->max = current_max;

    std::cout << "[min_max] Min: " << data->min << ", Max: " << data->max << std::endl;
    return 0;
}
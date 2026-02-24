#define NOMINMAX
#include <windows.h>
#include "data.h"
#include <iostream>

DWORD WINAPI average_thread(LPVOID param) {
    Data* data = static_cast<Data*>(param);
    if (!data || data->size <= 0) return 1;

    double sum = 0;
    for (int i = 0; i < data->size; i++) {
        sum += data->arr[i];
        Sleep(12); 
    }

    data->average = sum / data->size;
    std::cout << "[average] Avg: " << data->average << std::endl;
    return 0;
}
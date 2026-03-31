#include "Array.h"
#include <iostream>

Array::Array(int n) : data(n, 0) {
    InitializeCriticalSection(&cs);
}

Array::~Array() {
    DeleteCriticalSection(&cs);
}

int Array::getSize() const { return static_cast<int>(data.size()); }

int Array::get(int index) {
    EnterCriticalSection(&cs);
    int val = data[index];
    LeaveCriticalSection(&cs);
    return val;
}

void Array::set(int index, int value) {
    EnterCriticalSection(&cs);
    data[index] = value;
    LeaveCriticalSection(&cs);
}

void Array::clearValue(int value) {
    EnterCriticalSection(&cs);
    for (size_t i = 0; i < data.size(); ++i) {
        if (data[i] == value) data[i] = 0;
    }
    LeaveCriticalSection(&cs);
}

void Array::print() {
    EnterCriticalSection(&cs);
    std::cout << "\nArray content: ";
    for (int x : data) std::cout << x << " ";
    std::cout << std::endl;
    LeaveCriticalSection(&cs);
}
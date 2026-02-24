#include "data.h"

Data::Data(int n) : size(n), min(0), max(0), average(0.0) {
    arr = new int[n];
}

Data::~Data() {
    delete[] arr;
}

void Data::push(int value, int index) {
    if (index >= 0 && index < size) {
        arr[index] = value;
    }
}
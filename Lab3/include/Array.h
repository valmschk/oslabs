#ifndef ARRAY_H
#define ARRAY_H

#include <windows.h>
#include <vector>

class Array {
private:
    std::vector<int> data;
    CRITICAL_SECTION cs;

public:
    Array(int n);
    ~Array();

    int getSize() const;
    int get(int index);
    void set(int index, int value);
    void clearValue(int value);
    void print();
};

#endif
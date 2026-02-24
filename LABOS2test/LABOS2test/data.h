#ifndef DATA_H
#define DATA_H

struct Data {
    int* arr;
    int size;
    int min;
    int max;
    double average;

    Data(int n);
    ~Data();
    void push(int value, int index);

    Data(const Data&) = delete;
    Data& operator=(const Data&) = delete;
};

#endif

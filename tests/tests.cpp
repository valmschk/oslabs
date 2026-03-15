#include <iostream>
#include <cassert>
#include "data.h"

int main() {
    std::cout << "Running Tests..." << std::endl;
    Data d(2);
    d.push(10, 0);
    d.push(20, 1);

    assert(d.arr[0] == 10);
    assert(d.size == 2);

    std::cout << "Tests Passed!" << std::endl;
    return 0;
}
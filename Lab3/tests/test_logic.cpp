#include <iostream>
#include <cassert>
#include "Array.h"

int main() {
    std::cout << "Starting Unit Tests for Array class..." << std::endl;

    Array arr(10);
    if (arr.getSize() == 10) {
        std::cout << "[PASS] Test 1: Array size is correct." << std::endl;
    }
    else {
        std::cout << "[FAIL] Test 1: Array size is wrong!" << std::endl;
        return 1;
    }

    arr.set(5, 77);
    if (arr.get(5) == 77) {
        std::cout << "[PASS] Test 2: Set and Get operations work." << std::endl;
    }
    else {
        std::cout << "[FAIL] Test 2: Set or Get failed!" << std::endl;
        return 1;
    }

    arr.set(0, 77);
    arr.set(9, 77);
    arr.clearValue(77);

    if (arr.get(0) == 0 && arr.get(5) == 0 && arr.get(9) == 0) {
        std::cout << "[PASS] Test 3: clearValue successfully reset all marks." << std::endl;
    }
    else {
        std::cout << "[FAIL] Test 3: clearValue did not clean the array!" << std::endl;
        return 1;
    }

    std::cout << "\n " << std::endl;
    std::cout << "ALL LOGIC TESTS PASSED!" << std::endl;
    std::cout << " " << std::endl;

    return 0;
}
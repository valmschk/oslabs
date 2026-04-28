#include <iostream>
#include <cassert>
#include <vector>
#include <fstream>
#include <cstring>
#include "employee.h"

void test_struct_integrity() {
    employee emp = { 1, "Dev", 40.0 };
    assert(emp.num == 1);
    assert(std::strcmp(emp.name, "Dev") == 0);
    assert(sizeof(employee) >= 20);
}

void test_search_logic() {
    std::vector<employee> db = { {10, "A", 1}, {20, "B", 2} };
    auto find = [&](int id) {
        for (int i = 0; i < db.size(); ++i) if (db[i].num == id) return i;
        return -1;
        };
    assert(find(20) == 1);
    assert(find(99) == -1);
}

void test_file_persistence() {
    const char* f = "t.bin";
    employee out = { 7, "Test", 1.5 };
    std::ofstream o(f, std::ios::binary);
    o.write(reinterpret_cast<char*>(&out), sizeof(employee));
    o.close();
    employee in;
    std::ifstream i(f, std::ios::binary);
    i.read(reinterpret_cast<char*>(&in), sizeof(employee));
    i.close();
    assert(in.num == 7);
    std::remove(f);
}

int main() {
    test_struct_integrity();
    test_search_logic();
    test_file_persistence();
    std::cout << "All tests passed." << std::endl;
    return 0;
}
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include "em.h"
using namespace std;
int main(int argc, char* argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");

    if (argc < 3) return 1;

    string filename = argv[1];
    int count = stoi(argv[2]);

    ofstream out(filename, ios::binary);
    if (!out) return 1;

    employee emp;
    for (int i = 0; i < count; i++) {
        cout << "Сотрудник №" << i + 1 << " (ID Имя Часы): ";
        if (!(cin >> emp.num >> emp.name >> emp.hours)) break;
        out.write((char*)&emp, sizeof(employee));
    }
    out.close();
    return 0;
}
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <windows.h>
#include "em.h"
using namespace std;
int main(int argc, char* argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");

    if (argc < 4) return 1;

    string binFile = argv[1];
    string txtFile = argv[2];
    double rate = stod(argv[3]);

    ifstream in(binFile, ios::binary);
    ofstream out(txtFile);

    if (!in || !out) return 1;

    out << "Отчет по файлу: " << binFile << endl;
    out << "Номер, Имя, Часы, Зарплата" << endl;

    employee emp;
    while (in.read((char*)&emp, sizeof(employee))) {
        double salary = emp.hours * rate;
        out << emp.num << " " << emp.name << " " << emp.hours << " " << fixed << setprecision(2) << salary << endl;
    }

    in.close();
    out.close();
    return 0;
}
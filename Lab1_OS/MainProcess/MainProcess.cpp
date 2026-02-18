#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <vector>
#include "em.h"
using namespace std;
void runProcess(wstring cmdLine) {
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    vector<wchar_t> cmdBuf(cmdLine.begin(), cmdLine.end());
    cmdBuf.push_back(0);

    if (CreateProcess(NULL, cmdBuf.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE); 
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else {
        cout << "Ошибка запуска процесса! Убедитесь, что .exe файлы собраны." << endl;
    }
}
int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");

    string binName;
    int n;
    cout << "Введите имя бинарного файла: "; cin >> binName;
    cout << "Введите количество сотрудников: "; cin >> n;

    wstring c_args = L"Creator.exe " + wstring(binName.begin(), binName.end()) + L" " + to_wstring(n);
    runProcess(c_args);

    ifstream fin(binName, ios::binary);
    employee emp;
    cout << "\n--- Данные из бинарного файла: ---" << endl;
    while (fin.read((char*)&emp, sizeof(employee))) {
        cout << "ID: " << emp.num << ", Имя: " << emp.name << ", Часы: " << emp.hours << endl;
    }
    fin.close();

    string repName;
    double rate;
    cout << "\nВведите имя файла отчета: "; cin >> repName;
    cout << "Введите оплату за час: "; cin >> rate;

    wstring r_args = L"Reporter.exe " + wstring(binName.begin(), binName.end()) +
        L" " + wstring(repName.begin(), repName.end()) + L" " + to_wstring(rate);
    runProcess(r_args);

    cout << "\n--- ИТОГОВЫЙ ОТЧЕТ: ---" << endl;
    ifstream frep(repName);
    string line;
    while (getline(frep, line)) {
        cout << line << endl;
    }
    frep.close();

    system("pause");
    return 0;
}
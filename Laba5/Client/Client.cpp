#include <iostream>
#include <string>
#include <windows.h>
#include "employee.h"

int main() {
    HANDLE hPipe;
    while (true) {
        hPipe = CreateFileA("\\\\.\\pipe\\lab_pipe", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (hPipe != INVALID_HANDLE_VALUE) break;
        if (GetLastError() != ERROR_PIPE_BUSY || !WaitNamedPipeA("\\\\.\\pipe\\lab_pipe", 10000)) {
            std::cout << "Could not connect to pipe." << std::endl;
            return 1;
        }
    }

    std::cout << "Connected to server." << std::endl;

    while (true) {
        std::cout << "\n1. Read Record\n2. Modify Record\n3. Exit\nChoice: ";
        int choice;
        std::cin >> choice;

        if (choice == 3) {
            ClientRequest req = { CommandType::QUIT, 0 };
            DWORD bw;
            WriteFile(hPipe, &req, sizeof(req), &bw, NULL);
            break;
        }

        int id;
        std::cout << "Enter Employee ID: ";
        std::cin >> id;

        ClientRequest req = { (choice == 1 ? CommandType::READ : CommandType::MODIFY), id };
        DWORD bw, br;
        WriteFile(hPipe, &req, sizeof(req), &bw, NULL);

        employee emp;
        ReadFile(hPipe, &emp, sizeof(emp), &br, NULL);

        if (emp.num == -1) {
            std::cout << "Employee not found!" << std::endl;
            continue;
        }

        std::cout << "Received: ID=" << emp.num << ", Name=" << emp.name << ", Hours=" << emp.hours << std::endl;

        if (req.cmd == CommandType::MODIFY) {
            std::cout << "Enter new Name and Hours: ";
            std::cin >> emp.name >> emp.hours;
            std::cout << "Press Enter to send updates to server...";
            std::cin.ignore(); std::cin.get();
            WriteFile(hPipe, &emp, sizeof(emp), &bw, NULL);
        }

        std::cout << "Press Enter to finish and unlock record...";
        if (req.cmd != CommandType::MODIFY) std::cin.ignore();
        std::cin.get();

        CommandType done = CommandType::DONE;
        WriteFile(hPipe, &done, sizeof(done), &bw, NULL);
    }

    CloseHandle(hPipe);
    return 0;
}
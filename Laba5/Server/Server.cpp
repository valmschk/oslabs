#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <shared_mutex>
#include <mutex>
#include <memory>
#include <windows.h>
#include "employee.h"

std::mutex fileMutex;
std::vector<employee> memoryTracker;
std::vector<std::unique_ptr<std::shared_mutex>> recordLocks;
std::string globalFileName;

void PrintFileContent() {
    std::ifstream fin(globalFileName, std::ios::binary);
    employee e;
    std::cout << "\n Current File Content " << std::endl;
    while (fin.read(reinterpret_cast<char*>(&e), sizeof(employee))) {
        std::cout << "ID: " << e.num << " | Name: " << e.name << " | Hours: " << e.hours << std::endl;
    }
    std::cout << "\n" << std::endl;
}

int GetEmployeeIndex(int id) {
    for (size_t i = 0; i < memoryTracker.size(); ++i) {
        if (memoryTracker[i].num == id) return static_cast<int>(i);
    }
    return -1;
}

void ClientThread(HANDLE hPipe) {
    while (true) {
        ClientRequest req;
        DWORD bytesRead;
        if (!ReadFile(hPipe, &req, sizeof(req), &bytesRead, NULL) || bytesRead == 0) break;
        if (req.cmd == CommandType::QUIT) break;

        int idx = GetEmployeeIndex(req.id);
        employee emp;
        DWORD bw;

        if (idx == -1) {
            emp.num = -1;
            WriteFile(hPipe, &emp, sizeof(emp), &bw, NULL);
            continue;
        }

        if (req.cmd == CommandType::READ) {
            recordLocks[idx]->lock_shared(); 
            {
                std::lock_guard<std::mutex> lock(fileMutex);
                std::ifstream fin(globalFileName, std::ios::binary);
                fin.seekg(idx * sizeof(employee));
                fin.read(reinterpret_cast<char*>(&emp), sizeof(employee));
            }
            WriteFile(hPipe, &emp, sizeof(emp), &bw, NULL);

            CommandType done;
            ReadFile(hPipe, &done, sizeof(done), &bytesRead, NULL);
            recordLocks[idx]->unlock_shared();
        }
        else if (req.cmd == CommandType::MODIFY) {
            recordLocks[idx]->lock(); 
            {
                std::lock_guard<std::mutex> lock(fileMutex);
                std::ifstream fin(globalFileName, std::ios::binary);
                fin.seekg(idx * sizeof(employee));
                fin.read(reinterpret_cast<char*>(&emp), sizeof(employee));
            }
            WriteFile(hPipe, &emp, sizeof(emp), &bw, NULL);

            employee newEmp;
            ReadFile(hPipe, &newEmp, sizeof(newEmp), &bytesRead, NULL);
            {
                std::lock_guard<std::mutex> lock(fileMutex);
                std::fstream fout(globalFileName, std::ios::binary | std::ios::in | std::ios::out);
                fout.seekp(idx * sizeof(employee));
                fout.write(reinterpret_cast<char*>(&newEmp), sizeof(employee));
            }

            CommandType done;
            ReadFile(hPipe, &done, sizeof(done), &bytesRead, NULL);
            recordLocks[idx]->unlock();
        }
    }
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
}

int main() {
    std::cout << "Enter binary file name: ";
    std::cin >> globalFileName;

    int n;
    std::cout << "Enter number of employees: ";
    std::cin >> n;

    std::ofstream fout(globalFileName, std::ios::binary);
    for (int i = 0; i < n; ++i) {
        employee e;
        std::cout << "Employee " << i + 1 << " (ID Name Hours): ";
        std::cin >> e.num >> e.name >> e.hours;
        fout.write(reinterpret_cast<char*>(&e), sizeof(employee));
        memoryTracker.push_back(e);
        recordLocks.push_back(std::make_unique<std::shared_mutex>());
    }
    fout.close();

    PrintFileContent();

    int clientCount;
    std::cout << "Enter number of clients: ";
    std::cin >> clientCount;

    for (int i = 0; i < clientCount; ++i) {
        STARTUPINFOA si = { sizeof(si) };
        PROCESS_INFORMATION pi;
        if (!CreateProcessA(NULL, (LPSTR)".\\Client.exe", NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
            std::cout << "Error launching Client.exe. Make sure it is in the same folder." << std::endl;
        }
        else {
            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);
        }
    }

    std::vector<std::thread> threads;
    for (int i = 0; i < clientCount; ++i) {
        HANDLE hPipe = CreateNamedPipeA("\\\\.\\pipe\\lab_pipe", PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES, sizeof(employee), sizeof(ClientRequest), 0, NULL);

        if (ConnectNamedPipe(hPipe, NULL) || GetLastError() == ERROR_PIPE_CONNECTED) {
            threads.emplace_back(ClientThread, hPipe);
        }
    }

    for (auto& t : threads) t.join();

    std::cout << "\nAll clients finished." << std::endl;
    PrintFileContent();

    std::cout << "Type 'exit' to close server: ";
    std::string cmd;
    while (std::cin >> cmd && cmd != "exit");

    return 0;
}
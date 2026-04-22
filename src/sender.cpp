#include "common.h"
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

int main(int argc, char* argv[])
{
    try
    {
        if (argc < 4) throw std::runtime_error("Not enough arguments. Usage: Sender <id> <capacity> <filename>");

        int senderId = std::stoi(argv[1]);
        int capacity = std::stoi(argv[2]);
        std::string filename = argv[3];

        HANDLE hMutex = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, "MutexAccess");
        if (!hMutex) throw std::runtime_error("Failed to open mutex");

        HANDLE hEmpty = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, "EmptySlots");
        if (!hEmpty) throw std::runtime_error("Failed to open empty semaphore");

        HANDLE hFull = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, "FullSlots");
        if (!hFull) throw std::runtime_error("Failed to open full semaphore");

        std::string evName = "SenderReady" + std::to_string(senderId);
        HANDLE hReady = OpenEventA(EVENT_ALL_ACCESS, FALSE, evName.c_str());
        if (!hReady) throw std::runtime_error("Failed to open sender ready event");
        SetEvent(hReady);

        HANDLE hMap = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, "GlobalRingBuffer");
        if (!hMap) throw std::runtime_error("Failed to open shared memory");

        RingBuffer* buffer = static_cast<RingBuffer*>(MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(RingBuffer)));
        if (!buffer) throw std::runtime_error("Failed to map shared memory");

        std::cout << "Sender #" << senderId << " started." << std::endl;

        while (true)
        {
            std::cout << "\nEnter command (send/exit): ";
            std::string cmd;
            std::cin >> cmd;

            if (cmd == "exit") break;
            if (cmd == "send")
            {
                std::string msg;
                std::cout << "Enter message: ";
                std::cin.ignore();
                std::getline(std::cin, msg);

                if (msg.size() >= MAX_MESSAGE_LENGTH)
                {
                    msg = msg.substr(0, MAX_MESSAGE_LENGTH - 1);
                }

                WaitForSingleObject(hEmpty, INFINITE);
                WaitForSingleObject(hMutex, INFINITE);

                std::fstream fs(filename, std::ios::in | std::ios::out | std::ios::binary);
                if (!fs) throw std::runtime_error("Failed to open file");

                fs.seekp(buffer->tail * MAX_MESSAGE_LENGTH, std::ios::beg);
                std::string padded = msg;
                padded.append(MAX_MESSAGE_LENGTH - padded.size(), '\0');
                fs.write(padded.c_str(), MAX_MESSAGE_LENGTH);
                fs.close();

                buffer->tail = (buffer->tail + 1) % capacity;

                ReleaseSemaphore(hMutex, 1, NULL);
                ReleaseSemaphore(hFull, 1, NULL);

                std::cout << "Message sent." << std::endl;
            }
        }

        UnmapViewOfFile(buffer);
        CloseHandle(hMap);
        CloseHandle(hMutex);
        CloseHandle(hEmpty);
        CloseHandle(hFull);
        CloseHandle(hReady);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        system("pause");
        return 1;
    }
    return 0;
}
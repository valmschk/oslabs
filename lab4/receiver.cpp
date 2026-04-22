#include "common.h"
#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>

int main()
{
    try
    {
        std::string filename;
        std::cout << "Enter filename: ";
        std::cin >> filename;

        int capacity;
        std::cout << "Enter number of messages in queue: ";
        std::cin >> capacity;

        int numSenders;
        std::cout << "Enter number of senders: ";
        std::cin >> numSenders;

        HANDLE hMutex = CreateSemaphoreA(NULL, 1, 1, "MutexAccess");
        if (!hMutex) throw std::runtime_error("Failed to create mutex semaphore");

        HANDLE hEmpty = CreateSemaphoreA(NULL, capacity, capacity, "EmptySlots");
        if (!hEmpty) throw std::runtime_error("Failed to create empty semaphore");

        HANDLE hFull = CreateSemaphoreA(NULL, 0, capacity, "FullSlots");
        if (!hFull) throw std::runtime_error("Failed to create full semaphore");

        std::ofstream file(filename, std::ios::binary | std::ios::trunc);
        if (!file) throw std::runtime_error("Failed to create file");
        std::string emptyData(capacity * MAX_MESSAGE_LENGTH, '\0');
        file.write(emptyData.c_str(), emptyData.size());
        file.close();

        HANDLE hMap = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(RingBuffer), "GlobalRingBuffer");
        if (!hMap) throw std::runtime_error("Failed to create shared memory");

        RingBuffer* buffer = static_cast<RingBuffer*>(MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(RingBuffer)));
        if (!buffer) throw std::runtime_error("Failed to map shared memory");

        buffer->head = 0;
        buffer->tail = 0;

        std::vector<HANDLE> senderReadyEvents;
        for (int i = 0; i < numSenders; ++i)
        {
            std::string evName = "SenderReady" + std::to_string(i);
            HANDLE hEvent = CreateEventA(NULL, TRUE, FALSE, evName.c_str());
            if (!hEvent) throw std::runtime_error("Failed to create sender ready event");
            senderReadyEvents.push_back(hEvent);
        }

        for (int i = 0; i < numSenders; ++i)
        {
            std::string cmd = "Sender.exe " + std::to_string(i) + " " + std::to_string(capacity) + " " + filename;
            char cmdBuffer[256];
            strncpy(cmdBuffer, cmd.c_str(), sizeof(cmdBuffer));

            STARTUPINFOA si;
            PROCESS_INFORMATION pi;
            ZeroMemory(&si, sizeof(STARTUPINFOA));
            si.cb = sizeof(STARTUPINFOA);

            if (!CreateProcessA(NULL, cmdBuffer, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
            {
                throw std::runtime_error("Failed to start sender process");
            }
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }

        for (HANDLE hEvent : senderReadyEvents)
        {
            WaitForSingleObject(hEvent, INFINITE);
        }

        std::cout << "All senders ready!" << std::endl;

        while (true)
        {
            std::cout << "\nEnter command (read/exit): ";
            std::string cmd;
            std::cin >> cmd;

            if (cmd == "exit") break;
            if (cmd == "read")
            {
                WaitForSingleObject(hFull, INFINITE);
                WaitForSingleObject(hMutex, INFINITE);

                std::fstream fs(filename, std::ios::in | std::ios::out | std::ios::binary);
                if (!fs) throw std::runtime_error("Failed to open file");

                fs.seekg(buffer->head * MAX_MESSAGE_LENGTH, std::ios::beg);
                char msg[MAX_MESSAGE_LENGTH] = { 0 };
                fs.read(msg, MAX_MESSAGE_LENGTH);
                std::cout << "Received: " << msg << std::endl;

                fs.seekp(buffer->head * MAX_MESSAGE_LENGTH, std::ios::beg);
                std::string emptySlot(MAX_MESSAGE_LENGTH, '\0');
                fs.write(emptySlot.c_str(), MAX_MESSAGE_LENGTH);
                fs.close();

                buffer->head = (buffer->head + 1) % capacity;

                ReleaseSemaphore(hMutex, 1, NULL);
                ReleaseSemaphore(hEmpty, 1, NULL);
            }
        }

        UnmapViewOfFile(buffer);
        CloseHandle(hMap);
        CloseHandle(hMutex);
        CloseHandle(hEmpty);
        CloseHandle(hFull);
        for (HANDLE hEvent : senderReadyEvents) CloseHandle(hEvent);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
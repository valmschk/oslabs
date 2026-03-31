#include <iostream>
#include <vector>
#include <memory>
#include "Array.h"
#include "Marker.h"

HANDLE startEvent;

int main() {
    int size;
    std::cout << "Step 1: Enter array size: ";
    std::cin >> size;
    auto arr = std::make_shared<Array>(size);

    int mCount;
    std::cout << "Step 2: Enter number of marker threads: ";
    std::cin >> mCount;

    startEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    std::vector<MarkerData> markers(mCount);

    for (int i = 0; i < mCount; ++i) {
        markers[i].id = i + 1;
        markers[i].active = true;
        markers[i].sharedArray = arr;

        markers[i].blockedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

        markers[i].controlEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

        markers[i].threadHandle = CreateThread(NULL, 0, MarkerProc, &markers[i], 0, NULL);
    }

    SetEvent(startEvent);

    int activeCount = mCount;
    while (activeCount > 0) {
        std::vector<HANDLE> waitHandles;
        for (int i = 0; i < mCount; i++) {
            if (markers[i].active) {
                waitHandles.push_back(markers[i].blockedEvent);
            }
        }

        if (!waitHandles.empty()) {
            WaitForMultipleObjects((DWORD)waitHandles.size(), waitHandles.data(), TRUE, INFINITE);
        }

        Sleep(50);

        std::cout << "\n ";
        arr->print();
        std::cout << " ";

        int targetId;
        std::cout << "\nChoose thread ID to terminate (1 to " << mCount << "): ";
        std::cin >> targetId;

        int idx = targetId - 1;
        if (idx >= 0 && idx < mCount && markers[idx].active) {
            markers[idx].terminate = true;
            SetEvent(markers[idx].controlEvent);

            WaitForSingleObject(markers[idx].threadHandle, INFINITE);
            activeCount--;

            std::cout << "\n>>> Thread " << targetId << " terminated and its marks cleared.";
            arr->print();

            for (int i = 0; i < mCount; i++) {
                if (markers[i].active) {
                    ResetEvent(markers[i].blockedEvent); 
                    SetEvent(markers[i].controlEvent);  
                }
            }
        }
        else {
            std::cout << "\n[Error] Invalid ID or thread already dead. Try again.";
        }
    }

    for (int i = 0; i < mCount; i++) {
        CloseHandle(markers[i].blockedEvent);
        CloseHandle(markers[i].controlEvent);
        CloseHandle(markers[i].threadHandle);
    }
    CloseHandle(startEvent);

    std::cout << "\nALL THREADS FINISHED. MISSION ACCOMPLISHED." << std::endl;

    std::cout << "Press Enter to exit...";
    std::cin.ignore();
    std::cin.get();

    return 0;
}
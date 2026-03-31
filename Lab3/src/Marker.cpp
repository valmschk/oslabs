#include "Marker.h"
#include <iostream>

extern HANDLE startEvent;

DWORD WINAPI MarkerProc(LPVOID param) {
    MarkerData* data = static_cast<MarkerData*>(param);
    std::shared_ptr<Array> arr = data->sharedArray;

    WaitForSingleObject(startEvent, INFINITE);
    srand(data->id);

    while (true) {
        int index = rand() % arr->getSize();

        if (arr->get(index) == 0) {
            Sleep(5);
            arr->set(index, data->id);
            Sleep(5);
        }
        else {
            int count = 0;
            for (int i = 0; i < arr->getSize(); ++i) {
                if (arr->get(i) == data->id) count++;
            }

            std::cout << "\n[Marker " << data->id << "] Blocked! Marks: " << count
                << " | Last failed index: " << index;

            SetEvent(data->blockedEvent);

            WaitForSingleObject(data->controlEvent, INFINITE);

            if (data->terminate) {
                arr->clearValue(data->id);
                data->active = false;
                return 0;
            }
        }
    }
}
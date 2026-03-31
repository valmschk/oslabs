#ifndef MARKER_H
#define MARKER_H

#include <windows.h>
#include <memory>
#include "Array.h"

struct MarkerData {
    int id;
    HANDLE blockedEvent; 
    HANDLE controlEvent; 
    HANDLE threadHandle;
    bool terminate;
    bool active;
    std::shared_ptr<Array> sharedArray;

    MarkerData() : id(0), blockedEvent(NULL), controlEvent(NULL),
        threadHandle(NULL), terminate(false), active(false) {
    }
};

DWORD WINAPI MarkerProc(LPVOID param);

#endif
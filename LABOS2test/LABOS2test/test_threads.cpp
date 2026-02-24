#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define NOMINMAX
#include "doctest.h"
#include "data.h"
#include "minmax.h"
#include "average.h"
#include <windows.h>

TEST_CASE("Testing thread calculations") {
    Data data(5);
    data.push(10, 0);
    data.push(2, 1);
    data.push(30, 2);
    data.push(4, 3);
    data.push(50, 4);

    HANDLE hMinMax = CreateThread(NULL, 0, min_max_thread, &data, 0, NULL);
    HANDLE hAverage = CreateThread(NULL, 0, average_thread, &data, 0, NULL);

    WaitForSingleObject(hMinMax, INFINITE);
    WaitForSingleObject(hAverage, INFINITE);

    CHECK(data.min == 2);
    CHECK(data.max == 50);
    CHECK(data.average == doctest::Approx(19.2));

    CloseHandle(hMinMax);
    CloseHandle(hAverage);
}
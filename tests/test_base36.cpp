#include "model/Base36.h"
#include <cassert>
#include <string>
#include <iostream>

void test_base36() {
    using namespace Model;

    // toInt basics
    assert(base36ToInt("01") == 1);
    assert(base36ToInt("0A") == 10);
    assert(base36ToInt("ZZ") == 1295);
    assert(base36ToInt("00") == -1); // 00 is invalid
    assert(base36ToInt("A0") == 10 * 36);

    // fromInt basics
    assert(intToBase36(1) == "01");
    assert(intToBase36(1295) == "ZZ");
    assert(intToBase36(0) == "00");   // out of range
    assert(intToBase36(1296) == "00"); // out of range

    // round-trip
    for (int i = 1; i <= 1295; ++i) {
        std::string s = intToBase36(i);
        assert(base36ToInt(s.c_str()) == i);
    }

    std::cout << "test_base36: PASS\n";
}

// Called from test_main.cpp

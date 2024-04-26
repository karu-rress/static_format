/**
*
* test.cpp
*
* Copyright 2024 Sunwoo Na.
* All rights reserved.
*
*
* Test code for static_format.hpp
*
* Usage:
*
*    g++ -std=c++20 -O2 test1.cpp -o test1
*                   ^~~ optimization option is optional
*
* Last update: 2024/04/27
*
*/

#include <chrono>
#include <iostream>
#include <sstream>

#include "static_format.hpp"

using namespace std;
using namespace std::chrono;
using namespace std::literals;

int main() {
    constexpr auto loops{10'000'000ULL};

    constexpr int no1 = 1, no2 = 2;
    constexpr const char name1[] = "Alice", name2[] = "Bob";
    constexpr double height1 = 161.75, height2 = 176.1;

    system_clock::time_point start, end;

/////// 1. Traditional C++ style stringstream
    start = system_clock::now();
    for (std::size_t i{}; i < loops; i++) {
        stringstream ss;

        ss << "Student #" << no1 << " : " << name1 << ", height = " << height1 << "cm" << endl;
        ss << "Student #" << no2 << " : " << name2 << ", height = " << height2 << "cm" << endl;
        ss.str();
    }
    end = system_clock::now();
    cout << "[stringstream] Time elapsed: "
        << duration_cast<milliseconds>(end - start) << endl;

/////// 2. C++20's std::format
    start = system_clock::now();
    for (std::size_t i{}; i < loops; i++)
        format("Student #{} : {}, height = {}cm\n""Student #{} : {}, height = {}cm\n",
            no1, name1, height1, no2, name2, height2);

    end = system_clock::now();
    cout << "[format] Time elapsed: "
        << duration_cast<milliseconds>(end - start) << endl;

/////// 3. static_format (Compile-time executed)
    start = high_resolution_clock::now();
    for (std::size_t i{}; i < loops; i++)
        sf::static_format("Student #{} : {}, height = {}cm\nStudent #{} : {}, height = {}cm\n",
            no1, name1, height1, no2, name2, height2);

    end = high_resolution_clock::now();
    cout << "[static_format] Time elapsed: "
        << duration_cast<milliseconds>(end - start) << endl;

    return 0;
}
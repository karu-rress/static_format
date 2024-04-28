# static_format
Constexpr version of `std::format()`.  

# Requirements
This code works in gcc, using C++20 or newer.  
(NOT WORKINNG IN MSVC!)  

# Compile & Running
Unix/macOS/Linux:
```bash
$ g++ test.cpp -std=c++20 -o test && ./test
```
Windows:
```cmd
C:\(Folder)\> g++ test.cpp -std=c++20 -o test.exe && test.exe
```

# References
1. [Constexpr `itoa()` using C++1y](https://stackoverflow.com/questions/23444818/how-to-generate-a-constexpr-version-string-from-three-integers-or-perhaps-a-git)

2. [Implementing `std::format()` using C++17](https://codereview.stackexchange.com/questions/269425/implementing-stdformat)

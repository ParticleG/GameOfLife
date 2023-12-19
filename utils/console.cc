#include <cstdint>
#include <cstdio>
#include <format>

#include <utils/console.h>

#include <windows.h>

using namespace std;
using namespace utils;

void console::setConsoleSize(const int height, const int width) {
    DWORD CurrentMode;
    GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &CurrentMode);
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), CurrentMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    fputs(format("\x1b[8;{};{}t", height, (width + 1) * 2).c_str(), stdout);
}

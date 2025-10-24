#ifndef SPECIAL_CHARACTERS_H
#define SPECIAL_CHARACTERS_H
#include "maze.h"

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>
#include <wchar.h>
#include <stdint.h>

typedef wchar_t spchar_t;

spchar_t get_box_char(direction_t dirs) {
    switch (dirs) {
        case 0:
            return L' ';
        case NORTH: 
            return L' ';
        case SOUTH: 
            return L' ';
        case EAST:
            return L' '; 
        case WEST:
            return L' ';

        case NORTH | EAST:
            return L'╚';
        case NORTH | SOUTH:
            return L'║';
        case NORTH | WEST:
            return L'╝';
        case EAST | SOUTH:
            return L'╔';
        case EAST | WEST:
            return L'═';
        case SOUTH | WEST:
            return L'╗';

        case NORTH | EAST | SOUTH:
            return L'╠';
        case NORTH | EAST | WEST:
            return L'╩';
        case NORTH | SOUTH | WEST:
            return L'╣';
        case EAST | SOUTH | WEST:
            return L'╦';
        case NORTH | EAST | SOUTH | WEST:
            return L'╬';
        default:
            return L'?'; 
    }
}

void print_spchar(spchar_t ch) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) return;

    DWORD written = 0;
    WriteConsoleW(hConsole, &ch, 1, &written, NULL);
}

#else

#include <wchar.h>
#include <stdbool.h>
#include <locale.h>
#include <stdarg.h>

typedef wchar_t spchar_t;
spchar_t get_box_char(direction_t dirs) {
    switch (dirs) {
        case 0:
            return L' ';
        case NORTH: 
            return L' ';
        case SOUTH: 
            return L' ';
        case EAST:
            return L' '; 
        case WEST:
            return L' ';

        case NORTH | EAST:
            return L'╚';
        case NORTH | SOUTH:
            return L'║';
        case NORTH | WEST:
            return L'╝';
        case EAST | SOUTH:
            return L'╔';
        case EAST | WEST:
            return L'═';
        case SOUTH | WEST:
            return L'╗';

        case NORTH | EAST | SOUTH:
            return L'╠';
        case NORTH | EAST | WEST:
            return L'╩';
        case NORTH | SOUTH | WEST:
            return L'╣';
        case EAST | SOUTH | WEST:
            return L'╦';
        case NORTH | EAST | SOUTH | WEST:
            return L'╬';
        default:
            return L'?'; 
    }
}


static bool locale_is_set = false;

void print_spchar(spchar_t ch){
    if(!locale_is_set){
        setlocale(LC_CTYPE, "");
        locale_is_set = true; 
    }
    wprintf(L"%lc",ch);
}
#endif

#endif
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
            return L'.';
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

typedef char spchar_t;
spchar_t get_box_char(direction_t dirs) {
    switch (dirs) {
        case 0:
            return '·';
        case NORTH: case SOUTH: 
            return '║';
        case EAST: case WEST:
            return '═';
        case NORTH | EAST:
            return '╚';
        case NORTH | SOUTH:
            return '║';
        case NORTH | WEST:
            return '╝';
        case EAST | SOUTH:
            return '╔';
        case EAST | WEST:
            return '═';
        case SOUTH | WEST:
            return '╗';

        case NORTH | EAST | SOUTH:
            return '╠';
        case NORTH | EAST | WEST:
            return '╩';
        case NORTH | SOUTH | WEST:
            return '╣';
        case EAST | SOUTH | WEST:
            return '╦';
        case NORTH | EAST | SOUTH | WEST:
            return '╬';
        default:
            return '?'; 
    }
}

void print_spchar(spchar_t ch){
    printf("%c",ch);
}
#endif

#endif
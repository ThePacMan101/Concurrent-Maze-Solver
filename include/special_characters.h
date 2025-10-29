#ifndef SPECIAL_CHARACTERS_H
#define SPECIAL_CHARACTERS_H
#include "maze.h"
#include <wchar.h>

typedef wchar_t spchar_t;

spchar_t get_box_char(direction_t dirs);

void print_spchar(spchar_t ch);


#endif
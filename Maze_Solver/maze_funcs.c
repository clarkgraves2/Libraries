#include "maze.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

bool is_valid_char(char c) {
    return c == 'S' || c == 'G' || c == '.' || c == '#';
}
#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>

#define WALL ('#')
#define SPACE (' ')
#define START ('S')
#define END ('G')

typedef struct {
    char symbol;
    int row;
    int col;
    struct vertex_t* parent;
    float g_cost;
    float h_cost;
    float f_cost;
} vertex_t;

typedef struct {
    vertex_t** matrix;
    int rows;
    int cols;
} maze_t;

bool is_valid_char(char c);
maze_t * read_file_and_create_matrix(const char* filename);
void print_maze(const maze_t* maze);
void free_maze(maze_t* maze);

#endif // MAZE_H
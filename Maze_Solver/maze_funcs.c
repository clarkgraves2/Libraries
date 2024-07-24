#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "maze.h"

bool is_valid_char(char c) {
    return (c == WALL || c == SPACE || c == START || c == END);
}

maze_t* read_file_and_create_matrix(const char* filename)
{
    FILE* file = fopen(filename, "r");
    
    if (file == NULL) 
    {
        fprintf(stderr, "Error opening file.\n");
        return NULL;
    }

    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    int max_cols = 0;
    int current_row = 0;
    int allocated_rows = 10;  // Start with 10 rows, expand as needed
    
    vertex_t** matrix = calloc(allocated_rows, sizeof(vertex_t*));
    if (matrix == NULL) 
    {
        fprintf(stderr, "Initial memory allocation failed.\n");
        fclose(file);
        return NULL;
    }

    while ((read = getline(&line, &len, file)) != -1) {
        int valid_cols = 0;
        for (int i = 0; i < read - 1; i++) {
            if (is_valid_char(line[i])) 
            {
                valid_cols++;
            }
        }

        if (valid_cols > max_cols) 
        {
            max_cols = valid_cols;
        }

        if (valid_cols > 0) {  // Only create a row if there are valid characters
            if (current_row >= allocated_rows) 
            {
                allocated_rows *= 2;
                vertex_t** new_matrix = realloc(matrix, allocated_rows * sizeof(vertex_t*));
                if (new_matrix == NULL) 
                {
                    fprintf(stderr, "Memory reallocation failed.\n");
                    for (int i = 0; i < current_row; i++) 
                    {
                        free(matrix[i]);
                    }
                    free(matrix);
                    free(line);
                    fclose(file);
                    return NULL;
                }
                matrix = new_matrix;
            }

            matrix[current_row] = malloc(valid_cols * sizeof(vertex_t));
            
            if (matrix[current_row] == NULL) 
            {
                fprintf(stderr, "Row memory allocation failed.\n");
                for (int i = 0; i < current_row; i++) 
                {
                    free(matrix[i]);
                }
                free(matrix);
                free(line);
                fclose(file);
                return NULL;
            }

            int col = 0;
            for (int i = 0; i < read - 1; i++) 
            {
                if (is_valid_char(line[i])) 
                {
                    matrix[current_row][col].symbol = line[i];
                    matrix[current_row][col].row = current_row;
                    matrix[current_row][col].col = col;
                    matrix[current_row][col].parent = NULL;
                    matrix[current_row][col].g_cost = FLT_MAX;
                    matrix[current_row][col].h_cost = 0;
                    matrix[current_row][col].f_cost = FLT_MAX;
                    col++;
                }
            }
            current_row++;
        }
    }

    maze_t* maze = malloc(sizeof(maze_t));
    if (maze == NULL) {
        fprintf(stderr, "Maze structure allocation failed.\n");
        for (int i = 0; i < current_row; i++) 
        {
            free(matrix[i]);
        }
        free(matrix);
        free(line);
        fclose(file);
        return NULL;
    }

    maze->matrix = matrix;
    maze->rows = current_row;
    maze->cols = max_cols;

    free(line);
    fclose(file);
    return maze;
}

void print_maze(const maze_t* maze) 
{
    if (maze == NULL || maze->matrix == NULL) {
        fprintf(stderr, "Invalid maze structure.\n");
        return;
    }

    for (int y = 0; y < maze->rows; y++) 
    {
        for (int x = 0; x < maze->cols; x++) 
        {
            printf("%c", maze->matrix[y][x].symbol);
        }
        printf("\n");
    }
}

void free_maze(maze_t* maze)
{
    if (maze == NULL) return;
    
    if (maze->matrix != NULL) {
        for (int i = 0; i < maze->rows; i++) 
        {
            free(maze->matrix[i]);
        }
        free(maze->matrix);
    }
    
    free(maze);
}

maze_t * find_maze_path(maze_t * maze)
{
    vertex_t *start = NULL, *end = NULL;
    
    for (int r = 0; r < maze->rows; r++) 
    {
        for (int c = 0; c < maze->cols; c++) 
        {
            if (maze->matrix[r][c].symbol == START) 
            {
                start = &maze->matrix[r][c];
            } 
            else if (maze->matrix[r][c].symbol == END) 
            {
                end = &maze->matrix[r][c];
            }

            if (start && end)
            {
                break;
            }
        } 
        
        if (start && end)
        {
            break;
        }
    }
}
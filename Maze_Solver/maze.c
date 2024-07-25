#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "../Priority_Queue/priority_queue.h"
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
    
    // Finding start and end points
    for (int row = 0; row < maze->rows; row++) 
    {
        for (int col = 0; col < maze->cols; col++) 
        {
            maze->matrix[row][col].visited = false; // Initialize all vertices as unvisited
            if (maze->matrix[row][col].symbol == START) 
            {
                start = &maze->matrix[row][col];
            } 
            else if (maze->matrix[row][col].symbol == END) 
            {
                end = &maze->matrix[row][col];
            }
            if (start && end) break;
        } 
        if (start && end) break;
    }

    if (!start || !end) 
    {
        fprintf(stderr, "Start or end not found in maze\n");
        return NULL;
    }

   
    start->g_cost = 0;
    start->h_cost = calculate_heuristic(start, end);
    start->f_cost = start->g_cost + start->h_cost;
    start->parent = NULL;

    pqueue_t* priority_queue = pqueue_create((int (*)(const void*, const void*))compare_vertices);
    pqueue_insert(priority_queue, start->f_cost, start);

    while(!pqueue_is_empty(priority_queue))
    {
        vertex_t *current = pqueue_extract(priority_queue);
        
        if (current->symbol == END) 
        {
            pqueue_destroy(priority_queue);
            return reconstruct_path(maze, current);
        }
        
        current->visited = true;  
        
        int dr[] = {-1, 1, 0, 0};
        int dc[] = {0, 0, -1, 1};
        
        for (int i = 0; i < 4; i++) 
        {
            int new_row = current->row + dr[i];
            int new_col = current->col + dc[i];
            
            if (new_row < 0 || new_row >= maze->rows || new_col < 0 || new_col >= maze->cols) 
            {
                continue;  
            }
            
            vertex_t *neighbor = &maze->matrix[new_row][new_col];
            
            if (neighbor->visited || neighbor->symbol == WALL) 
            {
                continue;
            }
            
            float tentative_g_cost = current->g_cost + 1;
            
            if (tentative_g_cost < neighbor->g_cost) 
            {
                neighbor->parent = current;
                neighbor->g_cost = tentative_g_cost;
                neighbor->h_cost = calculate_heuristic(neighbor, end);
                neighbor->f_cost = neighbor->g_cost + neighbor->h_cost;
                
                if (!pqueue_contains(priority_queue, neighbor)) 
                {
                    pqueue_insert(priority_queue, neighbor->f_cost, neighbor);
                } 
                else 
                {
                    pqueue_change_priority(priority_queue, neighbor, neighbor->f_cost);
                }
            }
        }
    }

    pqueue_destroy(priority_queue);
    return NULL; // No path found
}

int compare_vertices(const void* a, const void* b) 
{
    const vertex_t* va = (const vertex_t*)a;
    const vertex_t* vb = (const vertex_t*)b;
    if (va->f_cost < vb->f_cost) return -1;
    if (va->f_cost > vb->f_cost) return 1;
    return 0;
}

float calculate_heuristic(vertex_t* a, vertex_t* b) 
{
    // Manhattan distance
    return abs(a->row - b->row) + abs(a->col - b->col);
}

maze_t* reconstruct_path(maze_t* maze, vertex_t* end) 
{
    vertex_t* current = end;
    while (current) 
    {
        if (current->symbol != START && current->symbol != END) 
        {
            current->symbol = '*'; 
        }
        current = current->parent;
    }
    return maze; 
}

#include "maze.h"
#include "../Priority_Queue/priority_queue.h"
#include <stdbool.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define GETOPTERROR (0)
#define GETOPT (1)
#define MINROWELEMENTS (1)

int
main (int argc, char * argv[])
{
    int cdx;
    int option = 1;
    bool v_opt_selected = false;

    // Get opt module to handle command line arguments.
    //
    while (option)
    {
        // get-opt long option handling and customization
        //
        int                  option_index   = 0;
        static struct option long_options[] = 
        {
            {"verbose", no_argument, NULL, 'v'},
            {     NULL,           0, NULL,   0}
        };

        // get-opt variable / option handling customization
        //
        cdx = getopt_long(argc, argv, "-:v:", long_options, &option_index);

        // If no option or filename provided program exits.
        //
        if (GETOPTERROR == cdx)
        {
            break;
        }
        
        switch (cdx)
        {
            case 1: // case to handle filename "long" option
                break;
            case 'v': // case to handle -v option of printing all hole sizes
                v_opt_selected = true;
                break;
            case '?': // Unknown option handling.
                printf ("Unknown option %c\n", optopt);
                break;
            default:
                option = 0;
                break; // Required Default option if above cases fail.
               

        }
    }

    // No file provided or only -v option provided error
    //
    if ((argc < 2) || (argc < 3 && true == v_opt_selected))
    {
        fprintf(stderr, "Error: No file or directory - Exiting\n");
        exit(EXIT_FAILURE);
    }

    // To many arguments provided error checking
    //
    if (argc > 3)
    {
        fprintf(stderr,"Error: Too Many Arguments - Exiting\n");
        exit(EXIT_FAILURE);
    }
    
    const char * filename = argv[1];

    maze_t * maze = read_file_and_create_matrix(filename); 
   
    if (maze != NULL) 
    {
        print_maze(maze);
        // Use the maze for pathfinding or other operations
        free_maze(maze);
    } 
    else 
    {
        fprintf(stderr, "Failed to create maze.\n");
    }
    
    return 0;
}


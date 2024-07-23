#include "maze.h"
#include <stdbool.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct vertex
{
    char symbol;
    int col;
    int row;
    struct vertex* parent;
    float g_cost;  // Cost from start to this node
    float h_cost;  // Heuristic cost from this node to goal
    float f_cost;  // Total cost (g_cost + h_cost)
} vertex_t;

#define GETOPTERROR (0)
#define GETOPT (1)
#define MINROWELEMENTS (1)



int
main (int argc, char * argv[])
{
    int cdx;
    bool v_opt_selected = false;

    // Get opt module to handle command line arguments.
    //
    while (GETOPT)
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
        cdx = getopt_long (argc, argv, "-:v", long_options, &option_index);

        // If no option or filename provided program exits.
        //
        if (GETOPTERROR == cdx)
        {
            break;
        }
        
        switch (cdx)
        {
            case GETOPT: // case to handle filename "long" option
                break;
            case 'v': // case to handle -v option of printing all hole sizes
                v_opt_selected = true;
                break;
            case '?': // Unknown option handling.
                printf ("Unknown option %c\n", optopt);
                break;
            default: // Required Default option if above cases fail.
                printf ("Wrong Combination of File and/or option");
                break;
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

    // Assigning userprovided file name in command line to variable.
    //
    const char * filename = argv[1];

    // Error checking for filename assignment error.
    if (NULL == filename)
    {
        fprintf(stderr,"Error: Failed to assign filename\n");
        exit(EXIT_FAILURE);
    }

    FILE * file_pointer;

    // Opening file to process and retrieve data.
    //
    file_pointer = fopen (filename, "r");

    // Error Checking for file not opening correctly.
    //
    if (NULL == file_pointer)
    {
        fprintf(stderr,"Error opening file - Exiting\n");
        exit(EXIT_FAILURE);
    }

    int valid_char_count = 0;
}


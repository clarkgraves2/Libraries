#include <stdio.h>
#include <string.h>
#include <regex.h>
#include "./Hash_Table/hash_table.h"
#include "./Linked_List/linked_list.h"

#define MAX_AIRPORTS (100)
#define FILE_ERROR (0)
#define LINE_SIZE (11)

typedef struct airport
{
    const char * name;
    airport_t * connection;
}airport_t;

int 
main(void)
{

FILE * fptr = fopen("airports.txt", "r");

if (NULL == fptr)
{
    return FILE_ERROR; 
}

regex_t regex;
int input_val;
int ret;

if (regcomp(&regex, "[A-Z]{3} -- [A-Z]{3}", 0)) 
{
        fprintf(stderr, "Could not compile regex\n");
        return 1;
}

hash_table_t * hashtable = create_hash_table(MAX_AIRPORTS, simple_hash);

char buffer[LINE_SIZE];

 while (fgets(buffer, sizeof(buffer), fptr)) 
 {
        // Remove newline character if present
        buffer[strcspn(buffer, "\n")] = 0;

        // Check if line matches the pattern
        if (regexec(&regex, buffer, 0, NULL, 0) == 0) 
        {
            char flight1[4];
            char flight2[4];
            
            // If it matches, use sscanf to extract values
            if (sscanf(buffer, "%s -- %s", flight1, flight2) == 2) 
            {
                hash_table_insert()


            } 
            else 
            {
                printf("Failed to parse line: %s\n", buffer);
            }
        } 
        else 
        {
            printf("Line does not match pattern: %s\n", buffer);
        }
    }



    fclose(fptr);
    regfree(&regex);
    return 0;
}


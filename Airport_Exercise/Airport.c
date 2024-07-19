#include <stdio.h>
#include <string.h>
#include <regex.h>
#include "../Hash_Table/hash_table.h"
#include "../Linked_List/linked_list.h"

#define MAX_AIRPORTS (100)
#define FILE_ERROR (0)
#define LINE_SIZE (11)

uint64_t simple_hash(const char *str, size_t size);

typedef struct airport
{
    const char * name;
    struct airport_t * connection;
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

if (regcomp(&regex, "[A-Z]{3} -- [A-Z]{3}", 0)) 
{
        fprintf(stderr, "Could not compile regex\n");
        return 1;
}
uint32_t size = 100;

hash_table_t * airport_htable = create_hash_table(size, simple_hash);

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
                if(NULL == hash_table_lookup(airport_htable, flight1))
                {
                    hash_table_insert(airport_htable, flight1, NULL);
                }

                if(NULL == hash_table_lookup(airport_htable, flight2))
                {
                    hash_table_insert(airport_htable, flight2, NULL);
                }

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

    print_hash_table(airport_htable);

    fclose(fptr);
    regfree(&regex);
    return 0;
}

uint64_t simple_hash(const char *str, size_t size) 
{
    uint64_t hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash % size;
}
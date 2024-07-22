#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>
#include "../Hash_Table/hash_table.h"
#include "../Linked_List/linked_list.h"

#define STARTING_SIZE (256)
#define FILE_ERROR (0)
#define LINE_SIZE (20)
#define ACK_LEN (3)
#define DST_IDX (7)



typedef struct airport
{
    const char * name;
    struct airport_t * connection;
}airport_t;


uint64_t simple_hash(const char *str, size_t size);
airport_t * airport_create (const char *p_name);

int main(void)
{
    FILE * fptr = fopen("airports.txt", "r");
    if (NULL == fptr)
    {
        perror("Error opening file");
        return FILE_ERROR; 
    }
    
    hash_table_t * airport_htable = create_hash_table(STARTING_SIZE, simple_hash);
    
    if (airport_htable == NULL)
    {
        fprintf(stderr, "Failed to create hash table\n");
        fclose(fptr);
        return 1;
    }

    regex_t regex  = { 0 };
    int b_is_regex = regcomp(&regex, "^[A-Z]{3} -- [A-Z]{3}$", REG_EXTENDED);
    if (0 != b_is_regex)
    {
        fprintf(stderr, "[ERR]: Could not compile regex\n");
        return 0;
    }

    char buffer[LINE_SIZE];
    size_t  size   = 0;
    ssize_t len    = 0;

    while (fgets(buffer, sizeof(buffer), fptr)) 
    {

        if ('\n' == buffer[len - 1])
        {
            buffer[--len] = '\0';
        }
        
        b_is_regex = regexec(&regex, buffer, 0, NULL, 0);
        if (REG_NOMATCH == b_is_regex)
        {
            fprintf(stderr, "[ERR]: Invalid Line: %s\n", buffer);
            continue;
        }
        
        buffer[ACK_LEN] = '\0';
        airport_t *p_src  = hash_table_lookup(airport_htable, buffer);
        
        if (NULL == p_src)
        {
            p_src = airport_create(buffer);
            if (NULL == p_src)
            {
                fprintf(stderr, "[ERR]: Creating Airport: %s\n", buffer);
                continue;
            }
            hash_table_insert(airport_htable, buffer, p_src);
        }

        airport_t *p_dst = hash_table_lookup(airport_htable, buffer + DST_IDX);
        
        if (NULL == p_dst)
        {
            p_dst = airport_create(buffer + DST_IDX);
            if (NULL == p_dst)
            {
                fprintf(stderr, "[ERR]: Creating Airport: %s\n", buffer);
                continue;
            }
            hash_table_insert(airport_htable, buffer + DST_IDX, p_dst);
        }

        insert_back(p_src->connection, p_dst);
        insert_back(p_dst->connection, p_src);
    }
    free(buffer);
    print_hash_table(airport_htable);
    destroy_hash_table(airport_htable);
    regfree(&regex);
    fclose(fptr);
    return 0;
}

uint64_t simple_hash(const char *str, size_t size) 
{
    if (str == NULL) return 0;

    uint64_t hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}

airport_t *
airport_create (const char *p_name)
{
    airport_t *p_airport = NULL;
    node_t   *p_llist   = NULL;

    if (NULL != p_name)
    {
        char *p_air_name = strdup(p_name);
        p_airport        = calloc(1, sizeof(*p_airport));
        p_llist          = create_list();

        if ((NULL == p_airport) || (NULL == p_llist) || (NULL == p_air_name))
        {
           return NULL;
        }
        else
        {
            p_airport->name      = p_air_name;
            p_airport->connection = p_llist;
        }
    }

    return p_airport;
}

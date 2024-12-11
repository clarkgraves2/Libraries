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
    char * name;
    llist_node_t * connection;
} airport_t;

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
        fclose(fptr);
        hash_table_destroy(airport_htable);
        return 1;
    }

    char buffer[LINE_SIZE];
    while (fgets(buffer, sizeof(buffer), fptr)) 
    {
        size_t len = strlen(buffer);
        if (len > 0 && '\n' == buffer[len - 1])
        {
            buffer[len - 1] = '\0';
        }
        
        b_is_regex = regexec(&regex, buffer, 0, NULL, 0);
        if (REG_NOMATCH == b_is_regex)
        {
            fprintf(stderr, "[ERR]: Invalid Line: %s\n", buffer);
            continue;
        }
        
        char src_code[ACK_LEN + 1];
        char dst_code[ACK_LEN + 1];
        strncpy(src_code, buffer, ACK_LEN);
        src_code[ACK_LEN] = '\0';
        strncpy(dst_code, buffer + DST_IDX, ACK_LEN);
        dst_code[ACK_LEN] = '\0';

        airport_t *p_src = hash_table_lookup(airport_htable, src_code);
        if (NULL == p_src)
        {
            p_src = airport_create(src_code);
            if (NULL == p_src)
            {
                fprintf(stderr, "[ERR]: Creating Airport: %s\n", src_code);
                continue;
            }
            hash_table_insert(airport_htable, src_code, p_src);
        }

        airport_t *p_dst = hash_table_lookup(airport_htable, dst_code);
        if (NULL == p_dst)
        {
            p_dst = airport_create(dst_code);
            if (NULL == p_dst)
            {
                fprintf(stderr, "[ERR]: Creating Airport: %s\n", dst_code);
                continue;
            }
            hash_table_insert(airport_htable, dst_code, p_dst);
        }

        llist_insert_back(p_src->connection, p_dst);
        llist_insert_back(p_dst->connection, p_src);
    }

    print_hash_table(airport_htable);
    destroy_hash_table(airport_htable);
    regfree(&regex);
    fclose(fptr);
    return 0;
}

airport_t *
airport_create (const char *p_name)
{
    airport_t *p_airport = NULL;
    llist_node_t   *p_llist   = NULL;

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

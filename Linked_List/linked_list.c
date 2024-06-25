#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fnmatch.h>
#include "linked_list.h"

struct node
{
    struct node *next;
    void        *data;
};

struct llist
{
    node_t *head;
    node_t *tail;
    ssize_t size;
};

llist_t * create_list(void)
{
    llist_t * new_list = calloc(1,sizeof(llist_t));

    if (NULL == new_list)
    {
        return NULL;
    }

    return new_list;
}

ssize_t list_size(llist_t * llist)
{
    if (NULL == llist)
    {
        return -1;
    }

    return llist->size;
}

node_t * create_node(void * data)
{
    node_t * new_node = calloc(1,sizeof(node_t));

    if (NULL == new_node)
    {
        return NULL;
    }
    new_node->data = data;
    new_node->next = NULL;
    return new_node;
}

node_t * insert_front(llist_t * list, void * data)
{
    if (NULL == list || NULL == data)
    {
        return NULL;
    }

    node_t * node_to_insert = create_node(data);
    
    if (NULL == node_to_insert)
    {
        return NULL;
    }

    if (NULL == list->head)
    {
        list->head = node_to_insert;
        list->tail = node_to_insert;
        (list->size)++;
    }
    else
    {
        node_to_insert->next = list->head;
        list->head = node_to_insert;
        (list->size)++;
    }

    return node_to_insert;
}

node_t * insert_back (llist_t * list, void * data)
{
      if (NULL == list || NULL == data)
    {
        return NULL;
    }

    node_t * node_to_insert = create_node(data);
    
    if (NULL == node_to_insert)
    {
        return NULL;
    }

    if (NULL == list->head)
    {
        list->head = node_to_insert;
        list->tail = node_to_insert;
        (list->size)++;
    }
    else
    {
        list->tail->next = node_to_insert;
        list->tail = node_to_insert;
        (list->size)++;
    }

    return node_to_insert;
}

void * get_node_data(node_t *node)
{
    if (node == NULL)
    {
        return NULL;
    }
    return node->data;
}

node_t * get_next_ptr(node_t *node)
{
    if (node == NULL)
    {
        return NULL;
    }
    return node->next;
}
#include "linked_list.h"
#include <stdlib.h>

typedef void (*data_destructor_t)(void *);

struct llist_node 
{
    struct llist_node *next;
    void *data;
};

struct llist 
{
    llist_node_t *head;
    llist_node_t *tail;
    ssize_t size;
    data_destructor_t data_destructor;
};

llist_t *llist_create(data_destructor_t destructor)
{
    llist_t *new_list = calloc(1, sizeof(llist_t));
    if (new_list) {
        new_list->data_destructor = destructor; 
    }
    return new_list;
}

void llist_destroy(llist_t **list)
{
    if (NULL == list || NULL == *list) 
    {
        return;
    }

    llist_node_t *current = (*list)->head;
    
    while (current != NULL) 
    {
        llist_node_t *next = current->next;
        
        if ((*list)->data_destructor) 
        {
            (*list)->data_destructor(current->data); 
        }

        free(current);
        current = next;
    }
    free(*list);
    *list = NULL;
}

ssize_t 
llist_size(const llist_t *list)
{
    return (list != NULL) ? list->size : -1;
}

llist_node_t *
llist_create_node(void *data)
{
    llist_node_t *new_node = calloc(1, sizeof(llist_node_t));
    
    if (new_node != NULL) 
    {
        new_node->data = data;
    }
    return new_node;
}

llist_node_t *
llist_insert_front(llist_t *list, void *data)
{
    if (list == NULL) {
        return NULL;
    }

    llist_node_t *
    node_to_insert = llist_create_node(data);
    if (node_to_insert == NULL) {
        return NULL;
    }

    node_to_insert->next = list->head;
    list->head = node_to_insert;

    if (list->tail == NULL) {
        list->tail = node_to_insert;
    }

    list->size++;
    return node_to_insert;
}

llist_node_t *
llist_insert_back(llist_t *list, void *data)
{
    if (list == NULL) {
        return NULL;
    }

    llist_node_t *node_to_insert = llist_create_node(data);
    if (node_to_insert == NULL) {
        return NULL;
    }

    if (list->tail == NULL) {
        list->head = list->tail = node_to_insert;
    } else {
        list->tail->next = node_to_insert;
        list->tail = node_to_insert;
    }

    list->size++;
    return node_to_insert;
}

llist_node_t *
llist_insert_at(llist_t *list, size_t position, void *data)
{
    if (list == NULL || position > (size_t)list->size) {
        return NULL;
    }

    if (position == 0 || list->head == NULL) {
        return llist_insert_front(list, data);
    }

    if (position == (size_t)list->size) {
        return llist_insert_back(list, data);
    }

    llist_node_t *node_to_insert = llist_create_node(data);
    if (node_to_insert == NULL) {
        return NULL;
    }

    llist_node_t *current = list->head;
    for (size_t i = 0; i < position - 1; ++i) {
        current = current->next;
    }

    node_to_insert->next = current->next;
    current->next = node_to_insert;
    list->size++;

    return node_to_insert;
}

int 
llist_delete_node(llist_t *list, const void *data, int (*compare)(const void*, const void*))
{
    if (list == NULL || list->head == NULL || compare == NULL) 
    {
        return -1;
    }

    llist_node_t *current = list->head;
    llist_node_t *prev = NULL;
    
    while (current != NULL) 
    {
        if (compare(current->data, data) == 0) 
        {
            if (prev == NULL) 
            {
                list->head = current->next;
            } 
            else 
            {
                prev->next = current->next;
            }
            
            if (current == list->tail) 
            {
                list->tail = prev;
            }
            
            if (list->data_destructor) 
            {
                list->data_destructor(current->data);
            }
            
            free(current);
            list->size--;
            return 0;
        }
        prev = current;
        current = current->next;
    }
    return -1;
}


void *
llist_get_node_data(const llist_node_t *node)
{
    return (node != NULL) ? node->data : NULL;
}

llist_node_t *
llist_get_next_ptr(const llist_node_t *node)
{
    return (node != NULL) ? node->next : NULL;
}

void 
llist_foreach(const llist_t *list, void (*func)(void *data))
{
    if (NULL == list || NULL == func) 
    {
        return;
    }

    llist_node_t *current = list->head;
    
    while (current != NULL) 
    {
        func(current->data);
        current = current->next;
    }
}
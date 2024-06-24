#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bt.h"

bt_t *
create_new_tree (void)
{
    bt_t * new_bt = calloc (1, sizeof (bt_t));

    if (NULL == new_bt)
    {
        return NULL;
    }

    new_bt->root = NULL;
    return new_bt;
}


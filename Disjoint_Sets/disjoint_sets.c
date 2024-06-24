#include <stdio.h>

typedef struct {
    int *parent;
    int *rank;
    int size;
} disjoint_set;

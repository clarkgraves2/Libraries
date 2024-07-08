#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define NUM_CHARS 256

typedef struct trie_node{
    bool terminal;
    struct trie_node * children[NUM_CHARS];
}trie_node_t;

trie_node_t * create_node()
{
    trie_node_t * new_node = malloc(sizeof(*new_node));

    for (int idx = 0; idx < NUM_CHARS; idx++)
    {
        new_node->children[idx] = NULL;
    }

    new_node->terminal = false;
    return new_node;
}

bool trie_insert (trie_node_t **root, char * signed_text)
{
    if (NULL == *root)
        *root = create_node();

    unsigned char * text = (unsigned char *)signed_text;
    trie_node_t * tmp = *root;
    int legnth = strlen(signed_text);

    for (int jdx = 0; jdx <legnth; jdx++)
    {
        if (NULL == tmp->children[text[jdx]])
        {
            tmp->children[text[jdx]] = create_node();
        }
        tmp = tmp->children[text[jdx]];
    }
        
    if (tmp->terminal)
    {
        return false;
    }
    else
    {
        tmp->terminal = true;
        return true;
    }
}

void print_trie_rec(trie_node_t * node, unsigned char * prefix, int length)
{
    unsigned char newprefix[length + 2];
    memcpy(newprefix, prefix, length);
    newprefix[length + 1] = 0;

    if (node->terminal)
    {
        printf("WORD: %s\n", prefix);
    }

    for (int kdx = 0; kdx < NUM_CHARS; kdx++)
    {
        if (node->children[kdx] != NULL)
        {
            newprefix[length] = kdx;
            print_trie_rec(node->children[kdx], newprefix, length + 1);
        }
    }
}

void print_trie (trie_node_t * root)
{
    if (NULL == root)
    {
        printf("TRIE EMPTY\n");
        return;
    }

    print_trie_rec(root, NULL, 0);
}

bool trie_search(trie_node_t * root, char * signedtext)
{
    unsigned char * text = (unsigned char *) signedtext;
    int length = strlen(signedtext);
    trie_node_t * tmp = root;

    for (int ldx = 0; ldx < length; ldx++)
    {
        if (NULL == tmp->children[text[ldx]])
        {
            return false;
        }
        tmp = tmp->children[text[ldx]];
    }
    return tmp->terminal;
}

bool node_has_children(trie_node_t * node)
{
    if (NULL == node)
    {
        return false;
    }

    for (int mdx =0; mdx <NUM_CHARS; mdx++)
    {
        if (node->children[mdx] != NULL)
        {
            return true;
        }
    }

    return false;
}


trie_node_t * delete_str_recursive(trie_node_t * node, unsigned char * text, bool * deleted)
{
    if (NULL == node)
    {
        return node;
    }

    if (*text == '\0')
    {
        if (node->terminal)
        {
            node->terminal = false;
            *deleted = true;
        
        
            if (false == node_has_children(node))
            {
                free(node);
                node = NULL;
            }
        }
        return node;
    }    

   node->children[text[0]] = delete_str_recursive(node->children[text[0]],text + 1, deleted);

   if (false == *deleted && node_has_children(node) && false == node->terminal)
   {
    free(node);
    node = NULL;
   }
   return node;
}



bool delete_str(trie_node_t ** root, char * signed_text)
{
    unsigned char * text = (unsigned char *)signed_text;
    bool result = false;

    if (NULL == *root)
    {
        return false;
    }

    *root = delete_str_recursive(*root, text, &result);
    return result;
}


int main()
{
    trie_node_t * root = NULL;

    trie_insert(&root, "KIT");
    trie_insert(&root, "CATTLE");
    trie_insert(&root, "KIN");
    trie_insert(&root, "CAT");
    trie_insert(&root, "HAPPY");

    print_trie(root);

    printf("Search for CATTLE: %d\n", trie_search(root, "CATLE"));
    printf("Search for CATTLE: %d\n", trie_search(root, "CATTLE"));
    printf("Search for CATTLE: %d\n", trie_search(root, "KITTEN"));
    printf("Search for CATTLE: %d\n", trie_search(root, "HAPPY"));

    delete_str(&root, "KIN");
    delete_str(&root, "CAT");

    print_trie(root);

}


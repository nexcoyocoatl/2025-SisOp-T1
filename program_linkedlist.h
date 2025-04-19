#ifndef _PROGRAM_LINKEDLIST_H_
#define _PROGRAM_LINKEDLIST_H_

#include <stdlib.h>

struct Node
{
    struct Program *program;
    struct Node* next;
};

struct Program_list
{
    struct Node* head;
    struct Node* tail;
    size_t size;
};

struct Program_list* proglist_create();
size_t proglist_len(struct Program_list* lst);
void proglist_add_node(struct Program_list* lst, struct Program *program);
int proglist_remove_node(struct Program_list* lst, size_t program_id);
int proglist_remove_node_index(struct Program_list* lst, size_t index);
int proglist_index_of(struct Program_list* lst, size_t program_id);
void proglist_dump(struct Program_list* lst);
void proglist_clear(struct Program_list* lst);
void proglist_free_all(struct Program_list* lst);

#endif
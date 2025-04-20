#include <stdio.h>
#include <stdlib.h>

#include "program_linkedlist.h"
#include "program.h"

struct Program_list* proglist_create()
{
    struct Program_list* lst = malloc(sizeof(*lst)); // (WIP) não sei se é isso
    lst->head = NULL;
    lst->tail = NULL;
    lst->size = 0;
    
    return lst;
}

size_t proglist_len(struct Program_list* lst)
{
    return lst->size;
}

void proglist_add_node(struct Program_list* lst, struct Program *program)
{
    struct Node* node = malloc(sizeof(*node));
    node->program = program;
    node->next = NULL;

    if (lst->size == 0)
    {
        lst->head = node;
    }
    else
    {
        struct Node* current = lst->head;
        while(current->next != NULL)
        {
            current = current->next;
        }

        current->next = node;
    }

    lst->tail = node;

    lst->size++;
}

int proglist_remove_node(struct Program_list* lst, size_t program_id) // (WIP)
{
    if (lst->size == 0)
    {
        return 0;
    }

    struct Node* current = lst->head;

    if (current->program->id == program_id)
    {
        lst->head = lst->head->next;
        lst->size--;

        if (lst->size == 0)
        {
            lst->tail = NULL;
        }

        free(current);
        return 1;
    }

    while (current->next != NULL)
    {
        if (current->next->program->id == program_id)
        {
            struct Node* temp = current->next;

            // testar se funciona
            if (temp == lst->tail)
            {
                lst->tail = current;
            }

            current->next = current->next->next;
            lst->size--;

            free(temp);

            if (lst->size == 0)
            {
                lst->tail = NULL;
            }

            return 1;
        }
        current = current->next;
    }

    return 0;
}

int proglist_remove_node_index(struct Program_list* lst, size_t index)
{
    size_t counter = 0;

    if (lst->size == 0)
    {
        return 0;
    }

    struct Node* current = lst->head;

    if (index == 0)
    {
        lst->head = lst->head->next;
        lst->size--;

        if (lst->size == 0)
        {
            lst->tail = NULL;
        }

        free(current);
        return 1;
    }

    while (current->next != NULL)
    {
        counter++;
        if (counter == index)
        {
            struct Node* temp = current->next;

            if (temp == lst->tail)
            {
                lst->tail = current;
            }

            current->next = current->next->next;
            lst->size--;

            free(temp);

            if (lst->size == 0)
            {
                lst->tail = NULL;
            }

            return 2;
        }
        current = current->next;
    }

    return 0;
}

struct Program *proglist_get(struct Program_list* lst, size_t index)
{
    if (lst->size == 0) return NULL;
        
    struct Node* current = lst->head;
    size_t counter = 0;

    while(current != NULL)
    {
        if (counter == index) {
            //printf("get returning pid %lu\n", current->program->id);
            return current->program;
        }
        counter++;
        current = current->next;
    }

    //printf("get null\n");
    return NULL;
}

int proglist_index_of(struct Program_list* lst, size_t program_id)
{
    if (lst->size == 0)
    {
        return -1;
    }

    if (lst->tail->program->id == program_id)
    {
        return lst->size;
    }

    struct Node* current = lst->head;
    size_t count = 0;

    while(current->next != NULL)
    {
        if (current->program->id == program_id)
        {
            return count;
        }
        count++;
        current = current->next;
    }

    return -1;
}

void proglist_dump(struct Program_list* lst)
{
    if (lst->size == 0)
    {
        printf("Empty\n");
        return;
    }

    struct Node* current = lst->head;
    while (current != NULL)
    {
        printf("[\n  id: %lu\n  deadline: %lu\n  instruction_count: %lu\n]\n", current->program->id, current->program->deadline, current->program->instruction_count);
        current = current->next;
    }

    // while (current != NULL)
    // {
    //     printf("[%lu]->", current->program->id);
    //     current = current->next;
    // }
    // printf("NULL\n");
}

void proglist_clear(struct Program_list* lst)
{
    struct Node* current;

    while (lst->head != NULL)
    {
        current = lst->head;
        lst->head = lst->head->next;
        free(current);
        lst->size--;
    }
}

void proglist_free_all(struct Program_list* lst)
{
    struct Node* current;

    while (lst->head != NULL)
    {
        current = lst->head;
        lst->head = lst->head->next;
        free(current->program);
        free(current);
        lst->size--;
    }
}
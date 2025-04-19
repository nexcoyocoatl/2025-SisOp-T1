#include <stdio.h>
#include "program.h"
#include "program_linkedlist.h"

int main()
{
    struct Program p1;
    p1.id = 1;
    p1.deadline = 1;
    p1.instruction_count = 1;
    struct Program p2;
    p2.id = 2;
    p2.deadline = 2;
    p2.instruction_count = 2;
    struct Program p3;
    p3.id = 3;
    p3.deadline = 3;
    p3.instruction_count = 3;

    struct Program_list* proglist1 = proglist_create();
    proglist_dump(proglist1);
    printf("%lu\n", proglist_len(proglist1));
    proglist_add_node(proglist1, &p1);
    proglist_add_node(proglist1, &p2);
    proglist_add_node(proglist1, &p3);
    proglist_dump(proglist1);
    printf("tail: %lu\n", proglist1->tail->program->id);

    printf("length: %lu\n\n", proglist_len(proglist1));
    proglist_remove_node(proglist1, 2);
    proglist_dump(proglist1);
    printf("tail: %lu\n", proglist1->tail->program->id);
    printf("length: %lu\n\n", proglist_len(proglist1));

    proglist_remove_node(proglist1, 3);
    proglist_remove_node(proglist1, 1);
    proglist_dump(proglist1);
    printf("length: %lu\n\n", proglist_len(proglist1));

    proglist_add_node(proglist1, &p2);
    proglist_dump(proglist1);
    printf("tail: %lu\n", proglist1->tail->program->id);
    printf("length: %lu\n\n", proglist_len(proglist1));

    proglist_clear(proglist1);
    proglist_dump(proglist1);
    printf("length: %lu\n\n", proglist_len(proglist1));

    return 0;
}
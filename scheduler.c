#include "scheduler.h"
#include "program_linkedlist.h"

#include <bits/types/cookie_io_functions_t.h>
#include <stdio.h>

struct Program_list* ready_queue;
struct Program_list* wait_queue;
struct Program_list* creating;

// cria pseudoprogramas com base no exemplo dos slides de escalonamento
// https://moodle.pucrs.br/pluginfile.php/4882481/mod_resource/content/4/escalonamento.pdf
// pag 24

void scheduler_setup()
{
    ready_queue = proglist_create();
    wait_queue = proglist_create();
    creating = proglist_create();
}

void scheduler_auto(struct Program programs[], size_t program_count)
{    
    for (size_t i = 0; i < program_count; i++)
    {
        proglist_add_node(creating, &programs[i]);
    }
}

void test()
{
    run_program(creating->head->next->program);
}

void scheduler_execute_programs()
{    
    size_t counter = 0; // Time unit counter

    // Dummy program
    struct Program dummy;
    dummy.deadline = 999;
    dummy.next_deadline = 999;
    dummy.processing_time = 999;
    dummy.time_remaining = 999;
    dummy.arrival_time = 999;
    
    struct Program *p = &dummy;
    
    while (ready_queue->size > 0 || wait_queue->size > 0 || creating->size > 0 )
    {
        struct Node *it;

        // Checks for uninitialized programs
        if (creating->size > 0)
        {
            it = creating->head;
            while (it != NULL)
            {
                struct Program *i = it->program;
                if (i->arrival_time == counter)
                {
                    printf("PID %lu added to ready_queue at position %lu.\n", i->id, counter);
                    proglist_add_node(ready_queue, i);
                    proglist_remove_node_index(creating, 0);
                }
                it = creating->head;
            }
        }
        
        // CPU on idle
        if (ready_queue->size <= 0)
        {
            printf("idle on time %lu.\n", counter);
            if (wait_queue->size > 0)
            {
                it = wait_queue->head;
                while (it != NULL)
                {
                    struct Program *i = it->program;
                    if (counter >= i->next_deadline)
                    {
                        i->next_deadline = counter + i->deadline;
                        i->time_remaining = i->processing_time;
                        proglist_add_node(ready_queue, i);
                        proglist_remove_node_index(wait_queue, 0);
                    }
                    it = it->next;
                }
            }
            counter++;
            continue;
        }

        // Selects program with the closest deadline on ready_queue
        it = ready_queue->head;
        while (it != NULL)
        {
            struct Program *temp = it->program;
            if (temp->next_deadline < p->next_deadline)
            {
                printf("Exchanging to PID %lu on time %lu\n", temp->id, counter);
                p = temp;
            }
            it = it->next;
        }

        printf("PID %lu: running\n", p->id);

        // Runs the program until every syscall (WIP) (PRECISO ARRUMAR AQUI, PROGRAMA NÃO RODA INSTRUĆÕES)
        p->b_running = 1;
        while (p->b_running == 1)
        {
            if (p == &dummy)
            {
                printf("On idle\n");
                counter++;
                continue;
            }
            run_program(p);
        }
        // Adds the processing_time until the interrupt of the program to the timer counter
        // (WIP) ACHO QUE AQUI É UM PROBLEMA
        p->time_remaining -= p->processing_time;
        counter += p->processing_time;

        // Program finished
        if (p->b_finished)
        {
            proglist_remove_node(ready_queue, p->id);
            p = &dummy;
        }

        // Deadline lost
        if (p->next_deadline == counter && p->time_remaining > 0)
        {
            printf("PID %lu deadline lost on time %lu.\n", p->id, counter);
            p->next_deadline = counter + p->deadline;
            p->time_remaining = p->processing_time;
        }

        // Deadline met
        if (p->time_remaining <= 0)
        {
            printf("PID %lu deadline met on time %lu.\n", p->id, counter);
            proglist_remove_node(ready_queue, p->id);
            proglist_add_node(wait_queue, p);

            // troca para outro programa
            if (ready_queue->size > 0)
            {
                p = ready_queue->head->program;
                printf("Exchanging to PID %lu on time %lu\n", p->id, counter);
            }
            else
            {
                p = &dummy;
            }
        }

        // Checks for wait_queue programs that should get back in the ready_queue
        if (wait_queue->size > 0)
        {
            it = wait_queue->head;
            while (it != NULL)
            {
                struct Program *i = it->program;
                if (counter >= i->next_deadline)
                {
                    i->next_deadline = counter + i->deadline;
                    i->time_remaining = i->processing_time;
                    proglist_add_node(ready_queue, i);
                    proglist_remove_node_index(wait_queue, 0);
                }
            }
        }
    }
}

// Prints existing programs on ready_queue
void scheduler_print_ready_queue()
{
    printf("Programs on ready queue list:");
    proglist_dump(ready_queue);
}

// Prints existing programs on wait_queue
void scheduler_print_wait_queue()
{
    printf("Programs on wait queue list:\n");
    proglist_dump(wait_queue);
}

// Prints existing programs on creating
void scheduler_print_creating()
{
    printf("Programas on creating list:\n");
    proglist_dump(creating);
}

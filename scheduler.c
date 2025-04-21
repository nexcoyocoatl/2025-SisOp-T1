#include "scheduler.h"
#include "program.h"
#include "program_linkedlist.h"
#include <stdio.h>

struct Program_list* ready_queue;
struct Program_list* wait_queue;
struct Program_list* arrival_queue;

void scheduler_setup(struct Program programs[], size_t program_count)
{
    ready_queue = proglist_create();
    wait_queue = proglist_create();
    arrival_queue = proglist_create();

    for (size_t i = 0; i < program_count; i++)
    {
        proglist_add_node(arrival_queue, &programs[i]);
    }
}

void scheduler_execute_programs()
{
    // Flags to print messages
    int b_end_line = 0;
    size_t counter = 0; // Time unit counter
    
    struct Program *p = NULL;
    struct Program *last_p = NULL;
    
    // While there are still programs in any queue
    while (ready_queue->size > 0 || wait_queue->size > 0 || arrival_queue->size > 0 )
    {
        struct Node *it;
        
        // Checks for uninitialized programs that will arrive
        if (arrival_queue->size > 0)
        {
            struct Program *p_it = proglist_get_program(arrival_queue, 0);
            size_t index = 0;
            struct Program *i;
            while (p_it != NULL)
            {
                i = p_it;
                if (counter >= i->arrival_time)
                {
                    printf("PID %lu added to ready_queue from arrival_queue at time %lu. arrival = %lu, t = %lu, deadline = %lu\n",
                                i->id, counter, i->arrival_time, i->processing_time, i->deadline);
                    i = p_it;
                    i->next_deadline = counter + i->deadline;
                    i->processing_time_remaining = (int)i->processing_time;
                    proglist_add_node(ready_queue, i);
                    proglist_remove_node_index(arrival_queue, index);
                    index--;
                }
                else
                {
                    printf("Skipping arrival_queue PID %lu: time %lu, arrival %lu\n", i->id, counter, i->arrival_time);
                }
                index++;
                p_it = proglist_get_program(arrival_queue, index);
            }
            printf("--------------------------------------\n");
        }
        
        // CPU on idle
        if (ready_queue->size <= 0)
        {
            printf("Idle on time %lu.\n", counter);

            // Checks for wait_queue programs that should get back in the ready_queue
            if (wait_queue->size > 0)
            {
                size_t index = 0;
                it = wait_queue->head;
                struct Program *i = it->program;
                while (it != NULL)
                {
                    if (counter >= i->next_deadline) {
                        i = it->program;
                        i->next_deadline = counter + i->deadline;
                        i->processing_time_remaining = (int)i->processing_time;
                        proglist_add_node(ready_queue, i);
                        proglist_remove_node_index(wait_queue, index);
                        index--;
                    }
                    index++;
                    it = proglist_get_node(wait_queue, index);
                }
            }
            counter++;
            continue;
        }

        // Selects program with the closest deadline on ready_queue
        {
            // Adds the first on ready_queue if there is no program running
            it = ready_queue->head;
            if (p == NULL)
            {
                p = it->program;
                it = it->next;
            }
            // Checks if there's one with an earlier deadline to be run first
            while (it != NULL)
            {
                struct Program *temp = it->program;
                if (temp->next_deadline < p->next_deadline)
                {
                    p = temp;
                }
                it = it->next;
            }

            // If it's not the same, there was a swap, so print the one that was swapped to
            if (p != last_p)
            {
                printf("Swapping to PID %lu on time %lu\n", p->id, counter);
            }
            else
            {
                printf("Resuming PID %lu on time %lu\n", p->id, counter);
            }
        }

        printf("PID %lu: running\n", p->id);
        
        // Runs the program until time remaining <= 0 or if there's an interruption
        if (p != NULL)
        {   
            p->b_running = 1;
            while (p->processing_time_remaining > 0)
            {
                run_one_instruction(p);
                if (p->b_running == 0 || p->b_finished)
                {
                    b_end_line = 1;
                }
                p->processing_time_remaining--;
                p->deadline_time_remaining--;
                counter++;
            }
            
            if (p->b_finished) // Program has finished and is then removed from all queues
            {
                proglist_remove_node(ready_queue, p->id);
                p = NULL;
                last_p = NULL;
            }
            else
            {
                last_p = p;
            }
        }

        // Deadline lost
        if (p != NULL && p->deadline_time_remaining > 0)
        {
            b_end_line = 1;
            printf("PID %lu: deadline missed on time %lu.\n", p->id, counter);
            p->next_deadline = counter + p->deadline;
            p->processing_time_remaining = (int)p->processing_time;
        }

        // Deadline met
        if (p != NULL && p->deadline_time_remaining <= 0)
        {
            b_end_line = 1;
            printf("PID %lu: deadline met on time %lu.\n", p->id, counter);
            proglist_remove_node(ready_queue, p->id);
            proglist_add_node(wait_queue, p);

            p = NULL;                
        }

        // Prints a line when program ends or is interrupted
        if (b_end_line)
        {
            printf("--------------------------------------\n");
        }
    }
    proglist_clear(ready_queue);
    proglist_clear(wait_queue);
    proglist_clear(arrival_queue);
}

// Test functions
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

// Prints existing programs on arrival_queue
void scheduler_print_arrival_queue()
{
    printf("Programs on arrival_queue list:\n");
    proglist_dump(arrival_queue);
}

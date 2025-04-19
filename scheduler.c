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

void scheduler_execute_programs()
{    
    size_t counter = 0; // contador de unidades de tempo

    // programa dummy para quando processador estiver em idle
    struct Program dummy;
    dummy.id = 999;
    dummy.instruction_count = 999;
    dummy.deadline = 999;
    dummy.processing_time = 999;
    dummy.time_remaining = 999;
    dummy.arrival_time = 999;
    
    struct Program *p = &dummy;
    
    while (ready_queue->size > 0 || wait_queue->size > 0 || creating->size > 0 )
    {
        struct Node *it;

        // verifica por processos que nao foram inicializados ainda
        if (creating->size > 0)
        {
            it = creating->head;
            while (it != NULL)
            {
                struct Program *i = it->program;
                if (i->arrival_time == counter)
                {
                    printf("Adicionando programa %lu a readyQueue em %lu\n", i->id, counter);
                    proglist_add_node(ready_queue, i);
                    proglist_remove_node_index(creating, 0);
                }
                it = creating->head;
            }
        }

        // processador em idle
        if (ready_queue->size <= 0)
        {
            printf("idle em %lu\n", counter);
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

        // seleciona programa com deadline mais proxima em readyQueue
        it = ready_queue->head;
        while (it != NULL)
        {
            struct Program *temp = it->program;
            if (temp->next_deadline < p->next_deadline)
            {
                printf("Trocando para programa %lu em %lu\n", temp->id, counter);
                p = temp;
            }
            it = it->next;
        }

        // Runs the program until every syscall (WIP) (PRECISO ARRUMAR AQUI, PROGRAMA NÃO RODA INSTRUĆÕES)
        p->b_running = 1;
        while (p->b_running == 1)
        {
            run_program(p);
        }
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
            printf("Deadline de programa %lu perdida em %lu\n", p->id, counter);
            p->next_deadline = counter + p->deadline;
            p->time_remaining = p->processing_time;
        }

        // Deadline satisfeita
        if (p->time_remaining <= 0)
        {
            printf("Deadline de programa %lu satisfeita em %lu\n", p->id, counter);
            proglist_remove_node(ready_queue, p->id);
            proglist_add_node(wait_queue, p);

            // troca para outro programa
            if (ready_queue->size > 0)
            {
                p = ready_queue->head->program;
                printf("Trocando para programa %lu em %lu\n", p->id, counter);
            }
            else
            {
                p = &dummy;
            }
        }

        // verifica por programas da wait_queue que devem voltar para ready_queue
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

// lista programas existentes na ready_queue
void scheduler_print_ready_queue()
{
    printf("Programas na lista de prontos para execução:");
    proglist_dump(ready_queue);
}

// lista programas existentes na wait_queue
void scheduler_print_wait_queue()
{
    printf("Programas na lista de espera:\n");
    proglist_dump(wait_queue);
}

// lista programas existentes na creating
void scheduler_print_creating()
{
    printf("Programas na creating:\n");
    proglist_dump(creating);
}

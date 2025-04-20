#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "program.h"

void scheduler_setup();

void scheduler_auto(struct Program programs[], size_t program_count);

void scheduler_execute_programs();

void scheduler_print_ready_queue();

void scheduler_print_wait_queue();

void scheduler_print_creating();

void test();

#endif
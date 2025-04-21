#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "program.h"

// Creates lists and sets up programs inside arrival_queue
void scheduler_setup(struct Program programs[], size_t program_count);

void scheduler_execute_programs();

// Test functions
// Prints existing programs on ready_queue
void scheduler_print_ready_queue();

// Prints existing programs on wait_queue
void scheduler_print_wait_queue();

// Prints existing programs on arrival_queue
void scheduler_print_arrival_queue();

void test();

#endif
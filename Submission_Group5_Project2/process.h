#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "utility.h"

struct process_s
{
	unsigned char pid;
	float arrival_time;
	float run_time;
	unsigned char priority;
};

typedef struct process_s Process;

Process *create_process(char pid, float arrival_time, float run_time, unsigned char priority);

Process *get_copy_of_process(Process *proc);

Linked_List *generate_processes(int n, int seed);

#endif
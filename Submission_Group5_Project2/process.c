#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "process.h"
#include "utility.h"

int compare(void *data1, void *data2)
{
	Process *p1 = (Process *)data1;
	Process *p2 = (Process *)data2;
	if (p1->arrival_time < p2->arrival_time)
		return -1;
	if (p1->arrival_time > p2->arrival_time)
		return 1;
	else
		return 0;
}

Linked_List *generate_processes(int n, int seed)
{
	Linked_List *process_list = create_Linked_List();
	char pid = 'A';
	unsigned char priority;
	float arrival_time, run_time;
	time_t t;

	srand((unsigned)seed);

	if (process_list == NULL)
		fprintf(stderr, "Unable to create Linked List\n");

	while (n--)
	{
		arrival_time = rand() % 100;
		run_time = (float)((rand() % 100) + 1) / 10;
		priority = (rand() % 4) + 1;
		Process *p = create_process(pid, arrival_time, run_time, priority);

		add_Node(process_list, p);
		pid++;
	}

	sort(process_list, compare);

	Node *ptr = process_list->head;
	pid = 'A';
	while (ptr != NULL)
	{
		((Process *)ptr->data)->pid = pid;
		if (pid == 'Z')
			pid = 'a' - 1;
		pid++;
		ptr = ptr->next;
	}

	return process_list;
}

Process *create_process(char pid, float arrival_time, float run_time, unsigned char priority)
{
	Process *proc = (Process *)malloc(sizeof(Process));
	proc->pid = pid;
	proc->arrival_time = arrival_time;
	proc->run_time = run_time;
	proc->priority = priority;
	return proc;
}

Process *get_copy_of_process(Process *proc) {}

#include <stdio.h>
#include <stdlib.h>
#include "stat.h"
#include "utility.h"

Process_Statistics *create_Process_Statistics(Process *proc)
{
	Process_Statistics *processStat = (Process_Statistics *)malloc(sizeof(Process_Statistics));
	processStat->proc = proc;
	processStat->wait_time = processStat->turnaround_time = processStat->response_time = processStat->run_time = 0;
	processStat->start_time = processStat->end_time = -1;
	return processStat;
}

Average_Statistics first_come_first_serve_nonpreemptive(Linked_List *processes)
{
	int t = 0;
	Queue *process_Queue = (Queue *)create_Queue();
	Node *processTrav = processes->head;
	if (processes->head == NULL)
		fprintf(stderr, "No Process to schedule\n");
	Process_Statistics *scheduled_process = NULL;

	Linked_List *ll = create_Linked_List();
	printf("\nFirst Come First Serve Algorithm:\n");

	while (t < 200 || scheduled_process != NULL)
	{
		if (processTrav != NULL)
		{
			Process *new_process = (Process *)(processTrav->data);
			if (new_process->arrival_time <= t)
			{
				enQueue(process_Queue, create_Process_Statistics(new_process));
				processTrav = processTrav->next;
			}
		}

		if (scheduled_process == NULL && process_Queue->size > 0)
			scheduled_process = (Process_Statistics *)deQueue(process_Queue);

		if (scheduled_process != NULL)
		{
			Process *proc = scheduled_process->proc;
			printf("%c", proc->pid);

			if (scheduled_process->start_time == -1)
				scheduled_process->start_time = t;

			scheduled_process->run_time++;

			if (scheduled_process->run_time >= proc->run_time)
			{
				scheduled_process->end_time = t;
				add_Node(ll, scheduled_process);
				scheduled_process = NULL;
			}
		}
		else
			printf("_");
		t++;
	}
	printf("\n");
	return print_policy_stat(ll);
}

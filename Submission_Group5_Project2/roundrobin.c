#include <stdio.h>
#include <stdlib.h>
#include "stat.h"
#include "utility.h"

Process_Statistics *create_Process_Statistics(Process *proc);

Average_Statistics round_robin_preemptive(Linked_List *processes, int time_slice)
{
	int t = 0;

	// Create Process Queue
	Queue *process_Queue = (Queue *)create_Queue();
	Node *processTrav = processes->head;
	if (processes->head == NULL)
		fprintf(stderr, "No Process to schedule\n");
	Process_Statistics *scheduled_process = NULL;

	Linked_List *linkedList = create_Linked_List();
	printf("\nRound Robbin Algorithm:\n");
	Node *cur_Node = NULL;
	int cur_run = 0;
	while (t < 100 || process_Queue->size > 0)
	{
		if (processTrav != NULL && t < 100)
		{
			Process *new_process = (Process *)(processTrav->data);
			while (processTrav != NULL && new_process->arrival_time <= t)
			{
				enQueue(process_Queue, create_Process_Statistics(new_process));
				processTrav = processTrav->next;
				if (processTrav != NULL)
					new_process = (Process *)(processTrav->data);
			}
		}

		if (cur_Node == NULL)
		{
			cur_run = 0;
			cur_Node = process_Queue->head;
		}
		else if (cur_run == time_slice)
		{
			cur_run = 0;
			cur_Node = cur_Node->next;
			if (cur_Node == NULL)
				cur_Node = process_Queue->head;
		}

		if (cur_Node != NULL)
		{
			scheduled_process = (Process_Statistics *)cur_Node->data;
			Process *proc = scheduled_process->proc;

			if (t >= 100)
			{
				if (scheduled_process->start_time == -1)
				{
					free(scheduled_process);
					Node *next = cur_Node->next;
					remove_Node(process_Queue, cur_Node->data);
					cur_Node = next;
					cur_run = 0;
					continue;
				}
			}
			printf("%c", proc->pid);
			cur_run++;
			if (scheduled_process->start_time == -1)
				scheduled_process->start_time = t;
			scheduled_process->run_time++;

			if (scheduled_process->run_time >= proc->run_time)
			{
				scheduled_process->end_time = t;
				add_Node(linkedList, scheduled_process);
				Node *next = cur_Node->next;
				remove_Node(process_Queue, cur_Node->data);
				cur_Node = next;
				cur_run = 0;
			}
		}
		else
			printf("_");
		t++;
	}

	printf("\nt : %d", t);

	return print_policy_stat(linkedList);
}

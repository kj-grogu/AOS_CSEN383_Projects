#include <stdio.h>
#include <stdlib.h>
#include "stat.h"
#include "utility.h"

Process_Statistics *create_Process_Statistics(Process *proc);

int compareRunTime(void *data1, void *data2)
{
	Process_Statistics *ps1 = (Process_Statistics *)data1;
	Process_Statistics *ps2 = (Process_Statistics *)data2;
	if (((Process *)ps1->proc)->run_time < ((Process *)ps2->proc)->run_time)
		return -1;
	if (((Process *)ps1->proc)->run_time > ((Process *)ps2->proc)->run_time)
		return 1;
	else
		return 0;
}

Average_Statistics shortest_job_first_nonpreemptive(Linked_List *processes)
{
	int t = 0;
	Queue *process_Queue = (Queue *)create_Queue();
	Node *proc_ptr = processes->head;
	if (processes->head == NULL)
		printf("No Process to schedule\n");
	Process_Statistics *scheduled_process = NULL;

	Linked_List *ll = create_Linked_List();
	printf("\nShortest Job First Algorithm:\n");
	while (t < 200 || scheduled_process != NULL)
	{
		if (proc_ptr != NULL)
		{
			Process *new_process = (Process *)(proc_ptr->data);
			while (proc_ptr != NULL && new_process->arrival_time <= t)
			{
				enQueue(process_Queue, create_Process_Statistics(new_process));
				sort(process_Queue, compareRunTime);
				proc_ptr = proc_ptr->next;
				if (proc_ptr != NULL)
					new_process = (Process *)(proc_ptr->data);
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

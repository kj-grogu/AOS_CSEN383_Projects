#include <stdio.h>
#include <stdlib.h>
#include "utility.h"
#include "process.h"
#include "stat.h"

Linked_List *create_Linked_List()
{
	Linked_List *new_ll = (Linked_List *)malloc(sizeof(Linked_List));
	new_ll->head = NULL;
	new_ll->tail = NULL;
	new_ll->size = 0;
	return new_ll;
}

Node *create_Node(void *data)
{
	Node *new_Node = (Node *)malloc(sizeof(Node));
	new_Node->data = data;
	new_Node->next = NULL;
	new_Node->prev = NULL;
	return new_Node;
}

void add_Node(Linked_List *ll, void *data)
{
	Node *new_Node = create_Node(data);
	if (ll->size == 0)
	{
		ll->head = new_Node;
		ll->tail = new_Node;
		ll->size = 1;
	}
	else
	{
		new_Node->prev = ll->tail;
		ll->tail->next = new_Node;
		ll->tail = new_Node;
		ll->size += 1;
	}
}

void remove_Node(Linked_List *ll, void *data)
{
	Node *current_Node = ll->head;

	while (current_Node != NULL && current_Node->data != data)
		current_Node = current_Node->next;

	if (current_Node != NULL)
	{
		if (current_Node->prev != NULL)
			current_Node->prev->next = current_Node->next;
		if (current_Node->next != NULL)
			current_Node->next->prev = current_Node->prev;
		if (ll->head == current_Node)
			ll->head = current_Node->next;
		if (ll->tail == current_Node)
			ll->tail = current_Node->prev;
		ll->size--;
		free(current_Node);
	}
}

void remove_head(Linked_List *ll)
{
	Node *current_Node = ll->head;
	if (current_Node != NULL)
	{
		ll->head = current_Node->next;
		if (ll->tail == current_Node)
			ll->tail = current_Node->prev;
		ll->size--;
		free(current_Node);
	}
}

void add_after(Linked_List *ll, Node *after_Node, void *data)
{
	Node *new_Node = create_Node(data);
	Node *next_Node = after_Node->next;
	new_Node->next = next_Node;
	if (next_Node != NULL)
		next_Node->prev = new_Node;

	new_Node->prev = after_Node;
	after_Node->next = new_Node;

	if (ll->tail == after_Node)
		ll->tail = new_Node;

	ll->size++;
}

void sort(Linked_List *ll, int (*cmp)(void *data1, void *data2))
{
	Node *i = ll->head;
	while (i != NULL)
	{
		Node *j = i->next;
		while (j != NULL)
		{
			void *p1 = i->data;
			void *p2 = j->data;
			if ((*cmp)(p1, p2) > 0)
				swap_Nodes(i, j);
			j = j->next;
		}
		i = i->next;
	}
}

void swap_Nodes(Node *a, Node *b)
{
	void *temp = a->data;
	a->data = b->data;
	b->data = temp;
}

Queue *create_Queue()
{
	return create_Linked_List();
}

void enQueue(Queue *q, void *data)
{
	Node *new_Node = create_Node(data);

	new_Node->prev = q->tail;
	if (q->tail != NULL)
	{
		q->tail->next = new_Node;
		q->tail = new_Node;
	}
	else
	{
		q->tail = new_Node;
		q->head = new_Node;
	}
	q->size += 1;
}

void *deQueue(Queue *q)
{
	if (q->head != NULL)
	{
		Node *current_Node = q->head;
		void *data = current_Node->data;

		Node *next_Node = q->head->next;

		if (next_Node != NULL)
			next_Node->prev = NULL;
		q->head = next_Node;

		if (q->tail == current_Node)
			q->tail = NULL;

		q->size--;
		free(current_Node);
		return data;
	}
}

int isEmpty(Queue *q)
{
	return q->size == 0;
}

Average_Statistics print_policy_stat(Linked_List *ll)
{
	Average_Statistics avg;
	printf("\n");
	printf("-----------------------------------------------------------------------------------------------------------------------------\n");
	printf("Process Name\t| Arrival Time | Start Time | End Time | Run Time | Response Time | Wait Time | Turn Around Time | Priority |\n");
	printf("-----------------------------------------------------------------------------------------------------------------------------\n");
	Node *ptr = ll->head;
	float avg_response_time = 0;
	float avg_wait_time = 0;
	float avg_turnaround = 0;
	int process_count = 0;
	while (ptr != NULL)
	{
		Process_Statistics *stat = (Process_Statistics *)ptr->data;
		if (stat == NULL)
			printf("No Stat\n");
		Process *proc = (Process *)stat->proc;
		if (proc == NULL)
			printf("No Process\n");
		float arrival_time = proc->arrival_time;
		float run_time = proc->run_time;
		float response_time = stat->start_time - arrival_time;
		float turnaround = stat->end_time - proc->arrival_time + 1;
		float wait_time = turnaround - run_time;
		unsigned char priority = proc->priority;
		avg_response_time += response_time;
		avg_wait_time += wait_time;
		avg_turnaround += turnaround;
		process_count++;
		printf("%16c|%14.1f|%12.1f|%10.1f|%10.1f|%15.1f|%11.1f| %17.1f|%10u|\n", proc->pid, arrival_time, stat->start_time, stat->end_time, run_time, response_time, wait_time, turnaround, priority);
		ptr = ptr->next;
	}
	avg.avg_throughput = process_count;
	if (process_count == 0)
		process_count = 1;
	avg_response_time = avg_response_time / process_count;
	avg_wait_time = avg_wait_time / process_count;
	avg_turnaround = avg_turnaround / process_count;
	printf("-----------------------------------------------------------------------------------------------------------------------------\n");
	printf("%16s|%14.1f|%12.1f|%10.1f|%10.1f|%15.1f|%11.1f| %17.1f|\n", " Average", 0.0, 0.0, 0.0, 0.0, avg_response_time, avg_wait_time, avg_turnaround);
	printf("-----------------------------------------------------------------------------------------------------------------------------\n");
	avg.avg_response_time = avg_response_time;
	avg.avg_wait_time = avg_wait_time;
	avg.avg_turnaround = avg_turnaround;

	return avg;
}

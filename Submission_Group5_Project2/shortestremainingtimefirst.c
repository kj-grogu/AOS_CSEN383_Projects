#include <stdio.h>
#include <stdlib.h>
#include "stat.h"
#include "utility.h"

Process_Statistics *create_Process_Statistics(Process *proc);

int compare_remaining_time(void *data1, void *data2)
{
  Process_Statistics *ps1 = (Process_Statistics *)data1;
  Process_Statistics *ps2 = (Process_Statistics *)data2;
  if (((((Process *)ps1->proc)->run_time) - (ps1->run_time)) < ((((Process *)ps2->proc)->run_time) - (ps2->run_time)))
    return -1;
  if (((((Process *)ps1->proc)->run_time) - (ps1->run_time)) > ((((Process *)ps2->proc)->run_time) - (ps2->run_time)))
    return 1;
  else
    return 0;
}

Average_Statistics shortest_remaining_time_preemptive(Linked_List *processes)
{
  int t = 0;
  Queue *process_Queue = create_Queue();
  Linked_List *ll = create_Linked_List();

  Node *process_pointer = processes->head;
  if (processes->head == NULL)
    printf("No Process to schedule\n");

  Process_Statistics *scheduled_process = NULL;
  printf("\n\nShortest Remainging Time To Completion Preemptive:\n");
  while (t < 200 || scheduled_process != NULL)
  {
    if (scheduled_process != NULL)
    {
      enQueue(process_Queue, scheduled_process);
      scheduled_process = NULL;
    }
    if (process_pointer != NULL)
    {
      Process *new_process = (Process *)(process_pointer->data);
      while (process_pointer != NULL && new_process->arrival_time <= t)
      {
        enQueue(process_Queue, create_Process_Statistics(new_process));
        process_pointer = process_pointer->next;
        if (process_pointer != NULL)
          new_process = (Process *)(process_pointer->data);
      }
      sort(process_Queue, compare_remaining_time);
    }

    if (scheduled_process == NULL && process_Queue->size > 0)
    {
      scheduled_process = (Process_Statistics *)deQueue(process_Queue);
      while (t >= 200 && scheduled_process->start_time == -1)
        scheduled_process = (Process_Statistics *)deQueue(process_Queue);
    }
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
  return print_policy_stat(ll);
}

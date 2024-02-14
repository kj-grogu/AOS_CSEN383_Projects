#ifndef _stat_h_
#define _stat_h_
#include "process.h"

struct process_stats
{
	Process *proc;
	float wait_time;
	float turnaround_time;
	float response_time;

	float start_time;
	float end_time;
	float run_time;
};
typedef struct process_stats Process_Statistics;

struct Average_Statistics_s
{
	float avg_response_time;
	float avg_wait_time;
	float avg_turnaround;
	float avg_throughput;
};

typedef struct Average_Statistics_s Average_Statistics;

Average_Statistics first_come_first_serve_nonpreemptive(Linked_List *processes);
Average_Statistics shortest_job_first_nonpreemptive(Linked_List *processes);
Average_Statistics shortest_remaining_time_preemptive(Linked_List *processes);
Average_Statistics round_robin_preemptive(Linked_List *processes, int time_slice);
Average_Statistics highest_priority_first_preemptive(Linked_List *processes);
Average_Statistics highest_priority_first_nonpreemptive(Linked_List *processes);
Average_Statistics highest_priority_first_preemptive_aging(Linked_List *processes);
Average_Statistics highest_priority_first_nonpreemptive_aging(Linked_List *processes);

Average_Statistics print_policy_stat(Linked_List *ll);
#endif

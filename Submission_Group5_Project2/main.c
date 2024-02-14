#include <stdio.h>
#include "utility.h"
#include "process.h"
#include "stat.h"

int main(int argc, char **argv)
{
	int i = 0;
	int simulationIterations = 5;
	int algorithms = 6;
	int timeSlice = 1;
	Average_Statistics fcfs_stats[6], sjf_stats[6], srf_stats[6], roundrobin_stats[6], hpfp_stats[6], hpfnp_stats[6], final_results[6];

	while (i < simulationIterations)
	{
		printf("\n*********************************************************************************\n");
		printf("\t\t\t\tIteration %d\n", i + 1);
		printf("*********************************************************************************\n");

		Linked_List *processList = generate_processes(26, i);
		Node *ptr = processList->head;
		printf("-------------------------------------------------\n");
		printf("Process ID | Arrival Time | Run Time | Priority |\n");
		printf("-------------------------------------------------\n");
		while (ptr != NULL)
		{
			Process *proc = (Process *)ptr->data;
			printf("%10c | %12.1f | %8.1f | %8d |\n", proc->pid, proc->arrival_time, proc->run_time, proc->priority);
			ptr = ptr->next;
		}
		printf("--------------------------------------------------\n");
		printf("Total No. of Processes : %d\n", processList->size);
		printf("--------------------------------------------------\n");

		fcfs_stats[i] = first_come_first_serve_nonpreemptive(processList);
		roundrobin_stats[i] = round_robin_preemptive(processList, timeSlice);
		sjf_stats[i] = shortest_job_first_nonpreemptive(processList);
		srf_stats[i] = shortest_remaining_time_preemptive(processList);
		hpfp_stats[i] = highest_priority_first_preemptive(processList);
		hpfnp_stats[i] = highest_priority_first_nonpreemptive(processList);
		i++;
	}

	for (int i = 0; i < algorithms; i++)
	{
		final_results[i].avg_response_time = 0;
		final_results[i].avg_wait_time = 0;
		final_results[i].avg_turnaround = 0;
		final_results[i].avg_throughput = 0;
	}

	for (int i = 0; i < simulationIterations; i++)
	{
		final_results[0].avg_response_time += fcfs_stats[i].avg_response_time;
		final_results[1].avg_response_time += sjf_stats[i].avg_response_time;
		final_results[2].avg_response_time += srf_stats[i].avg_response_time;
		final_results[3].avg_response_time += roundrobin_stats[i].avg_response_time;
		final_results[4].avg_response_time += hpfp_stats[i].avg_response_time;
		final_results[5].avg_response_time += hpfnp_stats[i].avg_response_time;

		final_results[0].avg_wait_time += fcfs_stats[i].avg_wait_time;
		final_results[1].avg_wait_time += sjf_stats[i].avg_wait_time;
		final_results[2].avg_wait_time += srf_stats[i].avg_wait_time;
		final_results[3].avg_wait_time += roundrobin_stats[i].avg_wait_time;
		final_results[4].avg_wait_time += hpfp_stats[i].avg_wait_time;
		final_results[5].avg_wait_time += hpfnp_stats[i].avg_wait_time;

		final_results[0].avg_turnaround += fcfs_stats[i].avg_turnaround;
		final_results[1].avg_turnaround += sjf_stats[i].avg_turnaround;
		final_results[2].avg_turnaround += srf_stats[i].avg_turnaround;
		final_results[3].avg_turnaround += roundrobin_stats[i].avg_turnaround;
		final_results[4].avg_turnaround += hpfp_stats[i].avg_turnaround;
		final_results[5].avg_turnaround += hpfnp_stats[i].avg_turnaround;

		final_results[0].avg_throughput += fcfs_stats[i].avg_throughput;
		final_results[1].avg_throughput += sjf_stats[i].avg_throughput;
		final_results[2].avg_throughput += srf_stats[i].avg_throughput;
		final_results[3].avg_throughput += roundrobin_stats[i].avg_throughput;
		final_results[4].avg_throughput += hpfp_stats[i].avg_throughput;
		final_results[5].avg_throughput += hpfnp_stats[i].avg_throughput;
	}

	for (int i = 0; i < algorithms; i++)
	{
		final_results[i].avg_response_time /= simulationIterations;
		final_results[i].avg_wait_time /= simulationIterations;
		final_results[i].avg_turnaround /= simulationIterations;
		final_results[i].avg_throughput /= simulationIterations;
	}
	printf("\n\n\n");
	printf("_____________________________________________________________________________________________________________________________\n\n");
	printf("The average of the %d runs of all algorithms is as follows:\n", simulationIterations);
	printf("\n");
	printf("ALGORITHM: FIRST COME FIRST SERVE:\n\n");
	printf("Average Response Time(RT) : %.1f\n", final_results[0].avg_response_time);
	printf("Average Wait Time(WT) : %.1f\n", final_results[0].avg_wait_time);
	printf("Average Turn Around Time(TAT) :%.1f\n", final_results[0].avg_turnaround);
	printf("Average Throughput(Tr) :%.1f\n", final_results[0].avg_throughput);
	printf("\n");
	printf("ALGORITHM: ROUND ROBIN PREEMPTIVE:\n\n");
	printf("Average Response Time(RT) : %.1f\n", final_results[3].avg_response_time);
	printf("Average Wait Time(WT) : %.1f\n", final_results[3].avg_wait_time);
	printf("Average Turn Around Time(TAT) :%.1f\n", final_results[3].avg_turnaround);
	printf("Average Throughput(Tr) :%.1f\n", final_results[3].avg_throughput);
	printf("\n");
	printf("ALGORITHM: SHORTEST JOB FIRST NON PREEMPTIVE:\n\n");
	printf("Average Response Time(RT) : %.1f\n", final_results[1].avg_response_time);
	printf("Average Wait Time(WT) : %.1f\n", final_results[1].avg_wait_time);
	printf("Average Turn Around Time(TAT) :%.1f\n", final_results[1].avg_turnaround);
	printf("Average Throughput(Tr) :%.1f\n", final_results[1].avg_throughput);
	printf("\n");
	printf("ALGORITHM: SHORTEST REMAINING TIME FIRST PREEMPTIVE:\n\n");
	printf("Average Response Time(RT) : %.1f\n", final_results[2].avg_response_time);
	printf("Average Wait Time(WT) : %.1f\n", final_results[2].avg_wait_time);
	printf("Average Turn Around Time(TAT) :%.1f\n", final_results[2].avg_turnaround);
	printf("Average Throughput(Tr) :%.1f\n", final_results[2].avg_throughput);
	printf("\n");
	printf("ALGORITHM: HIGHEST PRIORITY FIRST PREEMPTIVE:\n\n");
	printf("Average Response Time(RT) : %.1f\n", final_results[4].avg_response_time);
	printf("Average Wait Time(WT) : %.1f\n", final_results[4].avg_wait_time);
	printf("Average Turn Around Time(TAT) :%.1f\n", final_results[4].avg_turnaround);
	printf("Average Throughput(Tr) :%.1f\n", final_results[4].avg_throughput);
	printf("\n");
	printf("ALGORITHM: HIGHEST PRIORITY FIRST NON PREEMPTIVE:\n\n");
	printf("Average Response Time(RT) : %.1f\n", final_results[5].avg_response_time);
	printf("Average Wait Time(WT) : %.1f\n", final_results[5].avg_wait_time);
	printf("Average Turn Around Time(TAT) :%.1f\n", final_results[5].avg_turnaround);
	printf("Average Throughput(Tr) :%.1f\n", final_results[5].avg_throughput);
	printf("\n");
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "utility.h"

#define concertRows 10
#define concertCols 10
#define dimension 5
#define totalDuration 60
#define highPriceTicketSeller 1
#define mediumPriceTicketSeller 3
#define lowPriceTicketSeller 6
#define totalSellers (highPriceTicketSeller + mediumPriceTicketSeller + lowPriceTicketSeller)

typedef struct ticketSellerStruct
{
	char id;
	char sellerType;
	Queue *sellerQue;

} ticketSeller;

typedef struct customer
{
	char id;
	int arrivalTime;
	int responseTime;
	int turnaroundTime;

} customer;

char seats[concertRows][concertCols][dimension];
int turnAroundTimeHighPriceTickets;
int turnAroundTimeLowPriceTickets;
int turnAroundTimMediumPriceTickets;
int responseTimeHighPriceTickets;
int responseTimeLowPriceTickets;
int responseTimeMediumPriceTickets;

int currentTime;
int customersPerTicket = 0;

int threadCount = 0;
int threadsAwaitingClock = 0;
int runningThread = 0;

// Function declarations
int compareOnArrivalTime(void *value1, void *value2);
Queue *createCustomers(int);
void wakeupAllSellers();
void displayQueue(const Queue *q);
void createSellerThread(pthread_t *thread, char sellerType, int no_of_sellers);
void waitForTimeSlice();
void *sellTicket(void *);
int getAvailableSeat(char sellerType);

pthread_t sellerThreads[totalSellers];
pthread_mutex_t reservationMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t threadCountMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t threadCompletionMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t conditionMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t threadsAwaitingClockMutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char **argv)
{
	if (argc == 2)
	{
		customersPerTicket = atoi(argv[1]);
	}

	createSellerThread(sellerThreads, 'H', highPriceTicketSeller);
	createSellerThread(sellerThreads + highPriceTicketSeller, 'M', mediumPriceTicketSeller);
	createSellerThread(sellerThreads + highPriceTicketSeller + mediumPriceTicketSeller, 'L', lowPriceTicketSeller);

	for (int r = 0; r < concertRows; r++)
	{
		for (int c = 0; c < concertCols; c++)
		{
			strncpy(seats[r][c], "-", 1);
		}
	}

	while (true)
	{
		pthread_mutex_lock(&threadCountMutex);
		if (threadCount == 0)
		{
			pthread_mutex_unlock(&threadCountMutex);
			break;
		}
		pthread_mutex_unlock(&threadCountMutex);
	}

	// Simulate each currentTime quanta/slice as one iteration
	printf("\nThread simulation starts here:");
	printf("\n---------------------------------------------------------------------------------------\n");
	printf("Time 	SellerName	Activity			Response Time 	Turnaround Time");
	printf("\n---------------------------------------------------------------------------------------\n");
	threadsAwaitingClock = 0;
	wakeupAllSellers(); // For first tick

	do
	{

		// Wake up all thread
		waitForTimeSlice();
		currentTime = currentTime + 1;
		wakeupAllSellers();
		// Wait for thread completion
	} while (currentTime < totalDuration);

	// Wakeup all thread so that no more thread keep waiting for clock Tick in limbo
	wakeupAllSellers();

	while (runningThread)
		;

	printf("\nThread simulation Ended\n\n");
	// Display concert chart
	printf("\n\n");
	printf("Concert Seats Sold");
	printf("\n****************************\n\n");

	int h_customers = 0, m_customers = 0, l_customers = 0;
	for (int r = 0; r < concertRows; r++)
	{
		for (int c = 0; c < concertCols; c++)
		{
			if (c != 0)
				printf("\t");
			printf("%5s", seats[r][c]);
			if (seats[r][c][0] == 'H')
				h_customers++;
			if (seats[r][c][0] == 'M')
				m_customers++;
			if (seats[r][c][0] == 'L')
				l_customers++;
		}
		printf("\n");
	}

	printf("\n\n*****************************************");
	printf("\nMulti-threaded Ticket Sellers");
	printf("\nInput customersPerTicket = %02d\n", customersPerTicket);
	printf("*****************************************\n\n");

	printf(" ---------------------------------------------------------\n");
	printf("|%3c | No of Customers | Seated  | Returned | Throughput|\n", ' ');
	printf(" ---------------------------------------------------------\n");
	printf("|%3c | %15d | %8d | %8d | %.2f      |\n", 'H', highPriceTicketSeller * customersPerTicket, h_customers, (highPriceTicketSeller * customersPerTicket) - h_customers, (h_customers / 60.0));
	printf("|%3c | %15d | %8d | %8d | %.2f      |\n", 'M', mediumPriceTicketSeller * customersPerTicket, m_customers, (mediumPriceTicketSeller * customersPerTicket) - m_customers, (m_customers / 60.0));
	printf("|%3c | %15d | %8d | %8d | %.2f      |\n", 'L', lowPriceTicketSeller * customersPerTicket, l_customers, (lowPriceTicketSeller * customersPerTicket) - l_customers, (l_customers / 60.0));
	printf(" --------------------------------------------------------\n");
	printf("\n");

	printf(" ----------------------------------------------------------------\n");
	printf("|%3c Average Turnaround Time |  Average Response Time |\n", ' ');
	printf(" ----------------------------------------------------------------\n");
	printf("| %3c  | %3f          | %.2f 		|\n", 'H', turnAroundTimeHighPriceTickets / (customersPerTicket * 1.0), responseTimeHighPriceTickets / (customersPerTicket * 1.0));
	printf("| %3c  | %3f          | %.2f 		|\n", 'L', turnAroundTimeLowPriceTickets / (6.0 * customersPerTicket), responseTimeLowPriceTickets / (6.0 * customersPerTicket));
	printf("| %3c  | %3f          | %.2f 		|\n", 'M', turnAroundTimMediumPriceTickets / (3.0 * customersPerTicket), responseTimeMediumPriceTickets / (3.0 * customersPerTicket));
	printf(" ----------------------------------------------------------------\n");
	return 0;
}

void createSellerThread(pthread_t *thread, char sellerType, int no_of_sellers)
{
	for (int i = 0; i < no_of_sellers; i++)
	{
		ticketSeller *seller_param = (ticketSeller *)malloc(sizeof(ticketSeller));
		seller_param->id = i;
		seller_param->sellerType = sellerType;
		seller_param->sellerQue = createCustomers(customersPerTicket);

		pthread_mutex_lock(&threadCountMutex);
		threadCount++;
		pthread_mutex_unlock(&threadCountMutex);
		printf("Creating Seller thread %c%02d\n", sellerType, i);
		pthread_create(thread + i, NULL, &sellTicket, seller_param);
	}
}

void displayQueue(const Queue *que)
{
	Node *ptr = que->head;

	while (ptr != NULL)
	{
		customer *cust = (customer *)ptr->val;
		printf("[%d, %d]", cust->id, cust->arrivalTime);

		ptr = ptr->next;

		if (ptr != NULL)
		{
			printf(" -> ");
		}
	}

	printf("\n");
}

void waitForTimeSlice()
{
	// Check if all threads has finished their jobs for this currentTime slice
	while (true)
	{
		pthread_mutex_lock(&threadsAwaitingClockMutex);
		if (threadsAwaitingClock == runningThread)
		{
			threadsAwaitingClock = 0;
			pthread_mutex_unlock(&threadsAwaitingClockMutex);
			break;
		}
		pthread_mutex_unlock(&threadsAwaitingClockMutex);
	}
}

Queue *createCustomers(int customers)
{
	Queue *customer_queue = createQueue();
	char id = 0;
	while (customers--)
	{
		customer *cust = (customer *)malloc(sizeof(customer));
		cust->arrivalTime = rand() % totalDuration;
		cust->id = id;
		addToQueue(customer_queue, cust);
		id++;
	}
	sortLinkedList(customer_queue, compareOnArrivalTime);
	Node *ptr = customer_queue->head;
	id = 0;
	while (ptr != NULL)
	{
		id++;
		customer *cust = (customer *)ptr->val;
		cust->id = id;
		ptr = ptr->next;
	}
	return customer_queue;
}

void wakeupAllSellers()
{
	pthread_mutex_lock(&conditionMutex);
	printf("00:%02d ====Clock Tick====\n", currentTime);
	pthread_cond_broadcast(&condition_cond);
	pthread_mutex_unlock(&conditionMutex);
}

void *sellTicket(void *t_args)
{
	// Initializing thread
	ticketSeller *args = (ticketSeller *)t_args;
	Queue *customer_queue = args->sellerQue;
	Queue *sellerQue = createQueue();
	char sellerType = args->sellerType;
	int id = args->id + 1;

	pthread_mutex_lock(&threadCountMutex);
	threadCount--;
	runningThread++;
	pthread_mutex_unlock(&threadCountMutex);

	customer *cust = NULL;
	int random_wait_time = 0;

	while (currentTime < totalDuration)
	{
		// Waiting for clock tick
		pthread_mutex_lock(&conditionMutex);
		printf("00:%02d %c%02d Waiting for next clock tick\n", currentTime, sellerType, id);

		pthread_mutex_lock(&threadsAwaitingClockMutex);
		threadsAwaitingClock++;
		pthread_mutex_unlock(&threadsAwaitingClockMutex);

		pthread_cond_wait(&condition_cond, &conditionMutex);
		printf("00:%02d %c%02d Received Clock Tick\n", currentTime, sellerType, id);
		pthread_mutex_unlock(&conditionMutex);

		// Sell
		if (currentTime == totalDuration)
			break;
		// All New Customer Came
		while (customer_queue->size > 0 && ((customer *)customer_queue->head->val)->arrivalTime <= currentTime)
		{
			customer *temp = (customer *)pollFromQueue(customer_queue);
			addToQueue(sellerQue, temp);
			printf("00:%02d 	%c%d 		Customer Id %c%d%02d arrived\n", currentTime, sellerType, id, sellerType, id, temp->id);
		}
		// Serve next customer
		if (cust == NULL && sellerQue->size > 0)
		{
			cust = (customer *)pollFromQueue(sellerQue);
			cust->responseTime = currentTime - cust->arrivalTime;

			printf("00:%02d 	%c%d 		Serving Customer Id %c%d%02d	%8d\n", currentTime, sellerType, id, sellerType, id, cust->id, cust->responseTime);
			switch (sellerType)
			{
			case 'H':
				random_wait_time = (rand() % 2) + 1;
				responseTimeHighPriceTickets = responseTimeHighPriceTickets + cust->responseTime;
				break;
			case 'M':
				random_wait_time = (rand() % 3) + 2;
				responseTimeMediumPriceTickets = responseTimeMediumPriceTickets + cust->responseTime;
				break;
			case 'L':
				random_wait_time = (rand() % 4) + 4;
				responseTimeLowPriceTickets = responseTimeLowPriceTickets + cust->responseTime;
			}
		}
		if (cust != NULL)
		{
			// printf("Wait currentTime %d\n",random_wait_time);
			if (random_wait_time == 0)
			{
				// Selling Seat
				pthread_mutex_lock(&reservationMutex);

				// Find seat
				int si = getAvailableSeat(sellerType);
				if (si == -1)
				{
					printf("00:%02d 	%c%d 			Customer Id %c%d%02d has been told Concert Sold Out.\n", currentTime, sellerType, id, sellerType, id, cust->id);
				}
				else
				{
					int row_no = si / concertCols;
					int col_no = si % concertCols;
					cust->turnaroundTime = cust->turnaroundTime + currentTime;
					sprintf(seats[row_no][col_no], "%c%d%02d", sellerType, id, cust->id);
					printf("00:%02d 	%c%d 		Customer Id %c%d%02d assigned seat %d,%d %15d\n", currentTime, sellerType, id, sellerType, id, cust->id, row_no, col_no, cust->turnaroundTime);

					switch (sellerType)
					{
					case 'H':

						turnAroundTimeHighPriceTickets = turnAroundTimeHighPriceTickets + cust->turnaroundTime;
						break;
					case 'M':
						turnAroundTimMediumPriceTickets = turnAroundTimMediumPriceTickets + cust->turnaroundTime;
						break;
					case 'L':
						turnAroundTimeLowPriceTickets = turnAroundTimeLowPriceTickets + cust->turnaroundTime;
					}
				}
				pthread_mutex_unlock(&reservationMutex);
				cust = NULL;
			}
			else
			{
				random_wait_time--;
			}
		} // else {
		  // }
	}

	while (cust != NULL || sellerQue->size > 0)
	{
		if (cust == NULL)
			cust = (customer *)pollFromQueue(sellerQue);
		printf("00:%02d 	%c%d 		Ticket Window Closed. Customer Id %c%d%02d Leaves\n", currentTime, sellerType, id, sellerType, id, cust->id);
		cust = NULL;
	}
	pthread_mutex_lock(&threadCountMutex);
	runningThread--;
	pthread_mutex_unlock(&threadCountMutex);
}

int compareOnArrivalTime(void *cust1, void *cust2)
{
	customer *c1 = (customer *)cust1;
	customer *c2 = (customer *)cust2;
	if (c1->arrivalTime < c2->arrivalTime)
		return -1;
	else if (c1->arrivalTime < c2->arrivalTime)
		return 1;
	else
		return 0;
}

int getAvailableSeat(char sellerType)
{
	int si = -1;

	if (sellerType == 'H')
	{
		for (int r = 0; r < concertRows; r++)
		{
			for (int c = 0; c < concertCols; c++)
			{
				if (strcmp(seats[r][c], "-") == 0)
				{
					si = r * concertCols + c;
					return si;
				}
			}
		}
	}
	else if (sellerType == 'M')
	{
		int mid = concertRows / 2;
		int skip = 0;
		// int next_row_no = mid;
		for (skip = 0;; skip++)
		{
			int r = mid + skip;
			if (mid + skip < concertRows)
			{
				for (int c = 0; c < concertCols; c++)
				{
					if (strcmp(seats[r][c], "-") == 0)
					{
						si = r * concertCols + c;
						return si;
					}
				}
			}
			r = mid - skip;
			if (mid - skip >= 0)
			{
				for (int c = 0; c < concertCols; c++)
				{
					if (strcmp(seats[r][c], "-") == 0)
					{
						si = r * concertCols + c;
						return si;
					}
				}
			}
			if (mid + skip >= concertRows && mid - skip < 0)
			{
				break;
			}
		}
	}
	else if (sellerType == 'L')
	{
		for (int r = concertRows - 1; r >= 0; r--)
		{
			for (int c = concertCols - 1; c >= 0; c--)
			{
				if (strcmp(seats[r][c], "-") == 0)
				{
					si = r * concertCols + c;
					return si;
				}
			}
		}
	}

	return -1;
}

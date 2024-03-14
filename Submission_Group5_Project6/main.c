#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

#define BUFFER_SIZE 32
#define READ_END 0
#define WRITE_END 1
#define CHILDREN 5
#define DURATION 30
#define TIME_BUFF_SIZE 10
struct timeval startTV;
#define INSEC(MIN, SEC, USEC) ((60 * MIN) + SEC + (USEC / 1000000))
char *getTime()
{
    struct timeval now;
    char *retval = (char *)calloc(TIME_BUFF_SIZE + 1, sizeof(char));
    gettimeofday(&now, NULL);
    int msSinceStart = (now.tv_usec - startTV.tv_usec) / 1000;
    if (msSinceStart < 0)
    {
        msSinceStart = ((now.tv_usec + 1000000) - startTV.tv_usec) / 1000;
    }
    int secsSinceStart = now.tv_sec - startTV.tv_sec;
    snprintf(retval, TIME_BUFF_SIZE, "0:%02d.%03d:", secsSinceStart, msSinceStart);
    return retval;
}

void timeDiff(
    const int *min_start, const int *sec_start, const int *usec_start,
    const int *min_end, const int *sec_end, const int *usec_end,
    int *min_diff, int *sec_diff, int *usec_diff)
{
    // CALCULATE THE DIFFERENCE BETWEEN TWO TIME VALUES to have a timestamp from starttime to read time/writetime.
    long start_total = 0, end_total = 0, diff_total = 0;

    start_total += *min_start * 60 * 1000000;
    start_total += *sec_start * 1000000;
    start_total += *usec_start;

    end_total += *min_end * 60 * 1000000;
    end_total += *sec_end * 1000000;
    end_total += *usec_end;

    diff_total = end_total - start_total;

    *min_diff = diff_total / 60 / 1000000;
    diff_total -= *min_diff * 60 * 1000000;

    *sec_diff = diff_total / 1000000;
    diff_total -= *sec_diff * 1000000;

    *usec_diff = diff_total;
    diff_total -= *usec_diff;
}

int main(void)
{
    // INITIALIZATION.
    int pipe_fds[CHILDREN][2] = {0};       // FILE DESCRIPTORS FOR THE PIPES.
    int child_process_ids[CHILDREN] = {0}; // CHILD PROCESS IDS.
    int process_ids, child_index = 0;

    // CREATE THE PIPES.
    for (int i = 0; i < CHILDREN; i++)
    {
        if (pipe(pipe_fds[i]) == -1)
        {
            fprintf(stderr, "Error: pipe() failed\n");
            return 1;
        }
    }

    // CREATE THE CHILD PROCESSES.
    while (child_index < CHILDREN)
    {
        process_ids = fork();
        if (process_ids == 0)
        { // If CHILD PROCS DO NOT CREATE CHILD PROCS.
            break;
        }
        else
        {
            child_process_ids[child_index] = process_ids; // RECORD CHILD PROC IDS.
            child_index++;
        }
    }

    // INITIATION TIME.
    struct timeval current_time, select_timeout;
    struct tm *today;
    int minutes, seconds, microseconds;
    int diff_minutes, diff_seconds, diff_microseconds;
    int child_clock = 0, message_counter = 0, sleep_duration, select_result, bytes_to_read, parent_completion_flag = 0;
    char message_buffer[128]; // READ message_buffer FOR PIPE.
    fd_set fd_set_for_select; // SET OF FILE DESCRIPTORS BIT-ARRAYS.

    // GET THE PROCESS START TIME.
    gettimeofday(&current_time, NULL);
    today = localtime(&current_time.tv_sec);
    minutes = today->tm_min;
    seconds = today->tm_sec;
    microseconds = current_time.tv_usec;
    struct timeval tv;
    char *timebuff;
    time_t startTime = 0;
    tv.tv_sec = 4;
    tv.tv_usec = 0;
    startTime = time(0);
    gettimeofday(&startTV, NULL);
    // PARENT PROCESS
    if (process_ids > 0)
    {
        for (int i = 0; i < CHILDREN; i++)
            close(pipe_fds[i][WRITE_END]); // CLOSE PIPE WRITE ENDS.

        FILE *output_file;
        output_file = fopen("output.txt", "w");

        while (parent_completion_flag < CHILDREN)
        {
            parent_completion_flag = 0;
            select_timeout.tv_sec = 2;
            select_timeout.tv_usec = 500000;

            FD_ZERO(&fd_set_for_select); // INITIALIZE fd_set_for_select TO THE EMPTY SET.

            for (int i = 0; i < CHILDREN; i++)
                FD_SET(pipe_fds[i][READ_END], &fd_set_for_select); // ADD CHILD PROC FILE DESCRIPTORS TO SELECT.

            select_result = select(FD_SETSIZE, &fd_set_for_select, (fd_set *)0, (fd_set *)0, &select_timeout);

            switch (select_result)
            {
            case 0:
            { // select_timeout W/O INPUT.
                fprintf(stderr, "\r@");
                fflush(stderr);
            }
            case -1:
            { // ERROR.
                perror("select()");
                exit(1);
            }
            default:
            { // READ PIPED DATA FROM CHILD PROCESSES.

                for (int i = 0; i < CHILDREN; i++)
                {
                    if (FD_ISSET(pipe_fds[i][READ_END], &fd_set_for_select))
                    {
                        ioctl(pipe_fds[i][READ_END], FIONREAD, &bytes_to_read); // READ # OF BYTES AVAILABLE ON STDIN
                        if (bytes_to_read == 0)
                        {
                            parent_completion_flag++;
                        }
                        else
                        {
                            bytes_to_read = read(pipe_fds[i][READ_END], message_buffer, bytes_to_read);
                            message_buffer[bytes_to_read] = '\0';

                            // struct timeval now;
                            // gettimeofday(&now, NULL);
                            // today = localtime(&now.tv_sec);
                            // timeDiff(&minutes, &seconds, &microseconds,&(today->tm_min), &(today->tm_sec),(int*)&(now.tv_usec), &diff_minutes, &diff_seconds,&diff_microseconds);
                            // char timestamped_message[128];
                            // sprintf(timestamped_message, "%02d:%02d.%03d: %s", diff_minutes, diff_seconds,diff_microseconds / 1000, message_buffer);
                            timebuff = getTime();
                            fprintf(stdout, "%s %s", timebuff, message_buffer);
                            fflush(stdout);
                            fprintf(output_file, "%s %s", timebuff, message_buffer);
                            fflush(stdout);
                            free(timebuff);
                            timebuff = NULL;
                        }
                    }
                }
                break;
            }
            }
        }

        for (int i = 0; i < CHILDREN; i++)
            waitpid(child_process_ids[i], NULL, 0);
        fprintf(stderr, "DONE: Parent///\nTerminating the main");

        fclose(output_file);
    }
    // CHILD PROCESSES: PASS TIMESTAMP
    else if ((process_ids == 0) && (child_index != 0))
    {
        srand(child_index);
        for (int i = 0; i < child_index; i++)
            rand();
        sleep(rand() % 3);

        close(pipe_fds[child_index][READ_END]); // CLOSE PIPE READ END.

        while (child_clock < DURATION)
        {
            // GET TIME.
            gettimeofday(&current_time, NULL);
            today = localtime(&current_time.tv_sec);

            // CALC TIMESTAMP.
            timeDiff(
                &minutes, &seconds, &microseconds,
                &(today->tm_min), &(today->tm_sec), (int *)&(current_time.tv_usec),
                &diff_minutes, &diff_seconds, &diff_microseconds);
            sprintf(message_buffer, "%01d:%02d.%03d: Child %d Message %d\n",
                    diff_minutes,
                    diff_seconds,
                    diff_microseconds / 1000,
                    child_index, message_counter++);

            // WRITE TO THE PIPE.
            write(pipe_fds[child_index][WRITE_END], message_buffer, strlen(message_buffer) + 1);

            // INCREMENT child_clock AND SLEEP.
            sleep_duration = rand() % 3; // + 1;
            child_clock += sleep_duration;
            sleep(sleep_duration);
        }

        // GET TIME.
        gettimeofday(&current_time, NULL);
        today = localtime(&current_time.tv_sec);

        // CALC TIMESTAMP.
        timeDiff(
            &minutes, &seconds, &microseconds,
            &(today->tm_min), &(today->tm_sec), (int *)&(current_time.tv_usec),
            &diff_minutes, &diff_seconds, &diff_microseconds);
        sprintf(message_buffer, "%01d:%02d.%03d: Child %d DONE\n",
                diff_minutes,
                diff_seconds,
                diff_microseconds / 1000,
                child_index);
        write(pipe_fds[child_index][WRITE_END], message_buffer, strlen(message_buffer) + 1);
        close(pipe_fds[child_index][WRITE_END]);
    }
    // CHILD PROCESSES: READ STDIN
    else if ((process_ids == 0) && (child_index == 0))
    {
        int bytes_to_read, prev_diff = 0;
        char buf_usr_in[64];
        close(pipe_fds[child_index][READ_END]);

        while (child_clock < DURATION)
        {
            select_timeout.tv_sec = DURATION - child_clock;
            select_timeout.tv_usec = 00000;

            FD_ZERO(&fd_set_for_select);              // initialize fd_set_for_select to the empty set
            FD_SET(STDIN_FILENO, &fd_set_for_select); // set child proc file descriptors

            select_result = select(STDIN_FILENO + 1, &fd_set_for_select, (fd_set *)0, (fd_set *)0, &select_timeout);

            // GET TIME.
            gettimeofday(&current_time, NULL);
            today = localtime(&current_time.tv_sec);

            switch (select_result)
            {
            case 0:
            { // select_timeout W/O INPUT.
                fprintf(stderr, "\r#");
                fflush(stderr);
                timeDiff(
                    &minutes, &seconds, &microseconds,
                    &(today->tm_min), &(today->tm_sec), (int *)&(current_time.tv_usec),
                    &diff_minutes, &diff_seconds, &diff_microseconds);
                break;
            }
            case -1:
            { // ERROR.
                perror("select()");
                exit(1);
            }
            default:
            { // READ PIPED DATA FROM CHILD PROCESSES.
                if (FD_ISSET(STDIN_FILENO, &fd_set_for_select))
                {
                    ioctl(STDIN_FILENO, FIONREAD, &bytes_to_read); // READ # OF BYTES AVAILABLE ON STDIN.
                    if (bytes_to_read == 0)
                    {
                        fprintf(stderr, "Process done.\n");
                        exit(0);
                    }
                    bytes_to_read = read(STDIN_FILENO, buf_usr_in, bytes_to_read);
                    buf_usr_in[bytes_to_read] = '\0';

                    // CALC TIMESTAMP.
                    timeDiff(
                        &minutes, &seconds, &microseconds,
                        &(today->tm_min), &(today->tm_sec), (int *)&(current_time.tv_usec),
                        &diff_minutes, &diff_seconds, &diff_microseconds);
                    sprintf(message_buffer, "%01d:%02d.%03d: Child %d Message %d: %s",
                            diff_minutes,
                            diff_seconds,
                            diff_microseconds / 1000,
                            child_index, message_counter++, buf_usr_in);

                    // WRITE TO THE PIPE.
                    write(pipe_fds[child_index][WRITE_END], message_buffer, strlen(message_buffer) + 1);
                }
            }
            }

            // INCREMENT child_clock AND SLEEP.
            child_clock += INSEC(diff_minutes, diff_seconds, diff_microseconds) - prev_diff;
            prev_diff = INSEC(diff_minutes, diff_seconds, diff_microseconds);
        }

        // GET TIME.
        gettimeofday(&current_time, NULL);
        today = localtime(&current_time.tv_sec);

        // CALC TIMESTAMP.
        timeDiff(
            &minutes, &seconds, &microseconds,
            &(today->tm_min), &(today->tm_sec), (int *)&(current_time.tv_usec),
            &diff_minutes, &diff_seconds, &diff_microseconds);
        sprintf(message_buffer, "%01d:%02d.%03d: Child %d HAS CLOSED THE PIPE\n",
                diff_minutes,
                diff_seconds,
                diff_microseconds / 1000,
                child_index);
        write(pipe_fds[child_index][WRITE_END], message_buffer, strlen(message_buffer) + 1);
        close(pipe_fds[child_index][WRITE_END]);
    }
    else
    {
        fprintf(stderr, "Error: fork() HAS failed");
        return 1;
    }
    return 0;
}

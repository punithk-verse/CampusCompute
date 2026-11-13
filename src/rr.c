
#include <stdio.h>
#include <stdlib.h>
#include "job.h"
#include "rr.h"

void round_robin(job_t jobs[], int count, int quantum){

    if(count <= 0 || count > MAX_JOBS || quantum <= 0){
        printf("Invalid input for Round Robin scheduling.\n");
        return;
    }

    job_t scheduled[MAX_JOBS];

    for(int i = 0; i < count; i++){
        scheduled[i] = jobs[i];
        scheduled[i].remaining_time = scheduled[i].burst_time;
        scheduled[i].completion_time = 0;
        scheduled[i].turnaround_time = 0;
        scheduled[i].waiting_time = 0;
        scheduled[i].status = JOB_WAITING;
    }

    // Sort processes according to arrival time
    for(int i = 0; i < count - 1; i++){
        for(int j = 0; j < count - i - 1; j++){
            if(scheduled[j].arrival_time > scheduled[j + 1].arrival_time){
                job_t temp = scheduled[j];
                scheduled[j] = scheduled[j + 1];
                scheduled[j + 1] = temp;
            }
        }
    }

    // Circular ready queue
    int queue[MAX_JOBS];
    int front = 0;
    int rear = 0;
    int queue_count = 0;

    int current_time = 0;
    int added[MAX_JOBS] = {0};
    int completed = 0;

    // Dynamically allocated Gantt chart history
    int capacity = 100;
    int gantt_count = 0;

    int *gantt_process = malloc(capacity * sizeof(int));
    int *gantt_start = malloc(capacity * sizeof(int));
    int *gantt_end = malloc(capacity * sizeof(int));

    if(gantt_process == NULL || gantt_start == NULL || gantt_end == NULL){
        printf("Memory allocation failed.\n");
        free(gantt_process);
        free(gantt_start);
        free(gantt_end);
        return;
    }

    // Add initially arrived processes
    for(int i = 0; i < count; i++){
        if(scheduled[i].arrival_time <= current_time){
            queue[rear] = i;
            rear = (rear + 1) % MAX_JOBS;
            queue_count++;
            added[i] = 1;
        }
    }

    while(completed < count){

        // Handle CPU idle time
        if(queue_count == 0){

            int next_arrival = -1;

            for(int i = 0; i < count; i++){
                if(added[i] == 0){
                    next_arrival = i;
                    break;
                }
            }

            if(next_arrival != -1){
                int start = current_time;
                current_time = scheduled[next_arrival].arrival_time;

                // Expand Gantt history if full
                if(gantt_count == capacity){
                    capacity *= 2;

                    int *temp_process = realloc(gantt_process, capacity * sizeof(int));
                    if(temp_process == NULL){
                        printf("Memory allocation failed.\n");
                        free(gantt_process);
                        free(gantt_start);
                        free(gantt_end);
                        return;
                    }
                    gantt_process = temp_process;

                    int *temp_start = realloc(gantt_start, capacity * sizeof(int));
                    if(temp_start == NULL){
                        printf("Memory allocation failed.\n");
                        free(gantt_process);
                        free(gantt_start);
                        free(gantt_end);
                        return;
                    }
                    gantt_start = temp_start;

                    int *temp_end = realloc(gantt_end, capacity * sizeof(int));
                    if(temp_end == NULL){
                        printf("Memory allocation failed.\n");
                        free(gantt_process);
                        free(gantt_start);
                        free(gantt_end);
                        return;
                    }
                    gantt_end = temp_end;
                }

                // Record idle interval
                gantt_process[gantt_count] = -1;
                gantt_start[gantt_count] = start;
                gantt_end[gantt_count] = current_time;
                gantt_count++;
            }

            // Add newly arrived processes
            for(int i = 0; i < count; i++){
                if(added[i] == 0 && scheduled[i].arrival_time <= current_time){
                    queue[rear] = i;
                    rear = (rear + 1) % MAX_JOBS;
                    queue_count++;
                    added[i] = 1;
                }
            }
        }

        // Remove process from front of queue
        int index = queue[front];

        front = (front + 1) % MAX_JOBS;
        queue_count--;

        int exec_time;

        if(scheduled[index].remaining_time < quantum){
            exec_time = scheduled[index].remaining_time;
        }
        else{
            exec_time = quantum;
        }

        // Expand Gantt history if full
        if(gantt_count == capacity){
            capacity *= 2;

            int *temp_process = realloc(gantt_process, capacity * sizeof(int));
            if(temp_process == NULL){
                printf("Memory allocation failed.\n");
                free(gantt_process);
                free(gantt_start);
                free(gantt_end);
                return;
            }
            gantt_process = temp_process;

            int *temp_start = realloc(gantt_start, capacity * sizeof(int));
            if(temp_start == NULL){
                printf("Memory allocation failed.\n");
                free(gantt_process);
                free(gantt_start);
                free(gantt_end);
                return;
            }
            gantt_start = temp_start;

            int *temp_end = realloc(gantt_end, capacity * sizeof(int));
            if(temp_end == NULL){
                printf("Memory allocation failed.\n");
                free(gantt_process);
                free(gantt_start);
                free(gantt_end);
                return;
            }
            gantt_end = temp_end;
        }

        // Record process execution
        gantt_process[gantt_count] = index;
        gantt_start[gantt_count] = current_time;

        scheduled[index].status = JOB_RUNNING;

        current_time += exec_time;

        gantt_end[gantt_count] = current_time;
        gantt_count++;

        scheduled[index].remaining_time -= exec_time;

        // Add newly arrived processes
        for(int i = 0; i < count; i++){
            if(added[i] == 0 && scheduled[i].arrival_time <= current_time){
                queue[rear] = i;
                rear = (rear + 1) % MAX_JOBS;
                queue_count++;
                added[i] = 1;
            }
        }

        // Check process completion
        if(scheduled[index].remaining_time == 0){

            scheduled[index].completion_time = current_time;

            scheduled[index].turnaround_time =
                scheduled[index].completion_time - scheduled[index].arrival_time;

            scheduled[index].waiting_time =
                scheduled[index].turnaround_time - scheduled[index].burst_time;

            scheduled[index].status = JOB_COMPLETED;
            completed++;
        }
        else{
            // Re-enqueue unfinished process
            scheduled[index].status = JOB_WAITING;

            queue[rear] = index;
            rear = (rear + 1) % MAX_JOBS;
            queue_count++;
        }
    }

    // Display Gantt chart
    printf("\nRound Robin Gantt Chart:\n");

    for(int i = 0; i < gantt_count; i++){
        printf("%-8d", gantt_start[i]);
    }

    printf("%d\n", gantt_end[gantt_count - 1]);

    for(int i = 0; i < gantt_count; i++){

        if(gantt_process[i] == -1){
            printf("| Idle    ");
        }
        else{
            printf("| P%d      ", scheduled[gantt_process[i]].process_id);
        }
    }

    printf("|\n");

    // Display scheduling results
    float total_tat = 0;
    float total_wt = 0;

    printf("\nPID\tAT\tBT\tCT\tTAT\tWT\n");

    for(int i = 0; i < count; i++){

        printf("P%d\t%d\t%d\t%d\t%d\t%d\n",
            scheduled[i].process_id,
            scheduled[i].arrival_time,
            scheduled[i].burst_time,
            scheduled[i].completion_time,
            scheduled[i].turnaround_time,
            scheduled[i].waiting_time);

        total_tat += scheduled[i].turnaround_time;
        total_wt += scheduled[i].waiting_time;
    }

    printf("\nAverage Turnaround Time: %.2f\n", total_tat / count);
    printf("Average Waiting Time: %.2f\n", total_wt / count);

    // Release dynamically allocated memory
    free(gantt_process);
    free(gantt_start);
    free(gantt_end);
}


#include <stdio.h>
#include "job.h"
#include "priority.h"

void priority_scheduling(job_t jobs[], int count) {
    if(count <= 0 || count > MAX_JOBS) {
        printf("Invalid input for Priority Scheduling.\n");
        return;
    }

    job_t scheduled[MAX_JOBS];

    for(int i = 0; i < count; i++) {
        scheduled[i] = jobs[i];
        scheduled[i].remaining_time = scheduled[i].burst_time;
        scheduled[i].completion_time = 0;
        scheduled[i].turnaround_time = 0;
        scheduled[i].waiting_time = 0;
        scheduled[i].status = JOB_WAITING;
    }

    int current_time = 0;
    int completed = 0;
    long long total_tat = 0;
    long long total_wt = 0;

    int gantt_process[MAX_JOBS * 2];
    int gantt_start[MAX_JOBS * 2];
    int gantt_end[MAX_JOBS * 2];
    int gantt_count = 0;

    while(completed < count) {
        int selected = -1;

        for(int i = 0; i < count; i++) {
            if(scheduled[i].status != JOB_COMPLETED &&
               scheduled[i].arrival_time <= current_time) {

                if(selected == -1 ||
                   scheduled[i].priority < scheduled[selected].priority) {
                    selected = i;
                }
            }
        }

        if(selected == -1) {
            int next_arrival = -1;

            for(int i = 0; i < count; i++) {
                if(scheduled[i].status != JOB_COMPLETED &&
                   (next_arrival == -1 ||
                    scheduled[i].arrival_time <
                        scheduled[next_arrival].arrival_time)) {
                    next_arrival = i;
                }
            }

            if(next_arrival != -1) {
                gantt_process[gantt_count] = -1;
                gantt_start[gantt_count] = current_time;
                current_time = scheduled[next_arrival].arrival_time;
                gantt_end[gantt_count] = current_time;
                gantt_count++;
            }
            continue;
        }

        scheduled[selected].status = JOB_RUNNING;

        gantt_process[gantt_count] = selected;
        gantt_start[gantt_count] = current_time;

        current_time += scheduled[selected].burst_time;

        scheduled[selected].completion_time = current_time;
        scheduled[selected].remaining_time = 0;
        scheduled[selected].turnaround_time =
            scheduled[selected].completion_time -
            scheduled[selected].arrival_time;
        scheduled[selected].waiting_time =
            scheduled[selected].turnaround_time -
            scheduled[selected].burst_time;

        scheduled[selected].status = JOB_COMPLETED;

        gantt_end[gantt_count] = current_time;
        gantt_count++;

        total_tat += scheduled[selected].turnaround_time;
        total_wt += scheduled[selected].waiting_time;

        completed++;
    }

    printf("\nNon-Preemptive Priority Gantt Chart:\n");

    for(int i = 0; i < gantt_count; i++) {
        printf("%-8d", gantt_start[i]);
    }

    printf("%d\n", gantt_end[gantt_count - 1]);

    for(int i = 0; i < gantt_count; i++) {
        if(gantt_process[i] == -1) {
            printf("| Idle    ");
        } else {
            printf("| P%d      ", scheduled[gantt_process[i]].process_id);
        }
    }

    printf("|\n");

    printf("\n%-8s %-8s %-8s %-10s %-8s %-8s %-8s\n",
           "PID", "AT", "BT", "Priority", "CT", "TAT", "WT");

    for(int i = 0; i < count; i++) {
        printf("P%-7d %-8d %-8d %-10d %-8d %-8d %-8d\n",
            scheduled[i].process_id,
            scheduled[i].arrival_time,
            scheduled[i].burst_time,
            scheduled[i].priority,
            scheduled[i].completion_time,
            scheduled[i].turnaround_time,
            scheduled[i].waiting_time);
    }

    printf("\nAverage Turnaround Time: %.2f\n",
        (float)total_tat / count);

    printf("Average Waiting Time: %.2f\n",
        (float)total_wt / count);
}

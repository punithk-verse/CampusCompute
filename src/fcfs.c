
#include<stdio.h>
#include "job.h"
#include "fcfs.h"

void fcfs(job_t jobs[],int count){
    if(count==0){
        printf("No jobs available\n");
        return;
    }

    job_t scheduled[MAX_JOBS];

    for(int i=0;i<count;i++){
        scheduled[i]=jobs[i];
    }

    int current_time=0;
    int start_time[MAX_JOBS];
    job_t temp;

    for(int i=0;i<count-1;i++){
        for(int j=0;j<count-i-1;j++){
            if(scheduled[j].arrival_time>scheduled[j+1].arrival_time){
                temp=scheduled[j];
                scheduled[j]=scheduled[j+1];
                scheduled[j+1]=temp;
            }
        }
    }

    printf("\n fcfs scheduling   \n");

    for(int i=0;i<count;i++){

        if(current_time<scheduled[i].arrival_time){
            current_time=scheduled[i].arrival_time;
        }

        start_time[i]=current_time;

        scheduled[i].status=JOB_RUNNING;

        current_time+=scheduled[i].burst_time;

        scheduled[i].completion_time=current_time;
        scheduled[i].turnaround_time=scheduled[i].completion_time-scheduled[i].arrival_time;
        scheduled[i].waiting_time=scheduled[i].turnaround_time-scheduled[i].burst_time;

        scheduled[i].remaining_time=0;
        scheduled[i].status=JOB_COMPLETED;
    }

    printf("\nFCFS Gantt Chart:\n");

printf("%d", 0);
int time = 0;

for(int i=0;i<count;i++){

    if(time < scheduled[i].arrival_time){
        printf("\t%d", scheduled[i].arrival_time);
        time = scheduled[i].arrival_time;
    }

    printf("\t%d", scheduled[i].completion_time);
    time = scheduled[i].completion_time;
}

printf("\n");

time = 0;

for(int i=0;i<count;i++){

    if(time < scheduled[i].arrival_time){
        printf("| Idle\t");
        time = scheduled[i].arrival_time;
    }

    printf("| P%d\t", scheduled[i].process_id);
    time = scheduled[i].completion_time;
}

printf("|\n");

    float total_tat=0;
    float total_wt=0;

    printf("\nProcess ID   AT   BT   CT   TAT   WT\n");

    for(int i=0;i<count;i++){

        printf("%-8d %-4d %-4d %-4d %-5d %d\n",
               scheduled[i].process_id,
               scheduled[i].arrival_time,
               scheduled[i].burst_time,
               scheduled[i].completion_time,
               scheduled[i].turnaround_time,
               scheduled[i].waiting_time);

        total_tat+=scheduled[i].turnaround_time;
        total_wt+=scheduled[i].waiting_time;
    }

    printf("\nAverage Turnaround Time: %.2f\n",
           total_tat/count);

    printf("Average Waiting Time: %.2f\n",
           total_wt/count);
}

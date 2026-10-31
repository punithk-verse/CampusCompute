#include<stdio.h>
#include "job.h"
#include "fcfs.h"
void fcfs(job_t jobs[],int count){
    if(count==0){
        printf("No jobs available\n");
        return;
    }
    int current_time=0;
    job_t temp;
    for(int i=0;i<count-1;i++){
        for(int j=0;j<count-i-1;j++){
            if(jobs[j].arrival_time>jobs[j+1].arrival_time){
                    temp=jobs[j];
                 jobs[j]=jobs[j+1];
            jobs[j+1]=temp;
            }

         }
     }

printf("\n fcfs scheduling   \n");
printf("\n gantt chart:\n");
for(int i=0;i<count;i++){
    if(current_time<jobs[i].arrival_time){
        current_time=jobs[i].arrival_time;

    }
    jobs[i].status=JOB_RUNNING;

current_time+=jobs[i].burst_time;
jobs[i].completion_time=current_time;
jobs[i].turnaround_time=jobs[i].completion_time-jobs[i].arrival_time;
jobs[i].waiting_time=jobs[i].turnaround_time-jobs[i].burst_time;
jobs[i].remaining_time=0;
jobs[i].status=JOB_COMPLETED;
printf("| J%d ",jobs[i].job_id);
}
printf("|\n");

float total_tat=0;
float total_wt=0;
printf("\nJob ID   AT   BT   CT   TAT   WT\n");

    for (int i = 0; i < count; i++) {

        printf("%-8d %-4d %-4d %-4d %-5d %d\n",
               jobs[i].job_id,
               jobs[i].arrival_time,
               jobs[i].burst_time,
               jobs[i].completion_time,
               jobs[i].turnaround_time,
               jobs[i].waiting_time);

        total_tat += jobs[i].turnaround_time;
        total_wt += jobs[i].waiting_time;
    }

    printf("\nAverage Turnaround Time: %.2f\n",
           total_tat / count);

    printf("Average Waiting Time: %.2f\n",
           total_wt / count);
}
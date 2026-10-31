#include <stdio.h>
#include "job.h"
void initialize_job(job_t *job){
    job->remaining_time=job->burst_time;
    job->completion_time=0;
    job->turnaround_time=0;
    job->waiting_time=0;
    job->status=JOB_WAITING;
}

int add_job(job_t jobs[],int *count){
    if(*count>=MAX_JOBS){
        printf("error: Maximum job limit reached \n");
        return 0;
    }
    job_t new_job;
    printf("Enter Process ID :");
    if (scanf("%d", &new_job.process_id) != 1 || new_job.process_id < 0) {
        printf("Invalid Process ID.\n");
        return 0;
    }
    for(int i=0;i<*count;i++){
        if(jobs[i].process_id==new_job.process_id){
            printf("Error: Duplicate Process ID. \n");
            return 0;
        }
    }
    printf("enter Arrival time: ");
    if(scanf("%d",&new_job.arrival_time)!=1 || new_job.arrival_time<0){
        printf("invalid arrival time \n");
        return 0;
    }
    printf("Enter Burst Time: ");
    if (scanf("%d", &new_job.burst_time) != 1 ||
        new_job.burst_time <= 0) {
        printf("Invalid Burst Time.\n");
        return 0;
    }
    printf("Enter Priority(only for non-preemptive priority scheduling ): ");
    if (scanf("%d", &new_job.priority) != 1 ||
        new_job.priority < 0) {
        printf("Invalid Priority.\n");
        return 0;
    }

    initialize_job(&new_job);
    jobs[*count]=new_job;
    (*count)++;
    printf("Job added successfully \n");
    return 1;
}
void display_jobs(job_t jobs[],int count){
    if(count==0){
        printf("no job avialble \n");
        return;
    }
    printf("\nProcess ID   Arrival   Burst   Priority   Remaining   Status\n");

   for (int i = 0; i < count; i++) {

        char *status;

        switch (jobs[i].status) {
            case JOB_WAITING:
                status = "Waiting";
                break;

            case JOB_RUNNING:
                status = "Running";
                break;

            case JOB_COMPLETED:
                status = "Completed";
                break;

            default:
                status = "Unknown";
        }

        printf("%d        %d         %d       %d          %d           %s\n",
               jobs[i].process_id,
               jobs[i].arrival_time,
               jobs[i].burst_time,
               jobs[i].priority,
               jobs[i].remaining_time,
               status);
    }
}

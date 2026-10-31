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
    printf("enter job id :");
    if (scanf("%d", &new_job.job_id) != 1 || new_job.job_id < 0) {
        printf("Invalid Job ID.\n");
        return 0;
    }
    for(int i=0;i<*count;i++){
        if(jobs[i].job_id==new_job.job_id){
            printf("Error:Duplicate JOB ID \n");
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
    printf("Enter Priority: ");
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
    printf("\nJob ID   Arrival   Burst   Priority   Remaining   Status\n");

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
               jobs[i].job_id,
               jobs[i].arrival_time,
               jobs[i].burst_time,
               jobs[i].priority,
               jobs[i].remaining_time,
               status);
    }
}

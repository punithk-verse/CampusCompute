
#include <stdio.h>
#include "job.h"
#include "fcfs.h"
int main(void) {
    job_t jobs[MAX_JOBS];
    int count = 0;
    int choice;

    do {
        printf("\n--- CampusCompute ---\n");
        printf("1. Add Job\n");
        printf("2. Display Jobs\n");
        printf("3. FCFS Scheduling\n");
        printf("0. Exit\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            return 1;
        }

        switch (choice) {
            case 1:
                add_job(jobs, &count);
                break;

            case 2:
                display_jobs(jobs, count);
                break;
            case 3:
               fcfs(jobs,count);
               break;

            case 0:
                printf("Exiting...\n");
                break;

            default:
                printf("Invalid choice.\n");
        }

    } while (choice != 0);
  //for debugging 
    return 0;
}

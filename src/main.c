
#include <stdio.h>
#include "input.h"
#include "job.h"
#include "fcfs.h"
#include "rr.h"
#include "priority.h"
#include "banker.h"
#include "logger.h"
#include "ipc.h"
#include "worker.h"
#include "resource_limit.h"

int main(void)
{
    job_t jobs[MAX_JOBS];
    int count = 0;
    int choice = -1;
    int done = 0;

    while (!done) {
        printf("\n--- CampusCompute ---\n");
        printf("1. Add Job\n");
        printf("2. Display Jobs\n");
        printf("3. FCFS Scheduling\n");
        printf("4. Round Robin Scheduling\n");
        printf("5. Priority Scheduling\n");
        printf("6. Banker's Algorithm\n");
        printf("7. Concurrent Logger\n");
        printf("8. Shared Memory / IPC\n");
        printf("9. Worker Process Failure Demonstration\n");
        printf("10. Resource Limit Demonstration\n");
        printf("11. Exit\n");

        int input_result = read_int_line("Enter choice: ", &choice);

        if (input_result == -1) {
            printf("\nEnd of input. Exiting.\n");
            break;
        } else if (input_result == 0) {
            continue;
        }

        switch (choice) {
            case 1:
                add_job(jobs, &count);
                break;

            case 2:
                display_jobs(jobs, count);
                break;

            case 3:
                fcfs(jobs, count);
                break;

            case 4:
            {
                int quantum;
                int quantum_result =
                    read_int_line("Enter Time Quantum: ", &quantum);

                if (quantum_result == -1) {
                    printf("\nEnd of input. Exiting.\n");
                    done = 1;
                    break;
                }
                if (quantum_result == 0) {
                    break;
                }
                if (quantum <= 0) {
                    printf("Time quantum must be greater than zero.\n");
                    break;
                }
                round_robin(jobs, count, quantum);
                break;
            }

            case 5:
                priority_scheduling(jobs, count);
                break;

            case 6:
                bankers_algorithm();
                break;

            case 7:
                run_logger_demo();
                break;

            case 8:
                run_ipc_demo();
                break;

            case 9:
                run_worker_failure_demo();
                break;

            case 10:
                run_resource_limit_demo();
                break;

            case 11:
                printf("Exiting...\n");
                done = 1;
                break;

            default:
                printf("Invalid choice.\n");
        }

    }

    return 0;
}

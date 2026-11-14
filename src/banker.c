#include <stdio.h>
#include "banker.h"
#include "input.h"

#define MAX_PROCESSES 10
#define MAX_RESOURCES 10

static int read_integer(const char *prompt, int *value) {
    int result;

    do {
        result = read_int_line(prompt, value);
        if(result == -1) {
            return 0;
        }
    } while(result == 0);

    return 1;
}

void bankers_algorithm(void) {
    int processes, resources;

    int allocation[MAX_PROCESSES][MAX_RESOURCES];
    int maximum[MAX_PROCESSES][MAX_RESOURCES];
    int need[MAX_PROCESSES][MAX_RESOURCES];

    int available[MAX_RESOURCES];
    long long work[MAX_RESOURCES];

    int finish[MAX_PROCESSES] = {0};
    int safe_sequence[MAX_PROCESSES];

    int completed = 0;

    if(!read_integer("Enter number of processes: ", &processes)) {
        printf("Input ended unexpectedly.\n");
        return;
    }

    if(processes <= 0 || processes > MAX_PROCESSES) {
        printf("Number of processes must be between 1 and %d.\n",
               MAX_PROCESSES);
        return;
    }

    if(!read_integer("Enter number of resource types: ", &resources)) {
        printf("Input ended unexpectedly.\n");
        return;
    }

    if(resources <= 0 || resources > MAX_RESOURCES) {
        printf("Number of resource types must be between 1 and %d.\n",
               MAX_RESOURCES);
        return;
    }

    printf("\nEnter Allocation Matrix:\n");

    for(int i = 0; i < processes; i++) {
        printf("Process P%d:\n", i);

        for(int j = 0; j < resources; j++) {
            char prompt[80];

            snprintf(prompt, sizeof(prompt),
                     "Allocation for P%d, Resource %d: ", i, j);

            if(!read_integer(prompt, &allocation[i][j])) {
                printf("Input ended unexpectedly.\n");
                return;
            }

            if(allocation[i][j] < 0) {
                printf("Allocation cannot be negative.\n");
                return;
            }
        }
    }

    printf("\nEnter Maximum Matrix:\n");

    for(int i = 0; i < processes; i++) {
        printf("Process P%d:\n", i);

        for(int j = 0; j < resources; j++) {
            char prompt[80];

            snprintf(prompt, sizeof(prompt),
                     "Maximum for P%d, Resource %d: ", i, j);

            if(!read_integer(prompt, &maximum[i][j])) {
                printf("Input ended unexpectedly.\n");
                return;
            }

            if(maximum[i][j] < 0) {
                printf("Maximum cannot be negative.\n");
                return;
            }

            if(maximum[i][j] < allocation[i][j]) {
                printf("Invalid input: Maximum cannot be less than Allocation.\n");
                return;
            }
        }
    }

    printf("\nEnter Available Resources:\n");

    for(int j = 0; j < resources; j++) {
        char prompt[80];

        snprintf(prompt, sizeof(prompt),
                 "Available Resource %d: ", j);

        if(!read_integer(prompt, &available[j])) {
            printf("Input ended unexpectedly.\n");
            return;
        }

        if(available[j] < 0) {
            printf("Available resources cannot be negative.\n");
            return;
        }

        work[j] = available[j];
    }

    /* Calculate Need = Maximum - Allocation */

    printf("\nNeed Matrix:\n");

    for(int i = 0; i < processes; i++) {
        printf("P%d\t", i);

        for(int j = 0; j < resources; j++) {
            need[i][j] = maximum[i][j] - allocation[i][j];
            printf("%d ", need[i][j]);
        }

        printf("\n");
    }

    /* Banker's Safety Algorithm */

    while(completed < processes) {
        int found = 0;

        for(int i = 0; i < processes; i++) {
            if(finish[i] == 0) {
                int can_execute = 1;

                for(int j = 0; j < resources; j++) {
                    if(need[i][j] > work[j]) {
                        can_execute = 0;
                        break;
                    }
                }

                if(can_execute) {
                    for(int j = 0; j < resources; j++) {
                        work[j] += allocation[i][j];
                    }

                    safe_sequence[completed] = i;
                    completed++;

                    finish[i] = 1;
                    found = 1;
                }
            }
        }

        if(found == 0) {
            break;
        }
    }

    /* Display Result */

    if(completed == processes) {
        printf("\nSystem is in a SAFE STATE.\n");

        printf("Safe Sequence: ");

        for(int i = 0; i < processes; i++) {
            printf("P%d", safe_sequence[i]);

            if(i < processes - 1) {
                printf(" -> ");
            }
        }

        printf("\n");
    }
    else {
        printf("\nSystem is in an UNSAFE STATE.\n");

        printf("Processes that could not complete: ");

        for(int i = 0; i < processes; i++) {
            if(finish[i] == 0) {
                printf("P%d ", i);
            }
        }

        printf("\n");
    }
}
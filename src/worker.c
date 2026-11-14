#include "worker.h"

#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

static int run_worker(const char *description, int simulate_failure)
{
    pid_t child = fork();
    int status;
    pid_t waited;

    if (child == -1) {
        perror("fork");
        return 0;
    }

    if (child == 0) {
        _exit(simulate_failure ? 42 : 0);
    }

    do {
        waited = waitpid(child, &status, 0);
    } while (waited == -1 && errno == EINTR);

    if (waited == -1) {
        perror("waitpid");
        return 0;
    }

    if (WIFEXITED(status)) {
        int exit_code = WEXITSTATUS(status);

        if (simulate_failure && exit_code == 42) {
            printf("Parent detected the controlled simulated worker failure "
                   "(child exit code %d).\n", exit_code);
            return 1;
        }
        if (!simulate_failure && exit_code == 0) {
            printf("Parent observed normal worker completion "
                   "(child exit code 0).\n");
            return 1;
        }

        printf("Parent observed unexpected %s worker exit code %d.\n",
               description, exit_code);
        return 0;
    }

    if (WIFSIGNALED(status)) {
        printf("Parent observed %s worker terminated by signal %d.\n",
               description, WTERMSIG(status));
    } else {
        printf("Parent observed an unrecognized %s worker result.\n",
               description);
    }
    return 0;
}

int run_worker_failure_demo(void)
{
    int normal_ok = run_worker("normal", 0);
    int simulated_failure_ok = run_worker("simulated-failure", 1);

    return normal_ok && simulated_failure_ok;
}

#define _POSIX_C_SOURCE 200809L

#include "ipc.h"

#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define IPC_MESSAGE_SIZE 256

typedef struct {
    char message[IPC_MESSAGE_SIZE];
} shared_message_t;

static int semaphore_wait_intr(sem_t *semaphore)
{
    while (sem_wait(semaphore) == -1) {
        if (errno != EINTR) {
            return -1;
        }
    }
    return 0;
}

static int semaphore_timed_wait(sem_t *semaphore)
{
    struct timespec deadline;

    if (clock_gettime(CLOCK_REALTIME, &deadline) == -1) {
        return -1;
    }
    deadline.tv_sec += 5;

    while (sem_timedwait(semaphore, &deadline) == -1) {
        if (errno != EINTR) {
            return -1;
        }
    }
    return 0;
}

static int wait_for_child(pid_t child, int *status)
{
    pid_t result;

    do {
        result = waitpid(child, status, 0);
    } while (result == -1 && errno == EINTR);

    return result == child ? 0 : -1;
}

int run_ipc_demo(void)
{
    char shared_name[64];
    char ready_name[64];
    char done_name[64];
    int shared_fd = -1;
    int shared_created = 0;
    int ready_created = 0;
    int done_created = 0;
    shared_message_t *shared = MAP_FAILED;
    sem_t *ready = SEM_FAILED;
    sem_t *done = SEM_FAILED;
    pid_t child = -1;
    int child_status = 0;
    int child_reaped = 0;
    int success = 0;

    if (snprintf(shared_name, sizeof(shared_name), "/campuscompute_shm_%ld",
                 (long)getpid()) >= (int)sizeof(shared_name) ||
        snprintf(ready_name, sizeof(ready_name), "/campuscompute_ready_%ld",
                 (long)getpid()) >= (int)sizeof(ready_name) ||
        snprintf(done_name, sizeof(done_name), "/campuscompute_done_%ld",
                 (long)getpid()) >= (int)sizeof(done_name)) {
        fprintf(stderr, "Could not create IPC resource names.\n");
        return 0;
    }

    shared_fd = shm_open(shared_name, O_CREAT | O_EXCL | O_RDWR, 0600);
    if (shared_fd == -1) {
        perror("shm_open");
        goto cleanup;
    }
    shared_created = 1;

    if (ftruncate(shared_fd, (off_t)sizeof(*shared)) == -1) {
        perror("ftruncate");
        goto cleanup;
    }

    shared = mmap(NULL, sizeof(*shared), PROT_READ | PROT_WRITE, MAP_SHARED,
                  shared_fd, 0);
    if (shared == MAP_FAILED) {
        perror("mmap");
        goto cleanup;
    }

    ready = sem_open(ready_name, O_CREAT | O_EXCL, 0600, 0);
    if (ready == SEM_FAILED) {
        perror("sem_open (ready)");
        goto cleanup;
    }
    ready_created = 1;

    done = sem_open(done_name, O_CREAT | O_EXCL, 0600, 0);
    if (done == SEM_FAILED) {
        perror("sem_open (done)");
        goto cleanup;
    }
    done_created = 1;

    child = fork();
    if (child == -1) {
        perror("fork");
        goto cleanup;
    }

    if (child == 0) {
        char request[IPC_MESSAGE_SIZE];

        if (semaphore_wait_intr(ready) == -1) {
            _exit(10);
        }

        snprintf(request, sizeof(request), "%s", shared->message);
        snprintf(shared->message, sizeof(shared->message),
                 "Child received and replied: %.210s", request);

        if (sem_post(done) == -1) {
            _exit(11);
        }
        _exit(0);
    }

    snprintf(shared->message, sizeof(shared->message),
             "Hello from parent process %ld", (long)getpid());

    if (sem_post(ready) == -1) {
        perror("sem_post (ready)");
        kill(child, SIGTERM);
        goto reap_child;
    }

    if (semaphore_timed_wait(done) == -1) {
        perror("Timed wait for IPC child reply");
        kill(child, SIGTERM);
        goto reap_child;
    }

    if (wait_for_child(child, &child_status) == -1) {
        perror("waitpid");
        goto cleanup;
    }
    child_reaped = 1;

    if (!WIFEXITED(child_status) || WEXITSTATUS(child_status) != 0) {
        fprintf(stderr, "IPC child did not complete successfully.\n");
        goto cleanup;
    }

    printf("Shared-memory exchange: %s\n", shared->message);
    printf("Parent and child coordinated with named POSIX semaphores.\n");
    success = 1;
    goto cleanup;

reap_child:
    if (wait_for_child(child, &child_status) == -1) {
        perror("waitpid");
    } else {
        child_reaped = 1;
    }

cleanup:
    if (child > 0 && !child_reaped) {
        if (wait_for_child(child, &child_status) == -1) {
            perror("waitpid");
        }
    }
    if (done != SEM_FAILED) {
        sem_close(done);
    }
    if (ready != SEM_FAILED) {
        sem_close(ready);
    }
    if (done_created && sem_unlink(done_name) == -1) {
        perror("sem_unlink (done)");
        success = 0;
    }
    if (ready_created && sem_unlink(ready_name) == -1) {
        perror("sem_unlink (ready)");
        success = 0;
    }
    if (shared != MAP_FAILED && munmap(shared, sizeof(*shared)) == -1) {
        perror("munmap");
        success = 0;
    }
    if (shared_fd != -1 && close(shared_fd) == -1) {
        perror("close shared memory");
        success = 0;
    }
    if (shared_created && shm_unlink(shared_name) == -1) {
        perror("shm_unlink");
        success = 0;
    }

    return success;
}

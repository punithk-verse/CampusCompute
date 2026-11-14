#define _POSIX_C_SOURCE 200809L

#include "resource_limit.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <unistd.h>

static int highest_open_descriptor(int *highest)
{
    DIR *directory = opendir("/proc/self/fd");
    struct dirent *entry;
    int result = 0;
    int maximum = -1;

    if (directory == NULL) {
        return -1;
    }

    errno = 0;
    while ((entry = readdir(directory)) != NULL) {
        char *end;
        long descriptor = strtol(entry->d_name, &end, 10);

        if (*entry->d_name != '\0' && *end == '\0' &&
            descriptor >= 0 && descriptor <= 2147483647L &&
            descriptor > maximum) {
            maximum = (int)descriptor;
        }
        errno = 0;
    }

    if (errno != 0) {
        result = -1;
    }
    if (closedir(directory) == -1) {
        result = -1;
    }
    if (result == 0 && maximum >= 0) {
        *highest = maximum;
        return 0;
    }

    return -1;
}

int run_resource_limit_demo(void)
{
    struct rlimit original_limit;
    struct rlimit demonstration_limit;
    int highest;
    int *descriptors = NULL;
    size_t descriptor_count = 0;
    size_t descriptor_capacity = 0;
    int reached_limit = 0;
    int success = 0;
    int limit_changed = 0;

    if (getrlimit(RLIMIT_NOFILE, &original_limit) == -1) {
        perror("getrlimit");
        return 0;
    }
    if (highest_open_descriptor(&highest) == -1) {
        perror("Reading open descriptors from /proc/self/fd");
        return 0;
    }
    if (original_limit.rlim_cur <= (rlim_t)highest) {
        fprintf(stderr, "Cannot demonstrate RLIMIT_NOFILE: current "
                        "descriptors already reach its soft limit.\n");
        return 0;
    }

    demonstration_limit = original_limit;
    if (original_limit.rlim_cur > (rlim_t)highest + 4) {
        demonstration_limit.rlim_cur = (rlim_t)highest + 4;
    }

    if (setrlimit(RLIMIT_NOFILE, &demonstration_limit) == -1) {
        perror("setrlimit");
        return 0;
    }
    limit_changed = 1;

    for (;;) {
        int descriptor = open("/dev/null", O_RDONLY);

        if (descriptor == -1) {
            if (errno == EMFILE) {
                reached_limit = 1;
            } else {
                perror("open while demonstrating RLIMIT_NOFILE");
            }
            break;
        }

        if (descriptor_count == descriptor_capacity) {
            size_t new_capacity = descriptor_capacity == 0
                ? 8 : descriptor_capacity * 2;
            int *grown = realloc(descriptors, new_capacity * sizeof(*grown));

            if (grown == NULL) {
                perror("realloc");
                close(descriptor);
                break;
            }
            descriptors = grown;
            descriptor_capacity = new_capacity;
        }
        descriptors[descriptor_count++] = descriptor;
    }

    if (reached_limit) {
        printf("RLIMIT_NOFILE soft limit set to %llu; opening /dev/null "
               "stopped with EMFILE after %zu additional descriptor(s).\n",
               (unsigned long long)demonstration_limit.rlim_cur,
               descriptor_count);
        success = 1;
    } else {
        fprintf(stderr, "The configured descriptor limit was not observed.\n");
    }

    for (size_t i = 0; i < descriptor_count; i++) {
        if (close(descriptors[i]) == -1) {
            perror("close demonstration descriptor");
            success = 0;
        }
    }
    free(descriptors);

    if (limit_changed && setrlimit(RLIMIT_NOFILE, &original_limit) == -1) {
        perror("Restoring RLIMIT_NOFILE");
        success = 0;
    } else if (limit_changed) {
        printf("Original RLIMIT_NOFILE restored; all demonstration "
              "descriptors closed.\n");
    }

    return success;
}

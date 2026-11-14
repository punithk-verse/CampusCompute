#include "input.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int read_int_line(const char *prompt, int *value)
{
    char input[128];
    char *end;
    long parsed;

    printf("%s", prompt);
    fflush(stdout);

    if (fgets(input, sizeof(input), stdin) == NULL) {
        return -1;
    }

    end = input;
    errno = 0;
    parsed = strtol(input, &end, 10);

    if (end == input || errno == ERANGE ||
        parsed < INT_MIN || parsed > INT_MAX) {
        printf("Invalid input. Enter one integer.\n");
    } else {
        while (isspace((unsigned char)*end)) {
            end++;
        }

        if (*end == '\0') {
            *value = (int)parsed;
            return 1;
        }

        printf("Invalid input. Enter one integer.\n");
    }

    if (strchr(input, '\n') == NULL && !feof(stdin)) {
        int ch;

        while ((ch = getchar()) != '\n' && ch != EOF) {
        }
    }

    return 0;
}

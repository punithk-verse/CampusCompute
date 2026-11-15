
#include "logger.h"

#include <pthread.h>
#include <stdio.h>
#include <string.h>

#define LOGGER_QUEUE_CAPACITY 4
#define LOGGER_MESSAGE_SIZE 128

typedef struct {
    char message[LOGGER_MESSAGE_SIZE];
    int stop;
} log_item_t;

typedef struct {
    log_item_t items[LOGGER_QUEUE_CAPACITY];
    int head;
    int tail;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} log_queue_t;

static int logger_thread_error;

static int queue_push(log_queue_t *queue, const log_item_t *item)
{
    int result = pthread_mutex_lock(&queue->mutex);

    if (result != 0) {
        return result;
    }

    while (queue->count == LOGGER_QUEUE_CAPACITY) {
        result = pthread_cond_wait(&queue->not_full, &queue->mutex);
        if (result != 0) {
            pthread_mutex_unlock(&queue->mutex);
            return result;
        }
    }

    queue->items[queue->tail] = *item;
    queue->tail = (queue->tail + 1) % LOGGER_QUEUE_CAPACITY;
    queue->count++;

    result = pthread_cond_signal(&queue->not_empty);
    if (result != 0) {
        pthread_mutex_unlock(&queue->mutex);
        return result;
    }

    return pthread_mutex_unlock(&queue->mutex);
}

static int queue_pop(log_queue_t *queue, log_item_t *item)
{
    int result = pthread_mutex_lock(&queue->mutex);

    if (result != 0) {
        return result;
    }

    while (queue->count == 0) {
        result = pthread_cond_wait(&queue->not_empty, &queue->mutex);
        if (result != 0) {
            pthread_mutex_unlock(&queue->mutex);
            return result;
        }
    }

    *item = queue->items[queue->head];
    queue->head = (queue->head + 1) % LOGGER_QUEUE_CAPACITY;
    queue->count--;

    result = pthread_cond_signal(&queue->not_full);
    if (result != 0) {
        pthread_mutex_unlock(&queue->mutex);
        return result;
    }

    return pthread_mutex_unlock(&queue->mutex);
}

static void *logger_consumer(void *argument)
{
    log_queue_t *queue = argument;
    log_item_t item;
    int result;
    FILE *log_file = fopen("campuscompute.log", "a");

    if (log_file == NULL) {
        perror("Could not open campuscompute.log");
        return &logger_thread_error;
    }

    do {
        result = queue_pop(queue, &item);

        if (result != 0) {
            fclose(log_file);
            return &logger_thread_error;
        }

        if (!item.stop) {
            printf("[logger] Consumer dequeued: %s\n", item.message);

            if (fprintf(log_file, "[logger] %s\n", item.message) < 0) {
                fprintf(stderr, "Could not write to campuscompute.log.\n");
                fclose(log_file);
                return &logger_thread_error;
            }
        }

    } while (!item.stop);

    if (fclose(log_file) != 0) {
        perror("Could not close campuscompute.log");
        return &logger_thread_error;
    }

    return NULL;
}

static void *logger_producer(void *argument)
{
    const size_t event_count = LOGGER_QUEUE_CAPACITY + 3;
    log_queue_t *queue = argument;
    log_item_t item;

    for (size_t i = 1; i <= event_count; i++) {
        snprintf(item.message, sizeof(item.message),
                 "Producer enqueued demonstration event %zu", i);

        item.stop = 0;

        if (queue_push(queue, &item) != 0) {
            return &logger_thread_error;
        }
    }

    item.message[0] = '\0';
    item.stop = 1;

    if (queue_push(queue, &item) != 0) {
        return &logger_thread_error;
    }

    return NULL;
}

int run_logger_demo(void)
{
    log_queue_t queue = {0};
    pthread_t producer;
    pthread_t consumer;

    int mutex_ready = 0;
    int empty_ready = 0;
    int full_ready = 0;
    int consumer_ready = 0;
    int producer_ready = 0;

    int result;
    void *thread_result = NULL;
    int success = 1;

    result = pthread_mutex_init(&queue.mutex, NULL);

    if (result != 0) {
        fprintf(stderr, "Logger mutex initialization failed: %s\n",
                strerror(result));
        return 0;
    }

    mutex_ready = 1;

    result = pthread_cond_init(&queue.not_empty, NULL);

    if (result != 0) {
        fprintf(stderr, "Logger condition initialization failed: %s\n",
                strerror(result));
        success = 0;
        goto cleanup;
    }

    empty_ready = 1;

    result = pthread_cond_init(&queue.not_full, NULL);

    if (result != 0) {
        fprintf(stderr, "Logger condition initialization failed: %s\n",
                strerror(result));
        success = 0;
        goto cleanup;
    }

    full_ready = 1;

    result = pthread_create(&consumer, NULL, logger_consumer, &queue);

    if (result != 0) {
        fprintf(stderr, "Logger consumer creation failed: %s\n",
                strerror(result));
        success = 0;
        goto cleanup;
    }

    consumer_ready = 1;

    result = pthread_create(&producer, NULL, logger_producer, &queue);

    if (result != 0) {
        log_item_t stop_item = {{0}, 1};

        fprintf(stderr, "Logger producer creation failed: %s\n",
                strerror(result));

        success = 0;

        if (queue_push(&queue, &stop_item) != 0) {
            fprintf(stderr, "Logger could not signal consumer shutdown.\n");
        }
    } else {
        producer_ready = 1;
    }

    if (producer_ready) {
        result = pthread_join(producer, &thread_result);

        if (result != 0) {
            fprintf(stderr, "Logger producer join failed: %s\n",
                    strerror(result));
            success = 0;
        } else if (thread_result != NULL) {
            fprintf(stderr, "Logger producer encountered a synchronization error.\n");
            success = 0;
        }
    }

    if (consumer_ready) {
        result = pthread_join(consumer, &thread_result);

        if (result != 0) {
            fprintf(stderr, "Logger consumer join failed: %s\n",
                    strerror(result));
            success = 0;
        } else if (thread_result != NULL) {
            fprintf(stderr, "Logger consumer encountered an error.\n");
            success = 0;
        }
    }

cleanup:

    if (full_ready) {
        pthread_cond_destroy(&queue.not_full);
    }

    if (empty_ready) {
        pthread_cond_destroy(&queue.not_empty);
    }

    if (mutex_ready) {
        pthread_mutex_destroy(&queue.mutex);
    }

    if (success) {
        printf("Logger threads terminated and synchronization resources released.\n");
        printf("Log entries saved to campuscompute.log.\n");
    }

    return success;
}

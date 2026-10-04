/*
 * Lucas Seitz
 * CECS 326 - Programming Assignment 2
 *
 *   Creates thread_count worker threads (1-100). Each squares its ID and
 *   stores the result; main joins them and prints results in ID order.
 */

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN_THREADS 1
#define MAX_THREADS 100

/* Data passed to each worker: its ID and its own result slot. */
typedef struct {
    int id;
    long result;
} thread_data_t;

/* Worker thread function. */
static void *worker(void *arg)
{
    thread_data_t *data = (thread_data_t *)arg;

    printf("[thread %d] started\n", data->id);

    /* Each thread writes only to its own slot, so no locking is needed. */
    data->result = (long)data->id * data->id;

    return NULL;
}

static void print_usage(const char *prog)
{
    fprintf(stderr, "Usage: %s <thread_count>\n", prog);
    fprintf(stderr, "  thread_count must be an integer from %d through %d\n",
            MIN_THREADS, MAX_THREADS);
}

/*
 * Parses str as a thread count. Returns 0 on success and stores the value
 * in *out; returns -1 if str is not a whole integer in the valid range.
 */
static int parse_thread_count(const char *str, int *out)
{
    char *end = NULL;
    long value;

    errno = 0;
    value = strtol(str, &end, 10);

    if (end == str || *end != '\0') {
        fprintf(stderr, "Error: '%s' is not a valid integer.\n", str);
        return -1;
    }
    if (errno == ERANGE || value < MIN_THREADS || value > MAX_THREADS) {
        fprintf(stderr, "Error: thread_count must be between %d and %d (got '%s').\n",
                MIN_THREADS, MAX_THREADS, str);
        return -1;
    }

    *out = (int)value;
    return 0;
}

int main(int argc, char *argv[])
{
    int thread_count = 0;
    int created = 0;
    int exit_status = EXIT_SUCCESS;
    int rc;
    int i;
    pthread_t *threads = NULL;
    thread_data_t *data = NULL;

    /* 1. Command-line input */
    if (argc != 2) {
        fprintf(stderr, "Error: expected exactly one argument.\n");
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }
    if (parse_thread_count(argv[1], &thread_count) != 0) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    /* Allocate thread handles and per-thread data/result slots. */
    threads = calloc((size_t)thread_count, sizeof(*threads));
    data = calloc((size_t)thread_count, sizeof(*data));
    if (threads == NULL || data == NULL) {
        fprintf(stderr, "Error: memory allocation failed.\n");
        free(threads);
        free(data);
        return EXIT_FAILURE;
    }

    /* 2. Thread creation */
    printf("[main] creating %d threads\n", thread_count);
    for (i = 0; i < thread_count; i++) {
        data[i].id = i + 1;
        data[i].result = 0;

        rc = pthread_create(&threads[i], NULL, worker, &data[i]);
        if (rc != 0) {
            fprintf(stderr, "Error: pthread_create failed for thread %d: %s\n",
                    i + 1, strerror(rc));
            exit_status = EXIT_FAILURE;
            break; /* stop creating; still join the ones already running */
        }
        created++;
    }

    /* 4. Join every thread that was successfully created */
    for (i = 0; i < created; i++) {
        rc = pthread_join(threads[i], NULL);
        if (rc != 0) {
            fprintf(stderr, "Error: pthread_join failed for thread %d: %s\n",
                    i + 1, strerror(rc));
            exit_status = EXIT_FAILURE;
        }
    }

    /* 5. Final output, in thread-ID order */
    if (exit_status == EXIT_SUCCESS) {
        for (i = 0; i < thread_count; i++) {
            printf("[main] thread %d result: %ld\n", data[i].id, data[i].result);
        }
        printf("[main] completed successfully\n");
    } else {
        fprintf(stderr, "[main] completed with errors\n");
    }

    /* 6. Cleanup */
    free(threads);
    free(data);

    return exit_status;
}
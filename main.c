#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "barrier.h"

typedef struct {
    int id;
    int etapas;
    barrier_t *barrera;
} thread_arg_t;

void *worker(void *arg) {
    thread_arg_t *t = (thread_arg_t *)arg;

    unsigned int seed = (unsigned int)(time(NULL) ^ (t->id * 1234567));

    for (int e = 0; e < t->etapas; e++) {
        int delay_ms = (rand_r(&seed) % 400) + 100; 
        usleep(delay_ms * 1000);

        printf("[thread %d] esperando en etapa %d\n", t->id, e);
        fflush(stdout);

        barrier_wait(t->barrera);

        printf("[thread %d] paso barrera en etapa %d\n", t->id, e);
        fflush(stdout);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    int N = 5; 
    int E = 4; 

    if (argc >= 2) {
        N = atoi(argv[1]);
    }
    if (argc >= 3) {
        E = atoi(argv[2]);
    }

    if (N <= 0 || E <= 0) {
        fprintf(stderr, "Uso: %s [N_hebras > 0] [E_etapas > 0]\n", argv[0]);
        return 1;
    }

    printf("%d hebras y %d etapas\n", N, E);

    barrier_t barrera;
    if (barrier_init(&barrera, N) != 0) {
        fprintf(stderr, "Error al inicializar la barrera\n");
        return 1;
    }

    pthread_t *threads = malloc(N * sizeof(pthread_t));
    thread_arg_t *args = malloc(N * sizeof(thread_arg_t));
    if (threads == NULL || args == NULL) {
        perror("malloc");
        barrier_destroy(&barrera);
        free(threads);
        free(args);
        return 1;
    }
    for (int i = 0; i < N; i++) {
        args[i].id = i;
        args[i].etapas = E;
        args[i].barrera = &barrera;

        int rc = pthread_create(&threads[i], NULL, worker, &args[i]);
        if (rc != 0) {
            fprintf(stderr, "Error al crear la hebra %d (codigo %d)\n", i, rc);
        }
    }

    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    barrier_destroy(&barrera);
    free(threads);
    free(args);

    return 0;
}

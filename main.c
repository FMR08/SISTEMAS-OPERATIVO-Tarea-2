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

    srand(time(NULL) ^ (t->id * 1234567));

    for (int e = 0; e < t->etapas; e++) {
        int delay_ms = rand() % 100;
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
        fprintf(stderr, "Uso: %s [N_hebras] [E_etapas]\n", argv[0]);
        return EXIT_FAILURE;
    }

    printf("Ejecutando con N = %d hebras, E = %d etapas\n", N, E);

    barrier_t barrera;
    if (barrier_init(&barrera, N) != 0) {
        fprintf(stderr, "Error al inicializar la barrera\n");
        return EXIT_FAILURE;
    }

    pthread_t *threads = malloc(sizeof(pthread_t) * N);
    thread_arg_t *args = malloc(sizeof(thread_arg_t) * N);
    if (!threads || !args) {
        fprintf(stderr, "Error de memoria\n");
        barrier_destroy(&barrera);
        free(threads);
        free(args);
        return EXIT_FAILURE;
    }
    for (int i = 0; i < N; i++) {
        args[i].id = i;
        args[i].etapas = E;
        args[i].barrera = &barrera;

        if (pthread_create(&threads[i], NULL, worker, &args[i]) != 0) {
            fprintf(stderr, "Error al crear la hebra %d\n", i);
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

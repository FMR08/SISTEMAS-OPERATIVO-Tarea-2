#include "barrier.h"
#include <stdlib.h>

int barrier_init(barrier_t *b, int N) {
    if (b == NULL || N <= 0) {
        return -1;
    }

    b->count = 0;
    b->N = N;
    b->etapa = 0;

    if (pthread_mutex_init(&b->mtx, NULL) != 0) {
        return -1;
    }

    if (pthread_cond_init(&b->cond, NULL) != 0) {
        pthread_mutex_destroy(&b->mtx);
        return -1;
    }

    return 0;
}

void barrier_wait(barrier_t *b) {
    pthread_mutex_lock(&b->mtx);
    int mi_etapa = b->etapa;
    b->count++;
    if (b->count == b->N) {
        b->etapa++;
        b->count = 0;
        pthread_cond_broadcast(&b->cond); 
    } else {
        while (mi_etapa == b->etapa) {
            pthread_cond_wait(&b->cond, &b->mtx);
        }
    }

    pthread_mutex_unlock(&b->mtx);
}

void barrier_destroy(barrier_t *b) {
    pthread_mutex_destroy(&b->mtx);
    pthread_cond_destroy(&b->cond);
}

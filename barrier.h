#ifndef BARRIER_H
#define BARRIER_H

#include <pthread.h>

typedef struct {
    int count;
    int N;
    int etapa;
    pthread_mutex_t mtx;
    pthread_cond_t cond;
} barrier_t;

int barrier_init(barrier_t *b, int N);
void barrier_wait(barrier_t *b);
void barrier_destroy(barrier_t *b);

#endif 

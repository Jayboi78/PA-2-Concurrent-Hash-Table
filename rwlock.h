#ifndef RWLOCKS_H
#define RWLOCKS_H

#include <semaphore.h>
#include  <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "common.h"
#include "common_threads.h"

typedef struct {
    int readers;
    int acquired;
    int released;
    sem_t lock;
    sem_t writelock;
} rwlock_t;

void rwlock_init(rwlock_t *lock);
void rwlock_acquire_readlock(rwlock_t *lock);
void rwlock_release_readlock(rwlock_t *lock);
void rwlock_acquire_writelock(rwlock_t *lock);
void rwlock_release_writelock(rwlock_t *lock);

#endif /* RWLOCKS_H */

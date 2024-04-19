#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "rwlocks.h"
#include "common.h"
#include "common_threads.h"
#include <semaphore.h>

void rwlock_init(rwlock_t *lock) {
    lock->readers = 0;
    lock->acquired = 0;
    lock->released = 0;
    Sem_init(&lock->lock, 1); 
    Sem_init(&lock->writelock, 1); 
}

void rwlock_acquire_readlock(rwlock_t *lock) {
    Sem_wait(&lock->lock);
    lock->readers++;
    lock->acquired++;
    if (lock->readers == 1)
	    Sem_wait(&lock->writelock);
    Sem_post(&lock->lock);
}

void rwlock_release_readlock(rwlock_t *lock) {
    Sem_wait(&lock->lock);
    lock->readers--;
    lock->released++;
    if (lock->readers == 0)
	    Sem_post(&lock->writelock);
    Sem_post(&lock->lock);
}

void rwlock_acquire_writelock(rwlock_t *lock) {
    lock->acquired++;
    Sem_wait(&lock->writelock);
}

void rwlock_release_writelock(rwlock_t *lock) {
    lock->released++;
    Sem_post(&lock->writelock);
}

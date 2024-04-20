#ifndef RWLOCKS_H_
#define RWLOCKS_H_

#include <semaphore.h>

typedef struct _rwlock_t {
    int readers;
    int acquired;
    int released;
    sem_t writelock;
    sem_t lock;
} rwlock_t;

// Function prototypes
void rwlock_init(rwlock_t *lock);
void rwlock_acquire_readlock(rwlock_t *lock);
void rwlock_release_readlock(rwlock_t *lock);
void rwlock_acquire_writelock(rwlock_t *lock);
void rwlock_release_writelock(rwlock_t *lock);

#endif
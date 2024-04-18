#include "hashdb.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

int pthread_create(pthread_t *restrict tid, const pthread_attr_t 
    *restrict tattr, void*(*start_routine)(void *), void *restrict arg);

pthread_attr_t() tattr;
pthread_t tid;
extern void *start_routine(void *arg);
void *arg;
int ret; 


// Define your hash table structure
typedef struct {
    pthread_t lock; // Reader-Writer lock for synchronization
    hashRecord *head; // Head of the linked list
} ConcurrentHashMap;

// Global hash table instance
ConcurrentHashMap hash_table;

// Initialize the hash table
void init_hash_table() {
    hash_table.head = NULL;
    /* default behavior*/
  ret = pthread_create(&tid, NULL, start_routine, arg);
}

//Creating insert function where we insert or update the data for the given name and salary value.
void insert(const char *name, int salary) {
    // Compute the hash value
    uint32_t hash = jenkins_one_at_a_time_hash((uint8_t *)name, strlen(name));
    // Lock the hash table
    pthread_mutex_lock(&hash_table.lock);
    // Search for the record
    hashRecord *current = hash_table.head;
    hashRecord *prev = NULL;
    while (current != NULL) {
        if (current->hash == hash && strcmp(current->name, name) == 0) {
            // Update the salary
            current->salary = salary;
            // Unlock the hash table
            pthread_mutex_unlock(&hash_table.lock);
            return;
        }
        prev = current;
        current = current->next;
    }
    // Create a new record
    hashRecord *new_record = (hashRecord *)malloc(sizeof(hashRecord));
    new_record->hash = hash;
    new_record->name = name;
    new_record->salary = salary;
    new_record->next = NULL;
    // Insert the new record
    if (prev == NULL) {
        hash_table.head = new_record;
    } else {
        prev->next = new_record;
    }
    // Unlock the hash table
    pthread_mutex_unlock(&hash_table.lock);
}


// Function to delete a record from the hash table based on the name
void delete(const char *name) {
    // Compute the hash value
    uint32_t hash = jenkins_one_at_a_time_hash((uint8_t *)name, strlen(name));
    // Lock the hash table
    pthread_mutex_lock(&hash_table.lock);
    // Search for the record
    hashRecord *current = hash_table.head;
    hashRecord *prev = NULL;
    while (current != NULL) {
        if (current->hash == hash && strcmp(current->name, name) == 0) {
            // Remove the record
            if (prev == NULL) {
                // If the record is the head of the list
                hash_table.head = current->next;
            } else {
                prev->next = current->next;
            }
            // Free the memory
            free(current);
            // Unlock the hash table
            pthread_mutex_unlock(&hash_table.lock);
            return;
        }
        prev = current;
        current = current->next;
    }
    // If the record is not found
    printf("Record with name %s not found.\n", name);
    // Unlock the hash table
    pthread_mutex_unlock(&hash_table.lock);
}

// Function to search for a record in the hash table
uint32_t search_record(const char *name) {
    pthread_rwlock_rdlock(&hash_table.lock);

    hashRecord *current = hash_table.head;

    while (current != NULL && strcmp(current->name, name) != 0) {
        current = current->next;
    }

    uint32_t salary = 0;
    if (current != NULL && strcmp(current->name, name) == 0) {
        salary = current->salary;
    }

    pthread_rwlock_unlock(&hash_table.lock);

    return salary;
}


//Jenkins Function
uint32_t jenkins_one_at_a_time_hash(const uint8_t* key, size_t length) {
  size_t i = 0;
  uint32_t hash = 0;
  while (i != length) {
    hash += key[i++];
    hash += hash << 10;
    hash ^= hash >> 6;
  }
  hash += hash << 3;
  hash ^= hash >> 11;
  hash += hash << 15;
  return hash;
}
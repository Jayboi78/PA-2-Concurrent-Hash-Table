#include "hashdb.h"
#include "rwlocks.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern rwlock_t list_lock;
rwlock_t list_lock;

hashRecord hash_table;

FILE *fOut;

void *searchRecord(void *arg) {
    char *key = (char *)arg;
    rwlock_acquire_readlock(&list_lock);
    hashRecord *temp = hash_table.next;
    while (temp != NULL) {
        if (strcmp(temp->name, key) == 0) {
            fprintf(fOut, "Record found: %s, %d\n", temp->name, temp->salary);
            break;
        }
        temp = temp->next;
    }
    rwlock_release_readlock(&list_lock);
    pthread_exit(NULL);
}

void *deleteRecord(void *arg) {
    char *key = (char *)arg;
    rwlock_acquire_writelock(&list_lock);
    hashRecord *prev = &hash_table;
    hashRecord *curr = hash_table.next;
    while (curr != NULL) {
        if (strcmp(curr->name, key) == 0) {
            prev->next = curr->next;
            free(curr);
            fprintf(fOut, "Record deleted: %s\n", key);
            break;
        }
        prev = curr;
        curr = curr->next;
    }
    rwlock_release_writelock(&list_lock);
    pthread_exit(NULL);
}

uint32_t jenkins_one_at_a_time_hash(const uint8_t *key, size_t length) {
    uint32_t hash = 0;
    uint32_t i;

    for (i = 0; i < length; ++i) {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

void *insert(void *arg) {
    thread_insert_args *args = arg;
    uint32_t temp_hash = jenkins_one_at_a_time_hash(args->name, strlen(args->name));
    rwlock_acquire_writelock(&list_lock);
    fprintf(fOut, "INSERT, %u, %s, %d\n", temp_hash, args->name, args->salary);
    fprintf(fOut, "WRITE LOCK AQUIRED\n");
    hashRecord *newRecord = (hashRecord *)malloc(sizeof(hashRecord));
    strcpy(newRecord->name, args->name);
    newRecord->salary = args->salary;
    newRecord->hash = temp_hash;
    newRecord->next = hash_table.next;
    hash_table.next = newRecord;
    rwlock_release_writelock(&list_lock);
    fprintf(fOut, "WRITE LOCK RELEASED\n");
    pthread_exit(NULL);
}

void *delete(char *key) {
    fprintf(fOut, "DELETE, %s\n",key);
    // Verify that the name is in the list
    rwlock_acquire_readlock(&list_lock);
    fprintf(fOut, "READ LOCK ACQUIRED\n");
    fprintf(fOut, "SEARCH, %s\n",key);
    hashRecord *temp = hash_table.next;
    while (temp != NULL) {
        if (strcmp(temp->name, key) == 0) {
            break;
        }
        temp = temp->next;
    }
    if(temp == NULL){
        rwlock_release_readlock(&list_lock);
        fprintf(fOut, "NAME NOT FOUND");
        pthread_exit(NULL);
        return 0;
    }
    fprintf(fOut, "%u, %s, %d\n", temp->hash, temp->name, temp->salary);
    rwlock_release_readlock(&list_lock);
    fprintf(fOut, "READ LOCK RELEASED\n");
    
    // Name was found in the hash table, therefore delete it
    rwlock_acquire_writelock(&list_lock);
    fprintf(fOut, "WRITE LOCK ACQUIRED\n");
    hashRecord *prev = &hash_table;
    hashRecord *curr = hash_table.next;
    while (curr != NULL) {
        if (strcmp(curr->name, key) == 0) {
            prev->next = curr->next;
            free(curr);
            break;
        }
        prev = curr;
        curr = curr->next;
    }
    rwlock_release_writelock(&list_lock);
    fprintf(fOut, "WRITE LOCK RELEASED\n");
    pthread_exit(NULL);
}

void *search(char *key) {
    fprintf(fOut, "SEARCH, %s\n",key);
    rwlock_acquire_readlock(&list_lock);
    fprintf(fOut, "READ LOCK ACQUIRED\n");
    hashRecord *temp = hash_table.next;
    while (temp != NULL) {
        if (strcmp(temp->name, key) == 0) {
            break;
        }
        temp = temp->next;
    }
    if(temp == NULL){
        fprintf(fOut, "NAME NOT FOUND");
    } else {
        fprintf(fOut, "%u, %s, %d\n", temp->hash, temp->name, temp->salary);
    }
    rwlock_release_readlock(&list_lock);
    fprintf(fOut, "READ LOCK RELEASED\n");
    pthread_exit(NULL);
}

void *print() {
    rwlock_acquire_readlock(&list_lock);
    fprintf(fOut, "READ LOCK ACQUIRED\n");
    hashRecord *temp = hash_table.next;
    while (temp != NULL) {
        fprintf(fOut, "%u, %s, %d\n", temp->hash, temp->name, temp->salary);
        temp = temp->next;
    }
    rwlock_release_readlock(&list_lock);
    fprintf(fOut, "READ LOCK RELEASED\n");
    pthread_exit(NULL);
}

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

// This is the hash function given to us in the assignment instructions
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

// Function to generate a hash and insert a new record into the hash table
void *insert(void *arg) {
    thread_insert_args *args = arg;
    uint32_t temp_hash = jenkins_one_at_a_time_hash(args->name, strlen(args->name));

    // Aquire write lock
    rwlock_acquire_writelock(&list_lock);
    fprintf(fOut, "INSERT, %u, %s, %d\n", temp_hash, args->name, args->salary);
    fprintf(fOut, "WRITE LOCK AQUIRED\n");

    hashRecord *newRecord = (hashRecord *)malloc(sizeof(hashRecord));

    // Insert new record into the hash table
    strcpy(newRecord->name, args->name);
    newRecord->salary = args->salary;
    newRecord->hash = temp_hash;
    newRecord->next = hash_table.next;
    hash_table.next = newRecord;

    // Release write lock
    rwlock_release_writelock(&list_lock);
    fprintf(fOut, "WRITE LOCK RELEASED\n");
    pthread_exit(NULL);
}

// Function that finds a record in the hash table through comparing the name, or key,
// outputs the record, and removed it from the table
void *delete(char *key) {
    fprintf(fOut, "DELETE, %s\n",key);

    // Search for the name and verify it is in the table
    // Aquire read lock
    rwlock_acquire_readlock(&list_lock);
    fprintf(fOut, "READ LOCK ACQUIRED\n");
    fprintf(fOut, "SEARCH, %s\n",key);
    hashRecord *temp = hash_table.next;

    // traverse the table searching for a match to the key
    while (temp != NULL) {
        if (strcmp(temp->name, key) == 0) {
            break;
        }
        temp = temp->next;
    }

    // If temp becomes NULL the key was not found in the table 
    // Output error message, release the read lock, and exit the function
    if(temp == NULL){
        rwlock_release_readlock(&list_lock);
        fprintf(fOut, "NAME NOT FOUND");
        pthread_exit(NULL);
        return 0;
    }

    // If the record is found output the record and release the lock
    fprintf(fOut, "%u, %s, %d\n", temp->hash, temp->name, temp->salary);
    
    // release the read lock
    rwlock_release_readlock(&list_lock);
    fprintf(fOut, "READ LOCK RELEASED\n");
    
    // Name was found in the hash table, therefore delete it
    // Aquire the write lock
    rwlock_acquire_writelock(&list_lock);
    fprintf(fOut, "WRITE LOCK ACQUIRED\n");

    // Traverse the table again to aquire the pointer of the record 
    // to be deleted and remove it from the linked list
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

    // release the write lock
    rwlock_release_writelock(&list_lock);
    fprintf(fOut, "WRITE LOCK RELEASED\n");
    pthread_exit(NULL);
}

// Function that searches the hash table and outputs the 
// desired record
void *search(char *key) {
    fprintf(fOut, "SEARCH, %s\n",key);

    // Aquire the read lock
    rwlock_acquire_readlock(&list_lock);
    fprintf(fOut, "READ LOCK ACQUIRED\n");
    
    // Traverse the table until the record is found or 
    // the end is reached
    hashRecord *temp = hash_table.next;
    while (temp != NULL) {
        if (strcmp(temp->name, key) == 0) {
            break;
        }
        temp = temp->next;
    }

    // Change the output message depending on if the record was found
    if(temp == NULL){
        fprintf(fOut, "NAME NOT FOUND");
    } else {
        fprintf(fOut, "%u, %s, %d\n", temp->hash, temp->name, temp->salary);
    }

    // Release the read lock
    rwlock_release_readlock(&list_lock);
    fprintf(fOut, "READ LOCK RELEASED\n");
    pthread_exit(NULL);
}

// Output the entire hash table
void *print() {
    // Aquire the read lock
    rwlock_acquire_readlock(&list_lock);
    fprintf(fOut, "READ LOCK ACQUIRED\n");
    
    // Traverse the table outputing each record
    hashRecord *temp = hash_table.next;
    while (temp != NULL) {
        fprintf(fOut, "%u, %s, %d\n", temp->hash, temp->name, temp->salary);
        temp = temp->next;
    }

    // Release the read lock
    rwlock_release_readlock(&list_lock);
    fprintf(fOut, "READ LOCK RELEASED\n");
    pthread_exit(NULL);
}

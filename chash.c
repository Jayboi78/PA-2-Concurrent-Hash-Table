#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "hashdb.h" 
#include "rwlocks.h"

#define MAX_LINE_LENGTH 100
#define OUTPUT_FILE "output.txt"

typedef struct {
    char operation[10];
    char name[50];
    int salary;
} Record;

rwlock_t rwlock;

void *performOperation(void *arg) {
    Record *record = (Record *)arg;

    rwlock_rdlock(&rwlock); // Acquire reader lock

    if (strcmp(record->operation, "print") == 0) {
        print_table();
    } else {
        rwlock_wrlock(&rwlock); // Acquire writer lock

        if (strcmp(record->operation, "insert") == 0) {
            insert_record(record->name, record->salary);
        } else if (strcmp(record->operation, "search") == 0) {
            uint32_t salary = search_record(record->name);
            FILE *output_file = fopen(OUTPUT_FILE, "a");
            if (output_file == NULL) {
                fprintf(stderr, "Error: Could not open file %s\n", OUTPUT_FILE);
                exit(EXIT_FAILURE);
            }
            if (salary != 0) {
                fprintf(output_file, "SEARCH,%s,%u\n", record->name, salary);
            } else {
                fprintf(output_file, "SEARCH,%s,No Record Found\n", record->name);
            }
            fclose(output_file);
        } else if (strcmp(record->operation, "delete") == 0) {
            delete_record(record->name);
        } else {
            printf("Invalid operation: %s\n", record->operation);
        }

        rwlock_unlock(&rwlock); // Release writer lock
    }

    rwlock_unlock(&rwlock); // Release reader lock

    pthread_exit(NULL);
}

void readFromFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }

    char line[MAX_LINE_LENGTH];
    pthread_t threads[MAX_LINE_LENGTH];
    int thread_count = 0;

    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",");
        Record *record = (Record *)malloc(sizeof(Record));
        if (record == NULL) {
            fprintf(stderr, "Error: Memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }

        strcpy(record->operation, token);

        if (strcmp(record->operation, "print") != 0) {
            token = strtok(NULL, ",");
            strcpy(record->name, token);
            token = strtok(NULL, ",");
            record->salary = atoi(token);

            pthread_create(&threads[thread_count++], NULL, performOperation, (void *)record);
        } else {
            performOperation((void *)record);
            free(record);
        }
    }

    fclose(file);

    // Wait for all threads to finish
    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }
}

int main() {
    rwlock_init(&rwlock); // Initialize reader-writer lock
    init_hash_table(); // Initialize the hash table

    readFromFile("commands.txt");

    return 0;
}

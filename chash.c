#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "hashdb.h" 

#define MAX_LINE_LENGTH 100

typedef struct {
    char operation[10];
    char name[50];
    int salary;
} Record;

void *performOperation(void *arg) {
    Record *record = (Record *)arg;

    if (strcmp(record->operation, "print") == 0) {
        print_hash_table();
    } else {
        if (strcmp(record->operation, "insert") == 0) {
            insert(record->name, record->salary);
        } else if (strcmp(record->operation, "search") == 0) {
            search(record->name);
        } else if (strcmp(record->operation, "delete") == 0) {
            delete(record->name);
        } else {
            printf("Invalid operation: %s\n", record->operation);
        }
    }

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
    readFromFile("data.txt");
    return 0;
}

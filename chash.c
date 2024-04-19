#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "hashdb.h"
#include "rwlocks.h"

extern rwlock_t list_lock;
extern FILE *fOut;

typedef struct action {
    char cmd[50];
    char key[50];
    char value[50];
} action;

int main() {
    FILE *input;
    FILE *output;

    // Open commands.txt for reading
    char inputFile[] = "commands.txt";
    input = fopen(inputFile, "r");

    // Create file for output.txt
    char outputFile[] = "output.txt";
    output = fopen(outputFile, "w");
    fOut = output;

    // Verify file opening
    if (input == NULL) {
        printf("Error: Unable to open input file %s\n!", inputFile);
        return 1;
    }

    if (output == NULL) {
        printf("Error: Unable to open output file %s\n!", outputFile);
        return 1;
    }

    // Buffers for reading commands
    char command[100];
    char param1[100];
    char param2[100];

    int totalThreads = 0;
    int cmdCount = 0;
    int insertCount = 0;
    int insertArgsIdx = 0;

    // Thread and action structures
    pthread_t *threadList = NULL;
    thread_insert_args *insertParams = NULL;
    action *cmdList = NULL;

    // Initialize the reader-writer lock
    rwlock_init(&list_lock);

    // Read commands from input file and store them
    while (fscanf(input, "%[^,\n]%*c", command) == 1) {
        switch (command[0]) {
            case 't':
                fscanf(input, "%[^,\n]%*c", param1);
                fscanf(input, "%[^,\n]%*c", param2);

                // Set up action and thread lists
                totalThreads = atoi(param1);
                cmdList = calloc(totalThreads, sizeof(action));
                threadList = malloc(sizeof(pthread_t) * totalThreads);
                fprintf(fOut, "Running %d threads\n", totalThreads);
                break;
            case 'i':
                fscanf(input, "%[^,\n]%*c", param1);
                fscanf(input, "%[^,\n]%*c", param2);

                strcpy(cmdList[cmdCount].cmd, command);
                strcpy(cmdList[cmdCount].key, param1);
                strcpy(cmdList[cmdCount].value, param2);
                insertCount++;
                break;
            default:
                fscanf(input, "%[^,\n]%*c", param1);
                fscanf(input, "%[^,\n]%*c", param2); // Unused parameters

                strcpy(cmdList[cmdCount].cmd, command);
                strcpy(cmdList[cmdCount].key, param1);
                strcpy(cmdList[cmdCount].value, param2);
                break;
        }

        cmdCount++;
    }

    // Allocate memory for insert parameters
    insertParams = calloc(insertCount, sizeof(thread_insert_args));

    // Process each command into threads
    for (int i = 0; i < totalThreads; i++) {
        switch (cmdList[i].cmd[0]) {
            case 'i':
                insertParams[insertArgsIdx].name = strdup(cmdList[i].key);
                insertParams[insertArgsIdx].salary = atoi(cmdList[i].value);
                pthread_create(&threadList[i], NULL, insert, &insertParams[insertArgsIdx]);
                insertArgsIdx++;
                break;
            case 'd':
                pthread_create(&threadList[i], NULL, delete, cmdList[i].key);
                break;
            case 's':
                pthread_create(&threadList[i], NULL, search, cmdList[i].key);
                break;
            case 'p':
                pthread_create(&threadList[i], NULL, print, NULL);
                break;
        }
    }

    // Join the threads
    for (int i = 0; i < totalThreads; i++) {
        pthread_join(threadList[i], NULL);
    }

    // Output lock statistics
    fprintf(fOut, "Number of lock acquisitions: %d\n", list_lock.acquired);
    fprintf(fOut, "Number of lock releases: %d\n", list_lock.released);

    // Print final hash table
    print();

    // Clean up and close files
    fclose(input);
    fclose(output);

    return 0;
}

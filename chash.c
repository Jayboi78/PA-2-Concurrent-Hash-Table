#include "hashdb.h"
#include "rwlocks.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

//External declarations
extern rwlock_t list_lock;
extern FILE *fOut;

//Structure to hold command details
typedef struct commandetails {
    char cmd[50];
    char key[50];
    char value[50];
} commanddetails;

int main() {
    //File pointers for input and output
    FILE *input, *output;

    //Open input file for reading
    char inputfile[] = "commands.txt";
    input = fopen(inputfile, "r");

    // Create output file for writing
    char outputfile[] = "output.txt";
    output = fopen(outputfile, "w");
    fOut = output;

    // Verify file opening
    if (input == NULL) {
        printf("Error: Unable to open input file %s\n!", inputfile);
        return 1;
    }

    if (output == NULL) {
        printf("Error: Unable to open output file %s\n!", outputfile);
        return 1;
    }

    // Buffers for reading commands
    char command[100];
    char parambuffer1[100];
    char parambuffer2[100];

    // Variables for tracking threads and commands
    int allThreads = 0;
    int Commandfilecount = 0;
    int countinsertion = 0;
    int insertargsindex = 0;

    // Thread and commanddetails structures
    pthread_t *threadList = NULL;
    thread_insert_args *insertParams = NULL;
    commanddetails *Commandfilelist = NULL;

    // Initialize the reader-writer lock
    rwlock_init(&list_lock);

    // Read commands from input file and store them
    while (fscanf(input, "%[^,\n]%*c", command) == 1) {
        switch (command[0]) {
            case 't':
                //Store number of threads
                fscanf(input, "%[^,\n]%*c", parambuffer1);
                fscanf(input, "%[^,\n]%*c", parambuffer2);

                
                allThreads = atoi(parambuffer1);
                Commandfilelist = calloc(allThreads, sizeof(commanddetails));
                threadList = malloc(sizeof(pthread_t) * allThreads);
                fprintf(fOut, "Running %d threads\n", allThreads);
                break;
            case 'i':
                //Store insert command's details
                fscanf(input, "%[^,\n]%*c", parambuffer1);
                fscanf(input, "%[^,\n]%*c", parambuffer2);

                strcpy(Commandfilelist[Commandfilecount].cmd, command);
                strcpy(Commandfilelist[Commandfilecount].key, parambuffer1);
                strcpy(Commandfilelist[Commandfilecount].value, parambuffer2);
                countinsertion++;
                break;
            default:
                //Store other command's details
                fscanf(input, "%[^,\n]%*c", parambuffer1);
                fscanf(input, "%[^,\n]%*c", parambuffer2);

                strcpy(Commandfilelist[Commandfilecount].cmd, command);
                strcpy(Commandfilelist[Commandfilecount].key, parambuffer1);
                strcpy(Commandfilelist[Commandfilecount].value, parambuffer2);
                break;
        }

        Commandfilecount++;
    }

    // Allocate memory for when we have to the insert parameters
    insertParams = calloc(countinsertion, sizeof(thread_insert_args));

    // Process each command into threads
    for (int i = 0; i < allThreads; i++) {
        switch (Commandfilelist[i].cmd[0]) {
            case 'i':
                insertParams[insertargsindex].name = strdup(Commandfilelist[i].key);
                insertParams[insertargsindex].salary = atoi(Commandfilelist[i].value);
                pthread_create(&threadList[i], NULL, insert, &insertParams[insertargsindex]);
                insertargsindex++;
                break;
            case 'd':
                pthread_create(&threadList[i], NULL, delete, Commandfilelist[i].key);
                break;
            case 's':
                pthread_create(&threadList[i], NULL, search, Commandfilelist[i].key);
                break;
            case 'p':
                pthread_create(&threadList[i], NULL, print, NULL);
                break;
        }
    }

    //For loop to join the threads
    for (int i = 0; i < allThreads; i++) {
        pthread_join(threadList[i], NULL);
    }

    // Output lock information
    fprintf(fOut, "Number of lock acquisitions: %d\n", list_lock.acquired);
    fprintf(fOut, "Number of lock releases: %d\n", list_lock.released);

    // This prints out the final hash table
    print();

    // Closes the input and output files
    fclose(input);
    fclose(output);

    return 0;
}

#ifndef __hashdb_h__
#define __hashdb_h__

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct hash_struct {
    uint32_t hash;
    char name[50];
    uint32_t salary;
    struct hash_struct *next;
} hashRecord;

typedef struct thread_insert_args {
    char *name;
    int salary;
} thread_insert_args;

void *searchRecord(void *arg);
void *deleteRecord(void *arg);

uint32_t jenkins_one_at_a_time_hash(const uint8_t *key, size_t length);

void *insert(void *arg);

void *delete(char *key);

void *search(char *key);

void *print();

#endif

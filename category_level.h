#ifndef CATEGORY_LEVEL_H
#define CATEGORY_LEVEL_H
#include "semaphore.h"
#include "structs.h"

#define TERMINATION "TERMINATE"
#define SUCCESS "SUCCESS"

sem_t *log;
sem_t *put_result;
sem_t *sem_process;
sem_t *sem_thread;
recipt rcpt;
char *end_massage;
double *scores;


#endif
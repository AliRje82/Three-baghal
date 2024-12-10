#ifndef CATEGORY_LEVEL_H
#define CATEGORY_LEVEL_H
#include "semaphore.h"
#include "structs.h"

#define TERMINATION "TERMINATE"
#define SUCCESS "SUCCESS"

extern sem_t log;
extern sem_t put_result;
extern sem_t sem_process;
extern sem_t sem_thread;
extern recipt rcpt;
extern char *end_massage;
extern double *scores;


#endif
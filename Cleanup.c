#include "semaphore.h"
#include "sys/mman.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "pthread.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

#define SEM_MUTEX "/sem_mutex"
#define SEM_WRITER "/sem_writer"
#define SEM_QUEUE "/sem_queue"
#define SHARED_INT_COUNT "/shared_memory"
#define SHARED_MEM_SIZE sizeof(int)
#define MAX_ITEMS 653
#define MAX_STORES 3

void clean_up();

int main(){
    clean_up();
}

void clean_up(){
    char buffer[1024];

    for (int i = 1; i <= MAX_STORES; i++)
    {
        for (int j = 1; j <= MAX_ITEMS; j++)
        { 
            sprintf(buffer,"%s%d%d",SEM_MUTEX,i,j);
            sem_unlink(buffer);

            sprintf(buffer,"%s%d%d",SEM_WRITER,i,j);
            sem_unlink(buffer);

            sprintf(buffer,"%s%d%d",SEM_QUEUE,i,j);
            sem_unlink(buffer);

        }
        
    }

}
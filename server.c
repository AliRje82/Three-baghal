#include "semaphore.h"
#include "sys/mman.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "pthread.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "user_level.h"

#define SEM_MUTEX "/sem_mutex"
#define SEM_WRITER "/sem_writer"
#define SEM_QUEUE "/sem_queue"
#define SHARED_INT_COUNT "/shared_memory"
#define SHARED_MEM_SIZE sizeof(int)
#define MAX_ITEMS 653
#define MAX_STORES 3

void init_semaphores();
void clean_up();



int main(){
    init_semaphores();
    
    user_level_process();

    clean_up();
}

void init_semaphores(){
    char buffer[1024];
    sem_t *mutex;
    sem_t *writer;
    sem_t *queue;
    int shmd;
    int *reader_count;

    for (int i = 1; i <= MAX_STORES; i++)
    {
        for (int j = 1; j <= MAX_ITEMS; j++)
        {
            /*
            ! init semaphores
            */
            sprintf(buffer,"%s%d%d",SEM_MUTEX,i,j);
            mutex = sem_open(buffer,O_CREAT,0644,1);

            sprintf(buffer,"%s%d%d",SEM_WRITER,i,j);
            writer = sem_open(buffer,O_CREAT,0644,1);

            sprintf(buffer,"%s%d%d",SEM_QUEUE,i,j);
            queue = sem_open(buffer,O_CREAT,0644,1);

            /*
            ! init Shared memory
            */
            sprintf(buffer,"%s%d%d",SHARED_INT_COUNT,i,j);
            shmd = shm_open(buffer,O_CREAT|O_RDWR,0666);
            if(ftruncate(shmd,SHARED_MEM_SIZE)==-1){
                printf("ERROR\n");
            }
            reader_count = mmap(NULL,SHARED_MEM_SIZE,PROT_READ | PROT_WRITE,MAP_SHARED,shmd,0);
            *reader_count =0;
           /*
           ! Clean up
           */
            munmap(reader_count,SHARED_MEM_SIZE);
            sem_close(mutex);
            sem_close(writer);
            sem_close(queue);
        }
        
    }
    

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
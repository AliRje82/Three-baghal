#ifndef CATEGORY_LEVEL_H
#define CATEGORY_LEVEL_H
#include "semaphore.h"
#include "structs.h"
#include "stdio.h"
#include "dirent.h"
#include "stdlib.h"
#include "string.h"
#include "pthread.h"
#include "sys/stat.h"
#include "sys/types.h"

#define TERMINATION "TERMINATE"
#define SUCCESS "SUCCESS"

sem_t *log;        // 1 when write in log file which is shared among thread
sem_t *put_result; // 1 when write in rcpt which is shared among thread
sem_t *sem_process;
sem_t *sem_thread;
recipt *rcpt;
char *end_massage;
double *scores;
const int dir_no = 3;
const int max_size = 1024;

void *file_process(void *arg)
{
    char *file_path = (char *)arg;
    printf("Thread created for file%s\n", file_path);
    pthread_exit(NULL);
}

void category_level_thread(int write_fd, int read_fd, char *path)
{
    rcpt = (recipt *)malloc(sizeof(recipt));
    char *msg;
    pthread_t thread[100];
    int thread_count = 0;
    char file_path[max_size];
    if (sem_init(sem_process, 0, 0) != 0)
    {
        perror("Create sem process failed");
        return;
    }
    if (sem_init(sem_thread, 0, 0) != 0)
    {
        perror("Create sem thread failed");
        return;
    }

    printf("open %s\n", path);
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        perror("Can not open directory\n");
        return;
    }
    struct dirent *entry;
    struct stat file_stat;
    while ((entry = readdir(dir)) != NULL)
    {
        snprintf(file_path, sizeof(file_path), "%s/%s", path, entry->d_name);
        if (stat(file_path, &file_stat) == 0)
        {
            if (S_ISREG(file_stat.st_mode))
            {
                if (strstr(entry->d_name, ".txt") != NULL)
                {
                    if (pthread_create(&thread[thread_count], NULL, file_process, file_path) != 0)
                    {
                        perror("Failed to create a thread\n");
                    }
                    else
                    {
                        thread_count++;
                    }
                }
            }
        }
    }
    closedir(dir);
    for (int i = 0; i < thread_count; i++)
    {
        sem_wait(sem_process);
    }
    msg = encode(rcpt);
    write(write_fd, msg, strlen(msg) + 1);
    char *buffer;
    read(read_fd, buffer, max_size);
    scores = decode(buffer);
    for (int i = 0; i < thread_count; i++)
    {
        sem_post(sem_thread);
    }
    if(strlen(scores) == 0) {
        end_massage = TERMINATION;
    }else {
        end_massage = SUCCESS;
    }
}

#endif
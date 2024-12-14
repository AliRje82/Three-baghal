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
#include "thread_level.h"
extern userInfo *user;

const char *SUCCESS= "SUCCESS";
const char *TERMINATION= "TERMINATE";

sem_t *logw;     
sem_t *put_result; 
sem_t *sem_process;
sem_t *sem_thread;
recipt *rcpt;
char *end_massage;
double *scores;
const int dir_no = 3;
const int max_size = 1024;

void *runner(void *args);

char *copy(char *str){
    char *string = malloc(strlen(str) * sizeof(char));
    strcpy(string,str);
    return string;
}


void category_level_thread(int write_fd, int read_fd, char *path)
{
    rcpt = (recipt *)malloc(sizeof(recipt));
    rcpt->n = 0;
    printf("User n is %d\n",user->n);
    rcpt->items = (item **)malloc(sizeof(item *) * user->n);
    char *msg;
    pthread_t thread[100];
    int thread_count = 0;
    sem_process = malloc(sizeof(sem_t));
    sem_thread = malloc(sizeof(sem_t));
    logw = malloc(sizeof(sem_t));
    put_result = malloc(sizeof(sem_t));
    if (sem_init(sem_process, 0, 0) != 0)
    {
        printf("Create sem process failed\n");
        return;
    }
    if (sem_init(sem_thread, 0, 0) != 0)
    {
        printf("Create sem thread failed\n");
        return;
    }
    if (sem_init(logw, 0, 1) != 0)
    {
        printf("Create sem log failed\n");
        return;
    }
    if (sem_init(put_result, 0, 1) != 0)
    {
        printf("Create sem put_result failed\n");
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
    char file_path[512];

    while ((entry = readdir(dir)) != NULL)
    {
        snprintf(file_path, sizeof(file_path), "%s/%s", path, entry->d_name);
        if (stat(file_path, &file_stat) == 0)
        {
            if (S_ISREG(file_stat.st_mode))
            {
                if (strstr(entry->d_name, ".txt") != NULL)
                {
                    if (pthread_create(&thread[thread_count], NULL, runner, copy(file_path)) != 0)
                    {
                        printf("Failed to create a thread\n");
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
    char buffer[1024];
    read(read_fd, buffer, sizeof(buffer));
    if(strlen(buffer) == 1) {
        end_massage = TERMINATION;
        printf("Terminate!\n");
    }else {
        end_massage = SUCCESS;
        scores = decode_score(buffer, user->n);
    }
    for (int i = 0; i < thread_count; i++)
    {
        sem_post(sem_thread);
    }
    printf("Ending on Category! %s\n",path);
}

#endif
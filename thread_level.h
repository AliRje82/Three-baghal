#ifndef THREAD_LEVEL_H
#define THREAD_LEVEL_H

#include "stdio.h"
#include "stdlib.h"
#include "semaphore.h"
#include "string.h"
#include "structs.h"
#include "pthread.h"
#include <unistd.h>
#include <time.h>
#include "user_level.h"
#include "category_level.h"
#include "sys/mman.h"
#include "sys/stat.h"
#include "fcntl.h"


#define SEM_MUTEX "/sem_mutex"
#define SEM_WRITER "/sem_writer"
#define SEM_QUEUE "/sem_queue"
#define SHARED_INT_COUNT "/shared_memory"
#define SHARED_MEM_SIZE sizeof(int)
#define MAX_LINE_LEN 100

/*
Function defines:
*/
void write_log(char *path,char *massage);
int check_forHit(item *it,char *path);
void edit_file(char *path,double user_score,int user_wanted);
void writer_problem(char *path,sem_t *queue,sem_t *write,double user_score,int user_wanted);
int reader_problem(char *path, item *it,int *reader_count, 
    sem_t *mutex,sem_t *write,sem_t *queue);
int extract_file(FILE *fptr,item *it);
char* read_line(FILE* fptr);
void runner(void *args);
void main_thread(char *path,sem_t *mutex,sem_t *write,sem_t *queue,int *reader_count);

void main_thread(char *path,sem_t *mutex,sem_t *write,sem_t *queue,int *reader_count){
   item it;
   
   /*
   * Read the file!
   */
   int entity = reader_problem(path,&it,reader_count,mutex,write,queue);
   
    /*
    * Check to see if it is in the list
    */

   int hit=check_forHit(&it,path);

   if (hit == -1 || entity < user->groceries[hit].count){
    /*
    ! Check to see if we have enough!
    */

    printf("LOG: NO hit in this thread!\n");
    // TODO: Terminate the thread!
    sem_post(sem_process);
    sem_wait(sem_thread);

    return;
   }
    //*Putting the new result!
    sem_wait(put_result);
    //!Critical section
    rcpt.items[rcpt.n++]=it;
    sem_post(put_result);

    sem_post(sem_process);
    sem_wait(sem_thread);

    //*Checking for final massage
    if(strcmp(end_massage,SUCCESS)==0){
        //*Means Succed so we need to update file
        writer_problem(path,queue,write,scores[hit],user->groceries[hit].count);
        return;
    }
    return;

    
   




    
}

void runner(void *args){

    /*
     Cast the args to thread_args and call main function.
    */
    char *path =(char *) args;
    int store_num;
    int file_num;

    if(sscanf(path, "Dataset/Store%d/%*[^/]/%d.txt", &store_num,&file_num) == 2){

        char shared_addr[1024];

        /*
        *Opening the mutex
        */
        sprintf(&shared_addr,"%s%d%d",SEM_MUTEX,store_num,file_num);
        sem_t *mutex = sem_open(shared_addr,0);
        if(mutex == SEM_FAILED){
            printf("Coudnot open mutex\n");
            return;
        }

        /*
        *Openning the writer!
        */
        sprintf(&shared_addr,"%s%d%d",SEM_WRITER,store_num,file_num);
        sem_t *writer = sem_open(shared_addr,0);
        if(write == SEM_FAILED){
            printf("ERROR: Couldnot open wirter\n");
            return;
        }

        /*
        *Openning the Queue!
        */
        sprintf(&shared_addr,"%s%d%d",SEM_QUEUE,store_num,file_num);
        sem_t *queue = sem_open(shared_addr,0);
        if(write == SEM_FAILED){
            printf("ERROR: Couldnot open Queue\n");
            return;
        }

        /*
        *Openning the Shared memory!
        */
        sprintf(&shared_addr,"%s%d%d",SHARED_INT_COUNT,store_num,file_num);
        int shm_flg=shm_open(shared_addr,O_RDWR,0666);
        if(shm_flg == -1){
            perror("shm_open");
            printf("ERROR: couldnot open shared memory!\n");
            return;
        }

        int *reader_count = mmap(NULL,SHARED_MEM_SIZE,
        PROT_READ | PROT_WRITE,MAP_SHARED,shm_flg,0);

        if(reader_count == MAP_FAILED){
            perror("mmap");
            printf("ERROR: Couldnot open reader_count!\n");
            return;
        }
        
        main_thread(path,mutex,write,queue,reader_count);
        
    }else
    {
        printf("Something went wrong in thread processing!\n");
    }
    
}

/*
Write to log
*/
void write_log(char *path,char *massage){
    FILE *fptr;
    sem_wait(log);
    //Entring critical section
    fptr = fopen(path,"a");
    fprintf(fptr,massage);
    fclose(fptr);
    sem_post(log);
    //Exiting critical section
}

/*
Check For hit!
*/
int check_forHit(item *it,char *path){
    int i=0;
    for(i = 0; i < user->n; i++)
    {
        if(strcmp(it->name,user->groceries->name)==0) break;
    }
    char massage[MAX_LINE_LEN];
    char new_path[MAX_LINE_LEN];
    snprintf(new_path,MAX_LINE_LEN,"%s/USER%d_ORDERID%d.log",path,user->user_id,user->order_id);
    if(i < user->n){
        
        snprintf(massage,MAX_LINE_LEN,"Found %s in %s path. My TID is %d\n",it->name,path,getpid());
    }else{
        i=-1;
        snprintf(massage,MAX_LINE_LEN,"NotFound in %s path. My TID is %d\n",new_path,getpid());
    }

    write_log(new_path,massage);
    
    
    return i;
    
}


/*
Edit the file
*/
void edit_file(char *path,double user_score,int user_wanted){
    FILE *fptr;
    fptr = fopen(path,"r+");
    
    int line_num=0;
    char line[256];
    double new_score;
    while(fgets(line,sizeof(line),fptr)){
        line_num++;
        if(line_num ==3){
            if(sscanf(line,"Score: %lf",&new_score)==1){
                //Write a new score
                new_score = (new_score + user_score)/2;
                fseek(fptr,-strlen(line),SEEK_CUR);
                fprintf(fptr,"Score: %.2f\n",new_score);

            }else{
                printf("ERROR: Something went wrong while changing score in file %s\n",path);
            }
        }
        else if (line_num == 4){
            int entity;
            if(sscanf(line,"Entity: %d",&entity)==1){
                //Write a new entity
                entity-=user_wanted;
                fseek(fptr,-strlen(line),SEEK_CUR);
                fprintf(fptr,"Entity: %d\n",entity);

            }else{
                printf("ERROR: Something went wrong while changing Entity in file %s\n",path);
            }

        }else if (line_num ==6){
            //Setting new time
            time_t tick;
            struct tm *timeinfo;

            time(&tick);
            timeinfo=localtime(&tick);

            char new_time[30];
            strftime(new_time,sizeof(new_time),"%Y-%m-%d %H:%M:%S",timeinfo);

            fseek(fptr,-strlen(line),SEEK_CUR);
            fprintf(fptr,"Last Modified: %s\n",new_time);
        }
    }
    
    fclose(fptr);


}

/*
Writer Problem! Critical section
*/
void writer_problem(char *path,sem_t *queue,sem_t *write,double user_score,int user_wanted){
    sem_wait(queue);
    sem_wait(write);
    //Critical section
    edit_file(path,user_score,user_wanted);
    //Exiting critical section
    sem_post(write);
    sem_post(queue);

}


/*
Reader Problem! Critical section
*/
int reader_problem(char *path, item *it,int *reader_count, 
    sem_t *mutex,sem_t *write,sem_t *queue){
    FILE *fptr;

    sem_wait(queue);
    sem_wait(mutex);
    (*reader_count)++;
    if(*reader_count == 1)
        sem_wait(write);
    sem_post(mutex);
    sem_post(queue);
    
    //CRITICAL!
    fptr = fopen(path,"r");
    int entity = extract_file(fptr,it);
    fclose(fptr);
    //Exit critical section
    sem_wait(mutex);
    (*reader_count)--;
    if(*reader_count==0)
        sem_post(write);
    sem_post(mutex);
    return entity;

}   

//Returns entitiy!
int extract_file(FILE *fptr,item *it){
    
    char *head = read_line(fptr);
    char * search= head + strcspn(head," ")+1;
    strcpy(it->name,search);
    free(head);


    head = read_line(fptr);
    search = head + strcspn(head," ")+1;
    it->price = atof(search);
    free(head);

    head = read_line(fptr);
    search = head + strcspn(head," ")+1;
    it->score = atof(search);
    free(head);


    head = read_line(fptr);
    search = head + strcspn(head," ")+1;
    int entity = atoi(search);
    free(head);

    return entity;
    
}

//Read a line
char* read_line(FILE* fptr){
    char *line = (char*) malloc(MAX_LINE_LEN * sizeof(char));
    if (line==NULL){
        perror("No more memory\n");
        return NULL;
    }
    if(fgets(line,MAX_LINE_LEN,fptr)){
        line[strcspn(line,"\n")]='\0';
        return line;
    }else{
        free(line);
        return NULL;
    }
}
#endif
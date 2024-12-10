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
int check_forHit(item it,char *path);
void edit_file(char *path,double user_score,int user_wanted);
void writer_problem(char *path,sem_t queue,sem_t write,double user_score,int user_wanted);
int reader_problem(char *path, item *it,int *reader_count, 
    sem_t mutex,sem_t write,sem_t queue);
int extract_file(FILE *fptr,item *it);
char* read_line(FILE* fptr);
void runner(void *args);


void main_thread(char *path,sem_t mutex,sem_t write,sem_t queue,int *reader_count){


    
}

void runner(void *args){

    /*
     Cast the args to thread_args and call main function.
    */
    char *path =(char *) args;
    //main_thread(path); TODO pass the other semaphores!
}

/*
Write to log
*/
void write_log(char *path,char *massage){
    FILE *fptr;
    sem_wait(&log);
    //Entring critical section
    fptr = fopen(path,"a");
    fprintf(fptr,massage);
    fclose(fptr);
    sem_post(&log);
    //Exiting critical section
}

/*
Check For hit!
*/
int check_forHit(item it,char *path){
    int i=0;
    for(i = 0; i < user->n; i++)
    {
        if(strcmp(it.name,user->groceries->name)==0) break;
    }
    char massage[MAX_LINE_LEN];
    char new_path[MAX_LINE_LEN];
    snprintf(new_path,MAX_LINE_LEN,"%s/USER%d_ORDERID%d.log",path,user->user_id,user->order_id);
    if(i < user->n){
        snprintf(massage,MAX_LINE_LEN,"Found %s in %s path. My TID is %d\n",it.name,path,getpid());
    }else{
        snprintf(massage,MAX_LINE_LEN,"NotFound in %s path. My TID is %d\n",new_path,getpid());
    }

    write_log(new_path,massage);
    
    
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
void writer_problem(char *path,sem_t queue,sem_t write,double user_score,int user_wanted){
    sem_wait(&queue);
    sem_wait(&write);
    //Critical section
    edit_file(path,user_score,user_wanted);
    //Exiting critical section
    sem_post(&write);
    sem_post(&queue);

}


/*
Reader Problem! Critical section
*/
int reader_problem(char *path, item *it,int *reader_count, 
    sem_t mutex,sem_t write,sem_t queue){
    FILE *fptr;

    sem_wait(&queue);
    sem_wait(&mutex);
    *reader_count++;
    if(*reader_count == 1)
        sem_wait(&write);
    sem_post(&mutex);
    sem_post(&queue);
    
    //CRITICAL!
    fptr = fopen(path,"r");
    int entity = extract_file(fptr,it);
    fclose(fptr);
    //Exit critical section
    sem_wait(&mutex);
    *reader_count--;
    if(*reader_count==0)
        sem_post(&write);
    sem_post(&mutex);
    return entity;

}   

//Returns entitiy!
int extract_file(FILE *fptr,item *it){
    
    char *head = read_line(fptr);
    char * search= head + strcspn(head," ")+1;
    strcpy(search,it->name);
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
#include "stdio.h"
#include "stdlib.h"
#include "structs.h"
#include "semaphore.h"
#include "string.h"

#define SEM_MUTEX "/sem_reader_mutex"
#define SEM_RW_MUTEX "/sem_rw_mutex"
#define SHARED_INT_COUNT "/shared_memory"
#define SHARED_MEM_SIZE sizeof(int)
#define MAX_LINE_LEN 100

int extract_file(FILE *fptr,item &it);
char* read_line(FILE* fptr);
int reader_problem(char *path,item &it,int &reader_count, 
    sem_t sem_reader,sem_t sem_rw);
void writer_problem(char *path,sem_t sem_rw,int user_score,int user_wanted);
void edit_file(FILE *fptr,int user_score,int user_wanted);
int check_forHit(userInfo *user,sem_t log,item it);
void write_log(FILE *fptr,char *name);

void main_thread(char *path,userInfo *usr, sem_t log){

}
/*
Write to log
*/
void write_log(){
/* TOOD fix*/
}
/*
Check For hit!
*/
int check_forHit(userInfo *user,sem_t log,item it){
    int i=0;
    for(i = 0; i < user->n; i++)
    {
        if(strcmp(it.name,user->groceries->name)==0) break;
    }
    char *name = nullptr;
    if(i < user->n){
        name=(it.name);
    }
    FILE fptr;
    //TOOD set address!
    sem_wait(&log);
    write_log();
    sem_post(&log);
    
}


/*
Edit the file
*/
void edit_file(FILE *fptr,int user_score,int user_wanted){

}

/*
Writer Problem! Critical section
*/
void writer_problem(char *path,sem_t sem_rw,int user_score,int user_wanted){
    FILE *fptr;
    sem_wait(&sem_rw);
    //Critical section
    fptr = fopen(path,"r+");
    edit_file(fptr,user_score,user_wanted);
    fclose(fptr);
    //Exiting critical section
    sem_post(&sem_rw);

}


/*
Reader Problem! Critical section
*/
int reader_problem(char *path, item &it,int &reader_count, 
    sem_t sem_reader,sem_t sem_rw){
    FILE *fptr;

    sem_wait(&sem_reader);
    reader_count++;
    if(reader_count == 1)
        sem_wait(&sem_rw);
    sem_post(&sem_reader);
    
    //CRITICAL!
    fptr = fopen(path,"r");
    int entity = extract_file(fptr,it);
    fclose(fptr);
    //Exit critical section
    sem_wait(&sem_reader);
    reader_count--;
    if(reader_count==0)
        sem_post(&sem_rw);
    sem_post(&sem_reader);
    return entity;

}   

//Returns entitiy!
int extract_file(FILE *fptr,item &it){
    
    char *head = read_line(fptr);
    char * search= head + strcspn(head," ")+1;
    strcpy(search,it.name);
    free(head);


    head = read_line(fptr);
    search = head + strcspn(head," ")+1;
    it.price = atof(search);
    free(head);

    head = read_line(fptr);
    search = head + strcspn(head," ")+1;
    it.score = atof(search);
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

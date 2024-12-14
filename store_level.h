#ifndef STORE_LEVEL_H
#define STORE_LEVEL_H


#include "stdio.h"
#include <dirent.h>
#include "string.h"
#include "unistd.h"
#include <stdlib.h>
#include "structs.h"
#include "user_level.h"
#include "encoding.h"
#include "sys/wait.h"
#include "category_level.h"


void category_level_thread(int write_fd, int read_fd, char *path);

extern userInfo *user;
void create_pipe(pipes *pi);

#define MAX_BUFFER_SIZE 1024

void child_test(int write_fd, int read_fd,char *path);
recipt *merge_result(char massage[8][MAX_BUFFER_SIZE],int n);

void main_store(int write_fd, int read_fd,char *path){
    struct dirent *dp;
    DIR *dir;
    dir =opendir(path);
    if(dir == NULL){
        printf("Log: cannot open directori!\n");
        return;
    }
    pipes p[16];
    int i =0;
    char newPath[512];
    pid_t pid;
    while ((dp = readdir(dir))!= NULL)
    {
        if(strcmp(".",dp->d_name)==0 || strcmp("..",dp->d_name) ==0) continue;

        //Create pipe
        create_pipe(&p[i]);
        create_pipe(&p[i+1]);
        
        printf("Debug:Openning This this category %s\n",dp->d_name);
        snprintf(newPath,512,"%s/%s",path,dp->d_name);
        pid = fork();
        if(pid ==0){
            close(p[i].read_fd);
            close(p[i+1].write_fd);
            category_level_thread(p[i].write_fd,p[i+1].read_fd,newPath);
            exit(0);
        }else{
            close(p[i].write_fd);
            close(p[i+1].read_fd);
        }
        i+=2;
        
    }
    char massages[8][MAX_BUFFER_SIZE];
    for ( i = 0; i < 16; i+=2){
        read(p[i].read_fd, massages[i/2],sizeof(char)*MAX_BUFFER_SIZE);
        printf("Massage is %s",massages[i/2]);
    }
    //Get the data and merge!
    recipt *r;
    r = merge_result(massages,user->n);
    char *result_msg = encode(r);

    /*
    *Sending data to Upper level
    */
   write(write_fd,result_msg,strlen(result_msg)+1);

    /*
    * Wait for upper level
    */
    char final_result[MAX_BUFFER_SIZE];
    read(read_fd,final_result,MAX_BUFFER_SIZE);
    
    /*
    *send data to Lower level
    */
    printf("Sending the result to category\n");
    for ( i = 0; i < 16; i+=2)
        write(p[i+1].write_fd, final_result,sizeof(final_result));

    /* 
    *Waiting to terminate all Childs!
    */
    while (wait(NULL)>0);
    
    

}



recipt *merge_result(char massage[8][MAX_BUFFER_SIZE],int n){
    recipt **arr=(recipt **)malloc(8*sizeof(recipt*));

    for (int i = 0; i < 8; i++)
    {
        arr[i]=decode(massage[i]);
    }
    recipt *result =(recipt *) malloc(sizeof(recipt));
    result->items = malloc(n*sizeof(item*));
    result->n=0;
    int j =0;

    //Start to merge
    for (int i = 0; i < n && j<8; i++)
    {
        if(arr[j]->n == 0){
            j++;
            i--;
            continue;
        }
        result->items[(result->n)++] = arr[j]->items[(arr[j]->n)-1];
        arr[j]->n--;
    }

    return result;
    
    
}


#endif
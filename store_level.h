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

#define MAX_BUFFER_SIZE 1024

void child_test(int write_fd, int read_fd,char *path);

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
    while ((dp = readdir(dir))!= NULL)
    {
        if(strcmp(".",dp->d_name)==0 || strcmp("..",dp->d_name) ==0) continue;

        //Create pipe
        create_pipe(&p[i]);
        create_pipe(&p[i+1]);
        
        printf("Debug:Openning This this category %s\n",dp->d_name);
        snprintf(newPath,512,"%s/%s",path,dp->d_name);

        if(fork()==0){
            close(p[i].read_fd);
            close(p[i+1].write_fd);
            child_test(p[i].write_fd,p[i+1].read_fd,newPath);
            exit(0);
        }else{
            close(p[i].write_fd);
            close(p[i+1].read_fd);
        }
        i+=2;
        
    }
    char massages[8][MAX_BUFFER_SIZE];

    for ( i = 0; i < 16; i+=2)
        read(p[i].read_fd, massages[i],sizeof(massages[i]));
    
    /*
    TODO Merge data!
    */
    

    /*
    TODO Sending data to upper level
    */
   
    /*
    TODO Wait for upper level
    */

    /*
    TODO send data to upper level
    */

    /*Waiting to terminate all Childs!*/
    while (wait(NULL)>0);
    
    

}

void child_test(int write_fd, int read_fd,char *path){
    printf("Address that is given %s\n",path);
}

// void store_process (int write_fd, int read_fd, char *full_path) {
//     // char path [1024];
//     // read(read_fd, path, sizeof(path));
//     // printf("Store received path %s\n", full_path);
//     char response[] = "process complete";
//     write(write_fd, response, strlen(response)+1);
//     // printf("Store send massage %s\n", response);
//     // printf("%s\n",full_path);
// }
recipt *merge_(char massage[8][MAX_BUFFER_SIZE],int n){
    recipt **arr=(recipt **)malloc(8*sizeof(recipt*));
    for (int i = 0; i < 8; i++)
    {
        arr[i]=decode(massage[i]);
    }
    recipt *result =(recipt *) malloc(sizeof(recipt));
    result->items = malloc(n*sizeof(item));
    result->n=0;
    int j =0;

    //Start to merge
    for (int i = 0; i < result->n; i++)
    {
        if(arr[j]->n == 0){
            j++;
            i--;
            continue;
        }
        result->items[n].name = arr[j]->items->name;
        result->items[n].price = arr[j]->items->price;
        result->items[n].score=arr[j]->items->score;
        n++;
        arr[j]->n--;
    }

    return result;
    
    
}


#endif
#include "stdio.h"
#include <dirent.h>
#include "string.h"
#include "unistd.h"
#include <stdlib.h>

void child_test(char *path);

void main_store(char *path/*,TODO some pipe*/){
    
    struct dirent *dp;
    DIR *dir;
    dir =opendir(path);
    if(dir == NULL){
        printf("Log: cannot open directori!\n");
        return;
    }
    char newPath[512];
    while ((dp = readdir(dir))!= NULL)
    {
        if(strcmp(".",dp->d_name)==0 || strcmp("..",dp->d_name) ==0) continue;
        printf("Debug:Openning This this category %s\n",dp->d_name);
        snprintf(newPath,512,"%s/%s",path,dp->d_name);

        if(fork()==0){
            /*TODO call category level!*/
            child_test(newPath);
            exit(0);
        }
        
    }
    /*
    TODO Wait for lower level
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

void child_test(char *path){
    printf("Address that is given %s\n",path);


void store_process (int write_fd, int read_fd, char *full_path) {
    // char path [1024];
    // read(read_fd, path, sizeof(path));
    // printf("Store received path %s\n", full_path);
    char response[] = "process complete";
    write(write_fd, response, strlen(response)+1);
    // printf("Store send massage %s\n", response);
    // printf("%s\n",full_path);
}
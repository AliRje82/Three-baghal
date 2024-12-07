#include "stdio.h"
#include <stdlib.h>
#include <pthread.h>
#include "string.h"
#include "module.h"

#define MAX_LINE_LEN 1024

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

void thread(char *ptr, char **requests,int* numbers,int n){
    FILE *fptr;
    fptr = fopen(ptr,"r");
    // if file not opend !
    if ( fptr == NULL){
        printf("Problem happend to thread %d\n",pthread_self());
    }
    char* lineH = read_line(fptr);
    char * search= lineH + strcspn(lineH," ")+1;
    
    
    char* found=NULL;
    int i =0;

    while (i<n && strcmp(search,requests[i])!=0 )
    {
        i++;
    }
    double price=-1;
    double score=-1;
    int entity = -1;

    if(i<n){
        found = requests[i];
        printf("HIT %s\n",found);
        free(lineH);

        //Finding the price
        lineH=read_line(fptr);
        search= lineH + strcspn(lineH," ")+1;
        price = atof(search);
        free(lineH);

        //Finding the score
        lineH=read_line(fptr);
        search= lineH + strcspn(lineH," ")+1;
        score = atof(search);
        free(lineH);

        //Finding entity
        lineH=read_line(fptr);
        search= lineH + strcspn(lineH," ")+1;
        entity = atoi(search);
        free(lineH);
        if(entity < numbers[i]){
            printf("Not enough item");
            fclose(fptr);
            return;
        }
        printf("HIT in thread %d\n",pthread_self());
        printf("Found item with price %f and score %f and entity %d\n",price,score,entity);

    }else{
        printf("No Hit!\n");
        fclose(fptr);
        return;
    }
}
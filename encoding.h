#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "structs.h"
#ifndef ENCODING_H
#define ENCODING_H

#define MAX_ITEM_SIZE 32
#define MAX_NUM_SIZE 5

recipt *decode(char *str){
    printf("decooooooooooode%s\n",str);
    recipt *rcpt = (recipt *) malloc(sizeof(recipt));
    char *search=strtok(str,",");
    printf("Search..........%s\n", search);
    rcpt->n = atoi(search);
    rcpt->items = (item **)malloc(rcpt->n * sizeof(item*));
    if(rcpt->n == 0){
        rcpt->items=NULL;
        return rcpt;
    }
    search=strtok(NULL,",");
    for (int i = 0; i < rcpt->n; i++)
    {   
        rcpt->items[i] = malloc(sizeof(item));
        rcpt->items[i]->name=malloc(sizeof(char) * (strlen(search)+1));
        strcpy(rcpt->items[i]->name,search);
        printf("%s\n", rcpt->items[i]->name);
        search = strtok(NULL, ",");
        rcpt->items[i]->score=atof(search);
        search = strtok(NULL, ",");
        rcpt->items[i]->price=atof(search);
        search = strtok(NULL, ",");
    }
    printf("Returning rcpt\n");
    return rcpt;    
}

char *encode(recipt *rcpt){

    char *massage = (char *)malloc(MAX_ITEM_SIZE * rcpt->n * sizeof(char));
    sprintf(massage,"%d,",rcpt->n);
    printf("Recipt size is %d\n",rcpt->n);
    char encoding[MAX_ITEM_SIZE];
    for (int i = 0; i < rcpt->n; i++)
    {
        
        if((rcpt->n)-1==i){
            sprintf(encoding,"%s,%.2f,%.2f", // Name,Score,price
        rcpt->items[i]->name,rcpt->items[i]->score,rcpt->items[i]->price);
        strcat(massage,encoding);
        continue;
        }
        sprintf(encoding,"%s,%.2f,%.2f,", // Name,Score,price,
        rcpt->items[i]->name,rcpt->items[i]->score,rcpt->items[i]->price); 
        strcat(massage,encoding);
    }
    return massage;
    
}

char *encode_score(double scores[],int n){
    char *massage = (char *)malloc(MAX_NUM_SIZE *sizeof(char)*n);
    massage[0]='\0';
    char encoding[MAX_NUM_SIZE];
    for (int i = 0; i < n; i++)
    {
        
        if(n-1==i)
            sprintf(encoding,"%.2f",scores[i]);
        else
            sprintf(encoding,"%.2f,",scores[i]);
        strcat(massage,encoding);
    }
    return massage;
    
}

double *decode_score(char *massage,int n){
    double *scores = (double *)malloc(n*sizeof(double));
    char *search=strtok(massage,",");
    for (int i = 0; i < n; i++)
    {
        scores[i] = atof(search);
        search=strtok(NULL,",");
    }
    return scores;
    
}

#endif
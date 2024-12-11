#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "structs.h"
#ifndef ENCODING_H
#define ENCODING_H

#define MAX_ITEM_SIZE 32
#define MAX_NUM_SIZE 5

recipt *decode(char *str){
    recipt *rcpt = (recipt *) malloc(sizeof(recipt));
    char *search=strtok(str,",");
    rcpt->n = atoi(search);
    rcpt->items = (item *)malloc(rcpt->n * sizeof(item));
    search=strtok(NULL,",");
    for (int i = 0; i < rcpt->n; i++)
    {   
        rcpt->items[i].name=search;
        search = strtok(NULL, ",");
        rcpt->items[i].score=atof(search);
        search = strtok(NULL, ",");
        rcpt->items[i].price=atof(search);
        search = strtok(NULL, ",");
    }

    return rcpt;    
}

char *encode(recipt *rcpt){

    char *massage = (char *)malloc(MAX_ITEM_SIZE * rcpt->n * sizeof(char));
    sprintf(massage,"%d,",rcpt->n);
    for (int i = 0; i < rcpt->n; i++)
    {
        
        char encoding[MAX_ITEM_SIZE];
        if(rcpt->n-1==i){
            sprintf(encoding,"%s,%.2f,%.2f", // Name,Score,price
        rcpt->items[i].name,rcpt->items[i].score,rcpt->items[i].price);
        }
        sprintf(encoding,"%s,%.2f,%.2f,", // Name,Score,price,
        rcpt->items[i].name,rcpt->items[i].score,rcpt->items[i].price); 
        strcat(massage,encoding);
    }
    /*For clean memory we can also do*/
    //free(rcpt);
    return massage;
    
}

char *encode_score(double scores[],int n){
    char *massage = (char *)malloc(MAX_NUM_SIZE *sizeof(char)*n);
    massage[0]='\0';
    for (int i = 0; i < n; i++)
    {
        char encoding[MAX_NUM_SIZE];
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
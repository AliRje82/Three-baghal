#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "structs.h"
#ifndef ENCODING_H
#define ENCODING_H

#define MAX_ITEM_SIZE 32

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

    char *massage = (char *)malloc(MAX_ITEM_SIZE * rcpt->n);
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

#endif
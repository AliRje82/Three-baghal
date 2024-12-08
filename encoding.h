#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "structs.h"

#define MAX_ITEM_SIZE 32

recipt *decode(char *str,int n){
    recipt *rcpt = (recipt *) malloc(sizeof(recipt));
    rcpt->n=n;
    rcpt->items = (item *)malloc(n * sizeof(item));
    char *search=strtok(str,",");
    for (int i = 0; i < n; i++)
    {   
        strcpy(search,rcpt->items[i].name);
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
    massage[0]='\0';
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


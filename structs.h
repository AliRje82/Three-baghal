#ifndef STRUCTS_H
#define STRUCTS_H
#include "semaphore.h"

typedef struct
{
    char name[64];
    int count;
} grocery;

typedef struct
{
    grocery *groceries;
    int user_id;
    int order_id;
    double budget;
    int n;
    int has_bought_from_store1;
    int has_bought_from_store2;
    int has_bought_from_store3;
} userInfo;

typedef struct
{
    double score;
    double price;
    char *name;
} item;

typedef struct
{
    item **items;
    int n;
} recipt;



typedef struct {
    int read_fd;
    int write_fd;
}pipes;


#endif 
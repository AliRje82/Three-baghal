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
    int budget;
    int n;
} userInfo;

typedef struct
{
    double score;
    double price;
    char name[64];
} item;

typedef struct
{
    item *items;
    int n;
} recipt;



typedef struct {
    int read_fd;
    int write_fd;
}pipes;

typedef struct{
    char *path;
    userInfo *user;
    sem_t log;
}thread_args;
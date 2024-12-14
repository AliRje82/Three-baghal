

#ifndef USER_LEVEL_H
#define USER_LEVEL_H

#include "stdio.h"
#include "structs.h"
#include "store_level.h"
#include "stdlib.h"
#include "string.h"
#include "dirent.h"
#include "sys/types.h"
#include "sys/wait.h"
#include "sys/stat.h"
#include "unistd.h"
#include "pthread.h"

const int grocery_no = 512;
grocery *groceries;
userInfo *user;
pipes *p[6];

typedef struct {
        int max_store;
        int max_pipe;
} data;

void create_pipe(pipes *pi)
{
    int pipe_fd[2];
    if (pipe(pipe_fd) == (-1))
    {
        perror("Pipe failed");
        return;
    }
    pi->read_fd = pipe_fd[0];
    pi->write_fd = pipe_fd[1];
}

void user_input()
{
    char *username = (char *)malloc(sizeof(char) * 512);
    groceries = (grocery *)malloc(grocery_no * sizeof(grocery));
    user = (userInfo *)malloc(sizeof(userInfo));

    printf("Username: ");
    scanf("%s", username);
    char *token = strtok(username, "user");
    user->user_id = atoi(token);
    printf("DEBUG user_id: %d\n", user->user_id);
    free(username);

    printf("OrderList(press e/E if finish)%d: \n", user->order_id);
    for (int i = 0; i < grocery_no; i++)
    {
        scanf("%s", groceries[i].name);
        if (strcmp(groceries[i].name, "e") == 0 || strcmp(groceries[i].name, "E") == 0)
        {
            break;
        }
        printf("Number of %s: ", groceries[i].name);
        scanf("%d", &groceries[i].count);
        user->n += groceries[i].count;
        printf("DEBUG grocery name, count and user total count: %s %d %d\n", groceries[i].name, groceries[i].count, user->n);
    }
    user->groceries = groceries;
    user->order_id++;

    printf("Price threshold(enter -1 if you prefer no threshold): ");
    scanf("%lf", &user->budget);
    printf("DEBUG user budget: %lf", user->budget);
}

void *collect_scores(void *arg)
{
    data* d = (data*)arg;
    printf("%d", d->max_store);
    double scores [user->n];
    char m = "\0";

    printf("Please enter you score for each items");
    for (int i = 0; i < user->n; i++)
    {
        printf("Score: ");
        scanf("%lf", &scores[i]);
    }
    for(int i = 0; i<6; i+=2) {
        if(i == d->max_pipe) {
            write(p[d->max_pipe]->write_fd, scores, user->n);
        }
        write(p[i]->write_fd, m, strlen(m)+1);
    }
    pthread_exit(NULL);
}

void get_score(recipt **rcpt)
{
    double score1 = 0;
    double price1 = 0;

    double score2 = 0;
    double price2 = 0;

    double score3 = 0;
    double price3 = 0;

    double max_score = 0;
    double max_price = 0;
    data* d = malloc(sizeof(data));

    for (int i = 0; i < rcpt[0]->n; i++)
    {
        score1 += rcpt[0]->items[i].score * rcpt[0]->items[i].price;
        price1 += rcpt[0]->items[i].price;
    }
    score1 /= rcpt[0]->n;

    for (int i = 0; i < rcpt[1]->n; i++)
    {
        score2 += rcpt[1]->items[i].score * rcpt[1]->items[i].price;
        price2 += rcpt[1]->items[i].price;
    }
    score2 /= rcpt[1]->n;

    for (int i = 0; i < rcpt[2]->n; i++)
    {
        score3 += rcpt[2]->items[i].score * rcpt[2]->items[i].price;
        price3 += rcpt[2]->items[i].price;
    }
    score3 /= rcpt[3]->n;

    if (score1 > score2)
    {
        max_score = score1;
        max_price = price1;
        d->max_store = 1;
        d->max_pipe = 0;
    }
    else
    {
        max_score = score2;
        max_price = price2;
        d->max_store = 2;
        d->max_pipe = 2;
    }
    if (score3 > max_score)
    {
        max_score = score3;
        max_price = price3;
        d->max_store = 3;
        d->max_pipe = 4;
    }

    if (max_price > user->budget)
    {
        printf("The price of the item (%lf) are beyond your budget (%lf)....You can not buy these stuffs\n", max_price, user->budget);
        return;
    }

    printf("The best score is %lf which belongs to store%d: \n", max_score, d->max_store);

    pthread_t thread;
    if (pthread_create(&thread, NULL, collect_scores, d) != 0)
    {
        perror("Thread creation failed");
        return;
    }
}

void user_level_process()
{
    const char *path = "./Dataset";
    DIR *dir = opendir(path);
    char full_path[1024];

    if (dir == NULL)
    {
        perror("Can not open Dataset directory");
        return;
    }

    struct dirent *entry;
    struct stat statbuf;
    pid_t pid;
    int p_no = 0;

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        if (stat(full_path, &statbuf) == (-1))
        {
            continue;
        }
        if (S_ISDIR(statbuf.st_mode))
        {
            p[p_no] = (pipes *)malloc(sizeof(pipes));
            p[p_no + 1] = (pipes *)malloc(sizeof(pipes));
            create_pipe(p[p_no]);
            create_pipe(p[p_no + 1]);
            printf("Pipes created for directory %s\n", entry->d_name);
            printf("Pipe%d read_fd %d and write_fd %d\n", p_no, p[p_no]->read_fd, p[p_no]->write_fd);
            printf("Pipe%d read_fd %d and write_fd %d\n", p_no + 1, p[p_no + 1]->read_fd, p[p_no + 1]->write_fd);
            pid = fork();
            if (pid < 0)
            {
                perror("Fork failed");
                // closedir(dir);
                return;
            }
            else if (pid == 0)
            {
                printf("child process %d handling directory %s\n", getpid(), full_path);
                close(p[p_no]->read_fd);
                close(p[p_no + 1]->write_fd);
                // store_process(p[p_no]->write_fd, p[p_no + 1]->read_fd, full_path);
                //  close(p[p_no]->write_fd);
                //  close(p[p_no + 1]->read_fd);
                exit(0);
            }
            else
            {
                close(p[p_no]->write_fd);
                close(p[p_no + 1]->read_fd);
                // char buffer[1024];
                // read(p[p_no]->read_fd, buffer, sizeof(buffer));
                // printf("Massage received from child %d\n", pid);
                // close(p[p_no]->read_fd);
                // close(p[p_no]->write_fd);
            }
            p_no += 2;
        }
    }
    recipt *rcpt[3];
    for (int i = 0, j = 0; i < 6; i += 2, j++)
    {
        char buffer[1024];
        read(p[i]->read_fd, buffer, sizeof(buffer));
        printf("%s from pipe %d\n", buffer, i);
        rcpt[j] = (recipt *)malloc(sizeof(recipt));
        rcpt[j] = decode(buffer);
    }
    get_score(rcpt);
    closedir(dir);
    free(groceries);
    free(user);
    while (wait(NULL) > 0)
        ;
}

#endif
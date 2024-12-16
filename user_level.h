

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
#include "time.h"
#include "database.h"

const int grocery_no = 512;
grocery *groceries;
userInfo *user;
pipes *p[6];

typedef struct
{
    int max_store;
    int max_pipe;
    double *scores;
} data;
typedef struct
{
    data *d;
    recipt **rcpt;
} thread_data;

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

    user_query();
    printf("User id is: %d\norder id is:%d\nstore1,2,3 are: %d %d %d\n", user->user_id, user->order_id, user->has_bought_from_store1,
           user->has_bought_from_store2, user->has_bought_from_store3);
    printf("DEBUG user_id: %d\n", user->user_id);
    free(username);
    int counter = 0;
    printf("OrderList%d (press e/E if finish): \n", user->order_id);
    for (int i = 0; i < grocery_no; i++)
    {
        scanf("%s", groceries[i].name);
        if (strcmp(groceries[i].name, "e") == 0 || strcmp(groceries[i].name, "E") == 0)
        {
            break;
        }
        printf("Number of %s: ", groceries[i].name);
        scanf("%d", &groceries[i].count);
        counter++;
        printf("DEBUG grocery name, count and user total count: %s %d %d\n", groceries[i].name, groceries[i].count, user->n);
    }
    user->n = counter;
    user->groceries = groceries;
    user->order_id++;
    printf("Price threshold(enter -1 if you prefer no threshold): ");
    scanf("%lf", &user->budget);
    printf("DEBUG user budget: %lf", user->budget);
}

void *calculate_scores(void *arg)
{
    thread_data *tdata = (thread_data *)arg;
    recipt **rcpt = tdata->rcpt;
    data *d = tdata->d;
    
    double score1 = 0;
    double price1 = 0;

    double score2 = 0;
    double price2 = 0;

    double score3 = 0;
    double price3 = 0;

    double max_score = 0;
    double max_price = 0;

    for (int i = 0; i < rcpt[0]->n; i++)
    {
        score1 += rcpt[0]->items[i]->score * rcpt[0]->items[i]->price;
        price1 += rcpt[0]->items[i]->price;
    }
    score1 /= rcpt[0]->n;

    for (int i = 0; i < rcpt[1]->n; i++)
    {
        score2 += rcpt[1]->items[i]->score * rcpt[1]->items[i]->price;
        price2 += rcpt[1]->items[i]->price;
    }
    score2 /= rcpt[1]->n;

    for (int i = 0; i < rcpt[2]->n; i++)
    {
        score3 += rcpt[2]->items[i]->score * rcpt[2]->items[i]->price;
        price3 += rcpt[2]->items[i]->price;
    }
    score3 /= rcpt[2]->n;

    if (score1 > score2)
    {
        max_score = score1;
        max_price = price1;
        d->max_store = 1;
        d->max_pipe = 1;
    }
    else
    {
        max_score = score2;
        max_price = price2;
        d->max_store = 2;
        d->max_pipe = 3;
    }
    if (score3 > max_score)
    {
        max_score = score3;
        max_price = price3;
        d->max_store = 3;
        d->max_pipe = 5;
    }

    printf("The best score is %lf which belongs to store%d: \n", max_score, d->max_store);

    if ((d->max_store == 1 && user->has_bought_from_store1) || (d->max_store == 2 && user->has_bought_from_store2) || (d->max_store == 3 && user->has_bought_from_store3))
    {
        max_price *= 0.9;
    }

    if (max_price > user->budget && user->budget != (-1))
    {
        printf("The price of the item (%lf) are beyond your budget (%lf)....You can not buy these stuffs\n", max_price, user->budget);
        pthread_exit(0);
    }

    switch (d->max_store)
    {
    case 1:
        user->has_bought_from_store1 = 1;
        break;
    case 2:
        user->has_bought_from_store2 = 1;
        break;
    case 3:
        user->has_bought_from_store3 = 1;
        break;
    default:
        break;
    }

    update_order_and_stores();
    pthread_exit(0);
}

void *collect_scores(void *arg)
{
    thread_data *tdata = (thread_data *)arg;
    data *d = tdata->d;

    printf("%d\n", d->max_store);
    printf("Please enter you score for each items\n");
    printf("%s\n", user->groceries[0].name);
    for (int i = 0; i < user->n; i++)
    {
        printf("Score for item %s:", user->groceries[i].name);
        if (scanf("%lf", &d->scores[i]) == -1)
        {
            printf("Error\n");
        }
        printf("%lf", d->scores[i]);
    }
    pthread_exit(NULL);
}

void *sending_scores(void *arg)
{
    thread_data *tdata = (thread_data *)arg;
    data *d = tdata->d;

    char *m = "1";
    char *str;
    printf("Sending the result\n");
    for (int i = 1; i < 6; i += 2)
    {
        if (i == d->max_pipe)
        {
            str = encode_score(d->scores, user->n);
            write(p[d->max_pipe]->write_fd, str, strlen(str) + 1);
        }
        else
        {
            write(p[i]->write_fd, m, strlen(m) + 1);
        }
    }
    pthread_exit(NULL);
}

void three_thread_process(recipt **rcpt)
{
    pthread_t thread1, thread2, thread3;
    thread_data tdata;

    data *d = malloc(sizeof(data));
    d->scores = malloc(sizeof(double) * user->n);

    tdata.d = d;
    tdata.d->scores = d->scores;
    tdata.rcpt = rcpt;

    if (pthread_create(&thread1, NULL, calculate_scores, &tdata) != 0)
    {
        perror("Failed to create thread1");
        return;
    }
    pthread_join(thread1, NULL);

    if (pthread_create(&thread2, NULL, collect_scores, &tdata) != 0)
    {
        perror("Failed to create thread2");
        return;
    }
    pthread_join(thread2, NULL);

    if (pthread_create(&thread3, NULL, sending_scores, &tdata) != 0)
    {
        perror("Failed to create thread3");
        return;
    }

    pthread_join(thread3, NULL);
    // free(d);
    // free(d->scores);
}

void user_level_process()
{
    user_input();
    const char *path = "./Dataset";
    printf("%s\n", user->groceries[0].name);

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
                closedir(dir);
                return;
            }
            else if (pid == 0)
            {
                printf("child process %d handling directory %s\n", getpid(), full_path);

                close(p[p_no]->read_fd);
                close(p[p_no + 1]->write_fd);
                main_store(p[p_no]->write_fd, p[p_no + 1]->read_fd, full_path);
                exit(0);
            }
            else
            {
                close(p[p_no]->write_fd);
                close(p[p_no + 1]->read_fd);
            }
            p_no += 2;
        }
    }
    closedir(dir);

    recipt *rcpt[3];
    char buffer[1024];
    for (int i = 0, j = 0; i < 6; i += 2, j++)
    {
        read(p[i]->read_fd, buffer, sizeof(buffer));
        printf("%s from pipe %d\n", buffer, i);
        rcpt[j] = decode(buffer);
    }
    three_thread_process(rcpt);
    while (wait(NULL) > 0)
        ;
    // free(groceries);
    // free(user);
    for (int i = 0; i < 6; i++)
    {
        free(p[i]);
    }
    printf("Ending for User\n");
}

#endif
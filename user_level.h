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

const int grocery_no = 512;
grocery *groceries;
userInfo *user;
pipes *p[6];

void create_pipe(pipes *p)
{
    int pipe_fd[2];
    if (pipe(pipe_fd) == (-1))
    {
        perror("Pipe failed");
        return;
    }
    p->read_fd = pipe_fd[0];
    p->write_fd = pipe_fd[1];
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
    scanf("%d", &user->budget);
    printf("DEBUG user budget: %d", user->budget);
}

void user_level_process()
{
    const char *path = "./Dataset";
    DIR *dir = opendir(path);
    ;
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
            p[p_no] = malloc(sizeof(pipes));
            p[p_no + 1] = malloc(sizeof(pipes));
            create_pipe(p[p_no]);
            create_pipe(p[p_no + 1]);
            printf("Pipes created for directory %s\n", entry->d_name);
            printf("Pipe%d read_fd %d and write_fd %d\n", p_no, p[p_no]->read_fd, p[p_no]->write_fd );
            printf("Pipe%d read_fd %d and write_fd %d\n", p_no+1, p[p_no+1]->read_fd, p[p_no+1]->write_fd );
            pid = fork();
            if(pid < 0) {
                perror("Fork failed");
                closedir(dir);
                return;
            }
            else if(pid == 0) {
                printf("child process %d handling directory %s\n", getpid(), full_path);
                close(p[p_no]->read_fd);
                close(p[p_no + 1]->write_fd);
                store_process(p[p_no]->write_fd, p[p_no + 1]->read_fd, full_path);
                close(p[p_no]->write_fd);
                close(p[p_no + 1]->read_fd);
                exit(0);
            }else {
                close(p[p_no]->write_fd);
                close(p[p_no]->read_fd);
                char buffer[1024];
                read(p[p_no]->read_fd, buffer,sizeof(buffer));
                printf("Massage received from child %d\n", pid);
                close(p[p_no]->read_fd);
                close(p[p_no]->write_fd);
            }
            p_no += 2;
        }
    }
    closedir(dir);
    free(groceries);
    free(user);
    while (wait(NULL) > 0)
        ;
}
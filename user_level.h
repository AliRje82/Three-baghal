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


void user_input() {
    char *username = (char *)malloc(sizeof(char) * 512);
    groceries = (grocery *)malloc(grocery_no * sizeof(grocery));
    user = (userInfo *)malloc(sizeof(userInfo));

    printf("Username: ");
    scanf("%s", username);
    char * token = strtok(username, "user");
    user->user_id = atoi(token);
    printf("DEBUG user_id: %d\n", user->user_id);
    free(username);

    printf("OrderList(press e/E if finish)%d: \n", user->order_id);
    for(int i = 0; i < grocery_no; i++) {
        scanf("%s", groceries[i].name);
        if(strcmp(groceries[i].name, "e") == 0|| strcmp(groceries[i].name, "E") == 0){
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

void user_level_process() {
    const char *path = "./Dataset";
    DIR *dir = opendir(path);;
    char full_path[1024];

    if(dir == NULL) {
        perror("Can not open Dataset directory");
        return;
    }

    struct dirent *entry;
    struct stat statbuf;
    pid_t pid;
    int pipe_user_store[2]; 
    int pipe_store_user[2];

    while((entry = readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        if(stat(full_path, &statbuf) == (-1)) {
            continue;
        }

        if(S_ISDIR(statbuf.st_mode)){
            if(pipe(pipe_user_store) == (-1) || pipe(pipe_store_user) == (-1)) {
                perror("Pipe failed");
                return;
            }

            pid = fork();
            if(pid < 0) {
                perror("Fork failed");
                closedir(dir);
                return;
            }
            else if(pid == 0) {
                printf("child process %d handling directory %s\n", getpid(), full_path);
                close(pipe_user_store[1]);
                close(pipe_store_user[0]);
                store_process(pipe_user_store[0], pipe_store_user[1], full_path);
                close(pipe_user_store[0]);
                close(pipe_store_user[1]);
                exit(0);
            }else {
                close(pipe_user_store[0]);
                close(pipe_store_user[1]);
                // write(pipe_user_store[1],full_path, strlen(full_path)+1);
                // printf("User received massages from %d\n", getpid());
                char buffer[1024];
                read(pipe_store_user[0], buffer,sizeof(buffer));
                close(pipe_user_store[1]);
                close(pipe_store_user[0]);
            }
        }        
    }
    closedir(dir);
    while(wait(NULL) > 0);
}
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "dirent.h"
#include "sys/types.h"
//#include "sys/wait.h"
#include "string.h"
#include "unistd.h"
#include "sys/stat.h"
#include "fcntl.h"

const int max_size = 512;

void user_input(char **goods, int *goods_count, int *budget){
    printf("Please enter your goods (if finish press e or E):\n");
    for(int i = 0; i<max_size; i++) {
        goods[i] = malloc(max_size * sizeof(char));
        printf("Goods %d: ", i+1);
        fgets(goods[i], max_size, stdin);
        goods[i][strcspn(goods[i], "\n")] = '\0';
        if(strcmp(goods[i], "e") == 0 || strcmp(goods[i], "E") == 0) {
            free(goods[i]);
            break;

        }
        (*goods_count)++;
    }
    printf("Please enter the max amount of budget you want to pay: \n");
    printf("Budget: ");
    scanf("%d", budget);
    printf("The goods are: \n");
    for(int i = 0; i < *goods_count; i++) {
        printf("%s\n", goods[i]);
    }
    printf("Credit: %d\n", *budget);
}

void user_level(){
    char **goods = malloc(max_size * sizeof(char*));
    int *goods_count = malloc(sizeof(int));
    *goods_count = 0;
    int *budget = malloc(sizeof(int));
    *budget = 0;
    int file_count = 0;
    user_input(goods, goods_count, budget);
    const char *path = "./Dataset";
    struct dirent *entry;
    DIR *dp;
    dp = opendir(path);
    if(dp == NULL) {
        perror("Can not open Dataset directory");
        return;
    }
    printf("Content of Dataset directory:\n");
    while((entry = readdir(dp)) != NULL) {
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        printf("%s\n", entry->d_name);
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            closedir(dp);
            return;
        }else if (pid == 1) {
            printf("Child process with PID %d handling directory %s\n ", getpid(), full_path);
            sleep(1);
            exit(0);
        }
    }
    closedir(dp);
    while(wait(NULL) > 0);
    printf("\n[Parent] All child processes have completed.\n");


    // for(int i = 0; i < count; i++){
    //     free(goods[i]);
    // }
    // free(goods);
    // free(count);
    // free(budget);
}
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "dirent.h"
#include "sys/types.h"
#include "sys/wait.h"
#include "sys/stat.h"
#include "unistd.h"

void store_process (int write_fd, int read_fd, char *full_path) {
    // char path [1024];
    // read(read_fd, path, sizeof(path));
    // printf("Store received path %s\n", full_path);
    char response[] = "process complete";
    write(write_fd, response, strlen(response)+1);
    // printf("Store send massage %s\n", response);
    // printf("%s\n",full_path);
}
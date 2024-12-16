#include "stdio.h"
#include "user_level.h"
#include "store_level.h"
int main(int argc, char *argv[]){
    if (argc < 5) {
        fprintf(stderr, "Usage: %s <userid> <shopping_list> <threshold> <num_items>\n", argv[0]);
        return -1;
    }
    char *userid = argv[1];
    char *shopping_list = argv[2];
    char *threshold = argv[3];
    int num_items = atoi(argv[4]);

    printf("%s\n",shopping_list);


    gui_user_input(userid, shopping_list, threshold, num_items);
    user_level_process();

}
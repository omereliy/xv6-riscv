#include <kernel/types.h>
#include <kernel/stat.h>
#include <user/user.h>


int
main(int argc, char* argv[]){
    int cd = channel_create();
    if (cd < 0) {
        printf("Failed to create channel\n");
        exit(1);
    }
    if (fork() == 0) {
        if (channel_put(cd, 42) < 0) {
            printf("Failed to put data in channel\n");
            exit(1);
        }
        if (channel_put(cd, 43) < 0)
            printf("error\n");
        if (channel_destroy(cd) < 0)
            printf("error\n");
    } else {
        int data;
        if (channel_take(cd, &data) < 0) { 
            printf("Failed to take data from channel\n");
            exit(1);
        }
        if (channel_take(cd, &data) < 0)
            printf("error\n"); 
        if (channel_take(cd, &data) < 0)
            printf("ok\n");
    }
    return 0;
}



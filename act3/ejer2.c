#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void processMaking(int curr, int max) {
    if (curr < max) {
        pid_t pid1 = fork();
        if (pid1 == 0) {
            printf("%d\n", curr + 1);
            processMaking(curr + 1, max);
            exit(0);
        } else {
            pid_t pid2 = fork();
            if (pid2 == 0) {
                printf("%d\n", curr + 1);
                processMaking(curr + 1, max);
                exit(0);
            } else {
                wait(NULL);
                wait(NULL);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    int level = atoi(argv[1]);
    printf("0\n");
    processMaking(0, level);
    return 0;
}
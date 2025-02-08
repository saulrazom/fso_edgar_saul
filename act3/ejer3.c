#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int n = atoi(argv[1]);

    for (int i = 1; i <= n; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            printf("Proceso hijo %d\n", i);
            exit(0);
        }
    }

    for (int i = 0; i < n; i++) {
        wait(NULL);
    }

    printf("Fin\n");

    return 0;
}
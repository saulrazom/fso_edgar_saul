#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t child_pid = fork();  

    if (child_pid == 0) {
        // HIJO
        printf("HIJO (%d)\n", getpid());
        sleep(20);  
    } else if (child_pid > 0) {
        // PADRE
        printf("PADRE (%d)\n", getpid());
        sleep(1);
    }

    else{
        perror("fork");
        exit(EXIT_FAILURE);
    }
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

int main() {
    printf("PADRE (%d)\n", getpid());

    // CHILDREN
    for (int i = 0; i < 2; i++) {
        pid_t child_pid = fork();
        if (child_pid == 0) {
            printf("    HIJO %d (%d)\n", i + 1, getpid());
            for (int j = 0; j < 3; j++) {
                pid_t grandson_pid = fork();
                if (grandson_pid == 0) {
                    printf("        NIETO %d DE HIJO %d (%d)\n", j + 1, i + 1, getpid());
                    sleep(20); 
                    exit(0); 
                } else if (grandson_pid < 0) {
                    perror("fork");
                    exit(EXIT_FAILURE);
                }
            }
            sleep(20);     
            exit(0);       
        } else if (child_pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }

    // PADRE
    sleep(5);
    printf("PADRE (%d) TERMINA HIJOS Y NIETOS \n", getpid());
    kill(0, SIGTERM); 

    return 0;
}
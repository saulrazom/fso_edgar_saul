#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    if (pid == 0) {//Hijo
        for (int i = 0; i < 10; i++) {
            printf("Soy el hijo\n");
            sleep(1);
        }
    } else if (pid > 0) {//Padre
        for (int i = 0; i < 10; i++) {
            printf("Soy el padre\n");
            sleep(1);
        }
        wait(NULL);
        printf("Mi proceso hijo ya ha terminado\n");
    } else {//Error
        perror("fork");
        return 1;
    }
    return 0;
}
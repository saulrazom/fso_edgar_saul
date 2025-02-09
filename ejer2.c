#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {//Hijo
        printf("Soy el hijo: %d\n", getpid());
        sleep(1);
        printf("Hijo terminado.\n");
    } else if (pid > 0) {//Padre
        printf("Soy el padre: %d\n", getpid());
        sleep(20);
        printf("Padre terminado.\n");
    } 

    return 0;
}
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) { //Hijo
        printf("Soy el hijo (PID: %d)\n", getpid());
        printf("Mi padre es: %d\n", getppid());
        kill(getppid(), SIGKILL); 
        printf("Hoy asesiné a mi padre\n");
    } else if (pid > 0) { //Padre
        printf("Soy el padre (PID: %d)\n", getpid());
        sleep(2); 
        printf("Si esto se imprime es porque sigo vivo\n");
    }
    return 0;
}
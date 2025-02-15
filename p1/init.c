#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#define N_GETTYS 6 

pid_t getty_pids[N_GETTYS];

// Función para manejar la señal de shutdown
void signal_handler(int signum) {
    if (signum == SIGUSR1) {
        printf("Terminando todos los procesos...\n");

        // KILL -> GETTY
        for (int i = 0; i < N_GETTYS; i++) {
            if (getty_pids[i] > 0) {
                kill(getty_pids[i], SIGTERM);
            }
        }

        // Esperar a que todos los procesos hijos terminen
        for (int i = 0; i < N_GETTYS; i++) {
            if (getty_pids[i] > 0) {
                waitpid(getty_pids[i], NULL, 0);
            }
        }

        exit(0);
    }
}

// Función para crear un proceso getty en una nueva ventana xterm
void create_getty(int i, const char *spid) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        // HIJO -> GETTY
        char *args[] = {"xterm", "-fn", "xft:Monospace:size=18", "-e", "./getty", (char *)spid, NULL};
        execvp("xterm", args);

        perror("execvp");
        exit(1);
    } else {
        // PADRE
        getty_pids[i] = pid;
        printf("PROCESO GETTY (%d)\n", pid);
    }
}

int main() {
    char spid[10];
    sprintf(spid, "%d", getpid());
    printf("PADRE (%s)\n", spid);

    // Inicializar signal handler para SIGUSR1
    signal(SIGUSR1, signal_handler);

    for (int i = 0; i < N_GETTYS; i++) {
        create_getty(i, spid);
    }

    while (1) {
        int status;
        pid_t terminated_pid = waitpid(-1, &status, WNOHANG);  

        if (terminated_pid > 0) {
            // Un proceso getty terminó
            printf("Terminó proceso Getty (%d).\n", terminated_pid);

            // Restaurar proceso getty terminado
            for (int i = 0; i < 6; i++) {
                if (getty_pids[i] == terminated_pid) {
                    create_getty(i, spid);
                    break;
                }
            }
        }

        sleep(1); // Verificar cada segundo
    }

    return 0;
}
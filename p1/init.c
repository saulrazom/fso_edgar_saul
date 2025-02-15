#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

pid_t getty_pids[6];

// Función para manejar la señal de shutdown
void signal_handler(int signum) {
    if (signum == SIGUSR1) {
        printf("Parent received SIGUSR1. Terminating all getty processes...\n");

        // Enviar SIGTERM a todos los procesos getty
        for (int i = 0; i < 6; i++) {
            if (getty_pids[i] > 0) {
                printf("Terminating getty process %d\n", getty_pids[i]);
                kill(getty_pids[i], SIGTERM);
            }
        }

        // Esperar a que todos los procesos hijos terminen
        for (int i = 0; i < 6; i++) {
            if (getty_pids[i] > 0) {
                waitpid(getty_pids[i], NULL, 0);
            }
        }

        // Terminar el proceso padre
        printf("All getty processes terminated. Exiting parent process.\n");
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
        // Si execvp falla, terminar el proceso hijo
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

    // Inicializar el arreglo de pids
    memset(getty_pids, 0, sizeof(getty_pids));

    // Crear los 6 procesos getty
    for (int i = 0; i < 6; i++) {
        create_getty(i, spid);
    }

    // Verificación de que los 6 procesos siempre estén en ejecución
    while (1) {
        int status;
        pid_t terminated_pid = waitpid(-1, &status, WNOHANG);  // No bloquear

        if (terminated_pid > 0) {
            // Un proceso getty terminó
            printf("GETTY PROCESS (%d) TERMINATED.\n", terminated_pid);

            // Encontrar el índice del proceso terminado
            for (int i = 0; i < 6; i++) {
                if (getty_pids[i] == terminated_pid) {
                    create_getty(i, spid);
                    break;
                }
            }
        }

        usleep(100000);  // Esperar 100ms para evitar un uso excesivo de CPU
    }

    return 0;
}
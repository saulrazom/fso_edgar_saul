#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>


pid_t getty_pids[6];

// Función para manejar la señal de shutdown
void signal_handler(int signum) {
    if (signum == SIGUSR1) {
        printf("Init: Received shutdown signal. Terminating all processes...\n");

        // Terminar todos los procesos getty
        for (int i = 0; i < 6; i++) {
            if (getty_pids[i] > 0) {
                kill(getty_pids[i], SIGTERM);  // Enviar señal de terminación
                printf("Init: Terminated getty process with PID %d\n", getty_pids[i]);
            }
        }

        // Terminar el proceso init
        printf("Init: All processes terminated. Exiting...\n");
        exit(0);
    }
}

// Función para crear un proceso getty en una nueva ventana xterm
void create_getty(int i, char spid) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        // HIJO -> GETTY
        char *args[] = {"xterm", "-fn", "xft:Monospace:size=18", "-e", "./getty", spid, NULL};
        execvp("xterm", args);
    } else {
        // PADRE
        getty_pids[i] = pid;
        printf("PROCESO GETTY (%d)\n", pid);
    }
}

int main() {
     char spid[10];
     sprintf(spid,"%d",getpid());
     printf("PADRE (%s)\n", spid);

    // Inicializar signal handler para SIGUSR1
    signal(SIGUSR1, signal_handler);

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

        sleep(1);  
    }

    return 0;
}
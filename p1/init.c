#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

#define NUM_GETTY 6  // Número de procesos getty

pid_t getty_pids[NUM_GETTY];  // Array para almacenar los PID de los procesos getty

// Función para manejar la señal de shutdown
void signal_handler(int signum) {
    if (signum == SIGUSR1) {
        printf("Init: Received shutdown signal. Terminating all processes...\n");

        // Terminar todos los procesos getty
        for (int i = 0; i < NUM_GETTY; i++) {
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
void create_getty(int index) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    } else if (pid == 0) {
        // Proceso hijo: ejecutar getty en una nueva ventana xterm
        char *args[] = {"xterm", "-e", "./getty", NULL};
        execvp("xterm", args);
        // Si execvp falla
        perror("execvp failed");
        exit(1);
    } else {
        // Proceso padre: almacenar el PID del proceso getty
        getty_pids[index] = pid;
        printf("Init: Created getty process with PID %d\n", pid);
    }
}

int main() {
    printf("Init: Starting init process (PID: %d)\n", getpid());

    // Registrar el manejador de la señal de shutdown
    signal(SIGUSR1, signal_handler);

    // Crear los 6 procesos getty
    for (int i = 0; i < NUM_GETTY; i++) {
        create_getty(i);
    }

    // Monitorear los procesos getty
    while (1) {
        int status;
        pid_t terminated_pid = waitpid(-1, &status, WNOHANG);  // No bloquear

        if (terminated_pid > 0) {
            // Un proceso getty terminó
            printf("Init: Getty process with PID %d terminated.\n", terminated_pid);

            // Encontrar el índice del proceso terminado
            for (int i = 0; i < NUM_GETTY; i++) {
                if (getty_pids[i] == terminated_pid) {
                    // Crear un nuevo proceso getty en su lugar
                    create_getty(i);
                    break;
                }
            }
        }

        sleep(1);  // Esperar 1 segundo antes de verificar nuevamente
    }

    return 0;
}
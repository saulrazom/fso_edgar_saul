#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

// Función para ejecutar un comando en primer o segundo plano
void execute_command(char *command, int background) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        return;
    } else if (pid == 0) {
        // Proceso hijo: ejecutar el comando
        char *args[] = {"/bin/sh", "-c", command, NULL};
        execvp("/bin/sh", args);
        // Si execvp falla
        perror("execvp failed");
        exit(1);
    } else if (!background) {
        // Proceso padre: esperar si es en primer plano
        wait(NULL);
    }
}

int main() {
    char input[256];

    while (1) {
        // Mostrar el prompt
        printf("sh > ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;  // Eliminar el salto de línea

        // Comandos especiales
        if (strcmp(input, "exit") == 0) {
            printf("Exiting shell...\n");
            break;
        } else if (strcmp(input, "shutdown") == 0) {
            printf("Initiating shutdown...\n");
            kill(getppid(), SIGUSR1);  // Enviar señal de shutdown a init
            break;
        }

        // Ejecutar comando en primer o segundo plano
        int background = (input[strlen(input) - 1] == '&');
        if (background) {
            input[strlen(input) - 1] = 0;  // Eliminar el '&'
        }
        execute_command(input, background);
    }

    return 0;
}
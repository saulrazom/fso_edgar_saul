#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

// Acepta comandos en primero segundo plano
void execute_command(char *command, int background) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return;
    } else if (pid == 0) {
        // HIJO
        char *args[] = {"/bin/sh", "-c", command, NULL};
        execvp("/bin/sh", args);
        // Si execvp falla
        perror("execvp failed");
        exit(1);
    } else if (!background) {
        // PADRE -> Espera a que el hijo termine
        wait(NULL);
    }
}

int main(int argc, char *argv[]) {
    char input[256];
    pid_t ppid = atoi(argv[1]);  // PID PADRE

    while (1) {
        printf("sh > ");
        if (!fgets(input, sizeof(input), stdin)) {
            // Mal input
            printf("\nInput incorrecto...\n");
            break;
        }
        input[strcspn(input, "\n")] = 0;  // Quitar salto de línea

        // Exit y shutdown
        if (strcmp(input, "exit") == 0) {
            printf("Regresando a login...\n");
            break;
        } else if (strcmp(input, "shutdown") == 0) {
            printf("Shutdown en proceso...\n");
            kill(ppid, SIGUSR1);  // Enviar señal de shutdown a init
            exit(0);
        }

        // Detección de si es primer o segundo plano
        int background = (input[strlen(input) - 1] == '&');
        if (background) {
            input[strlen(input) - 1] = 0;  // Quitar &
        }
        execute_command(input, background);
    }

    return 0;
}
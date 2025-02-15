#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PASSWD_FILE "passwd"  // Archivo de contraseñas

// Función para validar el login y password
int validate_credentials(const char *username, const char *password) {
    FILE *file = fopen(PASSWD_FILE, "r");
    if (!file) {
        perror("Failed to open passwd file");
        return 0;
    }

    char line[256];
    while (fgets(line, sizeof(line), file) {
        line[strcspn(line, "\n")] = 0;  // Eliminar el salto de línea
        char *saved_username = strtok(line, ":");
        char *saved_password = strtok(NULL, ":");

        if (saved_username && saved_password &&
            strcmp(username, saved_username) == 0 &&
            strcmp(password, saved_password) == 0) {
            fclose(file);
            return 1;  // Credenciales válidas
        }
    }

    fclose(file);
    return 0;  // Credenciales inválidas
}

int main() {
    char username[256];
    char password[256];

    while (1) {
        // Solicitar login y password
        printf("Login: ");
        if (!fgets(username, sizeof(username), stdin)) {
            break;  // Si fgets falla (por ejemplo, si el usuario presiona Ctrl+D)
        }
        username[strcspn(username, "\n")] = 0;  // Eliminar el salto de línea

        printf("Password: ");
        if (!fgets(password, sizeof(password), stdin)) {
            break;  // Si fgets falla
        }
        password[strcspn(password, "\n")] = 0;  // Eliminar el salto de línea

        // Validar credenciales
        if (validate_credentials(username, password)) {
            printf("Authentication successful. Starting shell...\n");

            // Crear un proceso hijo para ejecutar sh
            pid_t pid = fork();
            if (pid < 0) {
                perror("Fork failed");
                exit(1);
            } else if (pid == 0) {
                // Proceso hijo: ejecutar sh
                char ppid_str[16];
                snprintf(ppid_str, sizeof(ppid_str), "%d", getppid());  // Pasar el PID de getty a sh
                execl("./sh", "./sh", ppid_str, NULL);
                // Si execl falla
                perror("execl failed");
                exit(1);
            } else {
                // Proceso padre: esperar a que sh termine
                wait(NULL);
                printf("Shell terminated. Returning to login prompt.\n");
            }
        } else {
            printf("Authentication failed. Please try again.\n");
        }
    }

    return 0;
}
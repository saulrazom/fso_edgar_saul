#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PASSWD_FILE "passwd"  

int validate_credentials(const char *username, const char *password) {
    FILE *file = fopen(PASSWD_FILE, "r");
    if (!file) {
        perror("file");
        return 0;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0; 
        char *saved_username = strtok(line, ":");
        char *saved_password = strtok(NULL, ":");

        if (saved_username && saved_password &&
            strcmp(username, saved_username) == 0 &&
            strcmp(password, saved_password) == 0) {
            fclose(file);
            return 1;  // Válidas
        }
    }

    fclose(file);
    return 0; // Inválidas
}

int main(int argc, char *argv[]) {
    char *spid = argv[1];

    char username[256];
    char password[256];

    while (1) {
        // Solicitar login y password
        printf("User: ");
        scanf("%s", username);
        printf("Password: ");
        scanf("%s", password);

        // Validar credenciales
        if (validate_credentials(username, password)) {
            printf("Datos correctos. Iniciando sh\n");

            // SH
            pid_t pid = fork();
            if (pid < 0) {
                perror("fork");
                exit(1);
            } else if (pid == 0) {
              char *args[] = {"sh", spid, NULL};
              execvp("./sh", args);

             // If execvp() fails
             perror("execvp failed");
             exit(1);
            } else {
                wait(NULL);
                printf("Regresando a login...\n");
            }
        } else {
            printf("Usuario o contraseña incorrecta. Intente de nuevo\n");
        }
    }

    return 0;
}
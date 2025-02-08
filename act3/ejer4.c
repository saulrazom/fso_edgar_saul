#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    char command[50];

    while(1){
        // Entrada
        printf("Introduce un comando o exit para terminar: ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;

        //Si es salida
        if(strcmp(command, "exit") == 0){
            printf("Ejecución terminada");
            exit(1);
        }

        int exec = system(command);

    }

    return 0;
}
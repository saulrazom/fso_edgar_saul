#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <sys/wait.h> 

int main(){
    char command[50];
    int p, code;

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

        p = fork();

        if(p==0){
            execlp(command, command, (char *)NULL);
        }

        wait(&code);
        printf("Código: %x\n", code);

    }

    return 0;
}
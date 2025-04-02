#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>

#define JUGADORES 4
#define MAX_MSG_SIZE sizeof(struct msgbuf)
#define MSG_TYPE_TURNO 1
#define QUEUE_NAME "/cola_domino"

struct msgbuf {
    long mtype;    
    int jugador;  
};

void jugador(int i) {
    struct msgbuf mensaje;
    int vueltas = 10; 
    int siguiente = (i % JUGADORES) + 1;
    
    mqd_t cola = mq_open(QUEUE_NAME, O_RDWR);
    if (cola == (mqd_t)-1) {
        perror("mq_open en jugador");
        exit(EXIT_FAILURE);
    }

    while(vueltas--) {
        // Espera el mensaje del jugador anterior o el de arranque en el caso del primer jugador
        if (mq_receive(cola, (char*)&mensaje, MAX_MSG_SIZE, NULL) == -1) {
            perror("mq_receive");
            exit(EXIT_FAILURE);
        }
        
        printf("JUGADOR %d INICIA ", i);
        sleep(1);
        
        mensaje.mtype = siguiente;
        
        // Enviar mensaje a siguiente jugador
        if (mq_send(cola, (char*)&mensaje, MAX_MSG_SIZE, 0) == -1) {
            perror("mq_send");
            exit(EXIT_FAILURE);
        }
        
        printf("| JUGADOR %d TERMINA\n", i);
    }
    
    mq_close(cola);
    exit(EXIT_SUCCESS);
}

int main() {
    int i;
    pid_t p;
    struct msgbuf mensaje;
    struct mq_attr attr = {
        .mq_flags = 0,          // Blocking mode
        .mq_maxmsg = 10,        // MAX número de mensajes en cola
        .mq_msgsize = MAX_MSG_SIZE, // MAX tamaño de mensaje
        .mq_curmsgs = 0         // n mensajes en cola
    };

    // Cola de mensajes POSIX
    mqd_t cola = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0666, &attr);
    if (cola == (mqd_t)-1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    // Procesos de jugadores
    for(i = 1; i <= JUGADORES; i++) {
        p = fork();
        if(p == 0) {
            jugador(i);
        } else if(p < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }

    // Iniciar enviando primer mensaje al jugador 1
    mensaje.mtype = 1;
    if (mq_send(cola, (char*)&mensaje, MAX_MSG_SIZE, 0) == -1) {
        perror("mq_send inicial");
        exit(EXIT_FAILURE);
    }

    for(i = 1; i <= JUGADORES; i++) {
        wait(NULL);
    }

    mq_close(cola);
    mq_unlink(QUEUE_NAME);

    return EXIT_SUCCESS;
}
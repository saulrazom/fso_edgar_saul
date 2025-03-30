/* 
   Para compilar:
   gcc -o mercator mercator.c -lrt -lm
   Se necesitan las librerías de tiempo real (-lrt) y matemáticas (-lm)
*/

#include <stdio.h>       
#include <stdlib.h>      
#include <unistd.h>      
#include <math.h>        
#include <sys/time.h>    
#include <sys/wait.h> 
#include <sys/shm.h>   
#include <mqueue.h>      
#include <fcntl.h>      
#include <sys/stat.h>    

#define NPROCS 4                 
#define SERIES_MEMBER_COUNT 200000 


typedef struct {//El maestro le da instrucciones al becario
    double x;                   
} InputMessage;

typedef struct { //El becario devuelve lo que calculo
    double sum;                  
} ResultMessage;

double get_member(int n, double x) {
    int i;
    double numerator = 1;
    for(i = 0; i < n; i++)
        numerator = numerator * x;

    if (n % 2 == 0)
        return (-numerator / n);
    else
        return numerator / n;
}

// Funcion para los becarios
void slave_proc(int proc_num) {
    char qname[20];
    // Crea nombre único de cola para este esclavo
    sprintf(qname, "/slave_%d", proc_num);
    
    // Abre la cola en modo solo lectura
    mqd_t mq = mq_open(qname, O_RDONLY);
    if (mq == (mqd_t)-1) {
        perror("mq_open slave");
        exit(1);
    }

    InputMessage msg;
    // Bloquea hasta recibir el mensaje con x
    if (mq_receive(mq, (char*)&msg, sizeof(msg), NULL) == -1) {
        perror("mq_receive");
        exit(1);
    }
    mq_close(mq);  // Cierra la cola después de recibir

    double sum = 0.0;
    // Calcula los términos asignados a este esclavo
    for(int i = proc_num; i < SERIES_MEMBER_COUNT; i += NPROCS)
        sum += get_member(i + 1, msg.x);

    // Abre cola de resultados para enviar suma parcial
    mqd_t result_mq = mq_open("/result_queue", O_WRONLY);
    if (result_mq == (mqd_t)-1) {
        perror("mq_open result");
        exit(1);
    }

    // Prepara y envía el mensaje con el resultado
    ResultMessage res_msg = { .sum = sum };
    if (mq_send(result_mq, (char*)&res_msg, sizeof(res_msg), 0) == -1) {
        perror("mq_send");
        exit(1);
    }

    mq_close(result_mq); // Cierra la cola de resultados
    exit(0);             // Termina el proceso esclavo
}

int main() {
    struct timeval ts;           // Para medición de tiempo
    long long start_ts;
    long long stop_ts;
    long long elapsed_time;
    mqd_t result_mq;             // Cola de resultados
    struct mq_attr attr = {      // Atributos de las colas
        .mq_flags = 0,           
        .mq_maxmsg = 10,         
        .mq_msgsize = sizeof(ResultMessage) 
    };

    result_mq = mq_open("/result_queue", O_CREAT | O_RDONLY, 0666, &attr); //abrimos la cola
    if (result_mq == (mqd_t)-1) {
        perror("mq_open result");
        exit(1);
    }

   
    for (int i = 0; i < NPROCS; i++) {//Ciclo que crea las colas para los becarios
        char qname[20];
        sprintf(qname, "/slave_%d", i);
        mqd_t slave_mq = mq_open(qname, O_CREAT | O_WRONLY, 0666, &attr);
        if (slave_mq == (mqd_t)-1) {
            perror("mq_open slave create");
            exit(1);
        }
        mq_close(slave_mq);
    }

    gettimeofday(&ts, NULL);
    start_ts = ts.tv_sec;

    // Hacemos el fork
    for (int i = 0; i < NPROCS; i++) {
        pid_t pid = fork();
        if (pid == 0) {     
            slave_proc(i);  
        } else if (pid < 0) {
            perror("fork");
            exit(1);
        }
    }

    FILE *fp = fopen("entrada.txt", "r");
    if (!fp) {
        perror("fopen");
        exit(1);
    }
    double x;
    fscanf(fp, "%lf", &x);  
    fclose(fp);

    // Envía x a todos los becarios
    for (int i = 0; i < NPROCS; i++) {
        char qname[20];
        sprintf(qname, "/slave_%d", i);
        mqd_t slave_mq = mq_open(qname, O_WRONLY);
        if (slave_mq == (mqd_t)-1) {
            perror("mq_open slave send");
            exit(1);
        }
        
        InputMessage msg = { .x = x };
        if (mq_send(slave_mq, (char*)&msg, sizeof(msg), 0) == -1) {
            perror("mq_send x");
            exit(1);
        }
        mq_close(slave_mq); 
    }

    // Sumar todo
    double total = 0.0;
    for (int i = 0; i < NPROCS; i++) {
        ResultMessage res_msg;
        // Bloquea hasta recibir resultado
        if (mq_receive(result_mq, (char*)&res_msg, sizeof(res_msg), NULL) == -1) {
            perror("mq_receive result");
            exit(1);
        }
        total += res_msg.sum;  
    }

    
    gettimeofday(&ts, NULL);
    stop_ts = ts.tv_sec;
    elapsed_time = stop_ts - start_ts;

    
    printf("Cálculo con %d términos de la serie\n", SERIES_MEMBER_COUNT);
    printf("Tiempo total: %lld segundos\n", elapsed_time);
    printf("Resultado calculado: %10.8f\n", total);
    printf("Valor real de ln(1 + %f): %10.8f\n", x, log(1 + x));

    
    mq_close(result_mq);                  
    mq_unlink("/result_queue");           
    for (int i = 0; i < NPROCS; i++) {    
        char qname[20];
        sprintf(qname, "/slave_%d", i);
        mq_unlink(qname);
    }

    
    for (int i = 0; i < NPROCS; i++) {
        wait(NULL);
    }

    return 0;
}
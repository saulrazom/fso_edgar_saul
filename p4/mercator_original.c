/*
Para compilar incluir la librería m (matemáticas)
Ejemplo:
gcc -o mercator mercator.c -lm
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/shm.h>

#define NPROCS 4
#define SERIES_MEMBER_COUNT 200000

typedef struct {
    double sums[NPROCS];
    int proc_count;
    int start_all;
    double x_val;
    double res;
} SHARED;

SHARED *shared;

double get_member(int n, double x)
{
    int i;
    double numerator = 1;
    for(i = 0; i < n; i++)
        numerator = numerator * x;

    if (n % 2 == 0)
        return (-numerator / n);
    else
        return numerator / n;
}

void proc(int proc_num)
{
    int i;
    // Espera a que master_proc ponga la bandera en 1 para iniciar
    while(!(shared->start_all));

    // Cada proceso realiza el cálculo de los términos que le tocan
    shared->sums[proc_num] = 0;

    for(i = proc_num; i < SERIES_MEMBER_COUNT; i += NPROCS)
        shared->sums[proc_num] += get_member(i + 1, shared->x_val);

    // Incrementa la variable proc_count que es la cantidad de procesos que terminaron su cálculo
    shared->proc_count++;
    exit(0);
}

void master_proc()
{
    int i;
    // Obtener el valor de x desde el archivo entrada.txt
    FILE *fp = fopen("entrada.txt", "r");
    if(fp == NULL)
        exit(1);
    fscanf(fp, "%lf", &shared->x_val);
    fclose(fp);
    
    // el master_proc pone una bandera para que los procesos inicien con el cálculo
    shared->start_all = 1;
    
    // Espera a que todos los procesos terminen su cálculo
    while (shared->proc_count != NPROCS) {}
    
    // Una vez que todos terminan, suma el total de cada uno
    shared->res = 0;
    for(i = 0; i < NPROCS; i++)
        shared->res += shared->sums[i];
    exit(0);
}

int main()
{
    int *threadIdPtr;
    long long start_ts;
    long long stop_ts;
    long long elapsed_time;
    long lElapsedTime;
    struct timeval ts;
    int i;
    int p;
    int shmid;
    int status;
    
    // Solicita y conecta la memoria compartida
    shmid = shmget(0x1234, sizeof(SHARED), 0666 | IPC_CREAT);
    shared = shmat(shmid, NULL, 0);
    
    // inicializa las variables en memoria compartida
    shared->proc_count = 0;
    shared->start_all = 0;
    
    gettimeofday(&ts, NULL);
    start_ts = ts.tv_sec; // Tiempo inicial
    
    for(i = 0; i < NPROCS; i++)
    {
        p = fork();
        if(p == 0)
            proc(i);
    }
    
    p = fork();
    if(p == 0)
        master_proc();
    
    printf("El recuento de ln(1 + x) miembros de la serie de Mercator es %d\n", SERIES_MEMBER_COUNT);
    
    for(int i = 0; i < NPROCS + 1; i++)
    {
        wait(&status);
        if(status == 0x100) // Si el master_proc termina con error
        {
            fprintf(stderr, "Proceso no puede abrir el archivo de entrada\n");
            break;
        }
    }
    
    gettimeofday(&ts, NULL);
    stop_ts = ts.tv_sec; // Tiempo final
    elapsed_time = stop_ts - start_ts;
    
    printf("Tiempo = %lld segundos\n", elapsed_time);
    printf("El resultado es %10.8f\n", shared->res);
    printf("Llamando a la función ln(1 + %f) = %10.8f\n", shared->x_val, log(1 + shared->x_val));
    
    // Desconecta y elimina la memoria compartida
    shmdt(shared);
    shmctl(shmid, IPC_RMID, NULL);
}
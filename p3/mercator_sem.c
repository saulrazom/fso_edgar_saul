// Saul Razo Magallanes
// Edgar Alonso Zaragoza Delgadillo

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>

#define NPROCS 4
#define SERIES_MEMBER_COUNT 200000

typedef struct {
    double sums[NPROCS];
    int proc_count;
    double x_val;
    double res;
    // Se reemplaza el funcionamiento de int start_all con el de los semáforos start_sem y end_sem
} SHARED;

SHARED *shared;

sem_t *start_sem; // Esclavos pueden comenzar
sem_t *end_sem;   // Esclavos terminan
sem_t *mutex;    

double get_member(int n, double x) {
    int i;
    double numerator = 1;
    for (i = 0; i < n; i++)
        numerator = numerator * x;
    if (n % 2 == 0)
        return (-numerator / n);
    else
        return numerator / n;
}

void proc(int proc_num) {
    int i;
    // Espera a que el maestro indique que puede comenzar
    sem_wait(start_sem);

    // Cada proceso realiza el cálculo de los términos que le tocan
    shared->sums[proc_num] = 0;
    for (i = proc_num; i < SERIES_MEMBER_COUNT; i += NPROCS)
        shared->sums[proc_num] += get_member(i + 1, shared->x_val);

    // Incrementa la variable proc_count que es la cantidad de procesos que terminaron su cálculo
    sem_wait(mutex);
    shared->proc_count++;
    sem_post(mutex);

    // Indica que este proceso ha terminado
    sem_post(end_sem);

    exit(0);
}

void master_proc() {
    int i;
    // Obtener el valor de x desde el archivo entrada.txt
    FILE *fp = fopen("entrada.txt", "r");
    if (fp == NULL)
        exit(1);
    fscanf(fp, "%lf", &shared->x_val);
    fclose(fp);

    // Inicia los procesos esclavos
    for (i = 0; i < NPROCS; i++)
        sem_post(start_sem);

    // Espera a que todos los procesos terminen su cálculo
    for (i = 0; i < NPROCS; i++)
        sem_wait(end_sem);

    // Una vez que todos terminan, suma el total de cada uno
    shared->res = 0;
    for (i = 0; i < NPROCS; i++)
        shared->res += shared->sums[i];

    exit(0);
}

int main() {
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

    // Inicializa las variables en memoria compartida
    shared->proc_count = 0;

    // Elimina los semáforos si ya existen
    sem_unlink("/start_sem");
    sem_unlink("/end_sem");
    sem_unlink("/mutex");

    // Inicializa los semáforos
    start_sem = sem_open("/start_sem", O_CREAT | O_EXCL, 0666, 0); // bloqueado
    end_sem = sem_open("/end_sem", O_CREAT | O_EXCL, 0666, 0);     // bloqueado
    mutex = sem_open("/mutex", O_CREAT | O_EXCL, 0666, 1);         // desbloqueado para que el primer proceso pueda entrar

    if (start_sem == SEM_FAILED || end_sem == SEM_FAILED || mutex == SEM_FAILED) {
        perror("Error al crear los semáforos");
        exit(1);
    }

    gettimeofday(&ts, NULL);
    start_ts = ts.tv_sec; // Tiempo inicial

    for (i = 0; i < NPROCS; i++) {
        p = fork();
        if (p == 0)
            proc(i);
    }

    p = fork();
    if (p == 0)
        master_proc();

    printf("El recuento de ln(1 + x) miembros de la serie de Mercator es %d\n", SERIES_MEMBER_COUNT);

    for (int i = 0; i < NPROCS + 1; i++) {
        wait(&status);
        if (status == 0x100) // Si el master_proc termina con error
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

    sem_close(start_sem);
    sem_close(end_sem);
    sem_close(mutex);
    sem_unlink("/start_sem");
    sem_unlink("/end_sem");
    sem_unlink("/mutex");

    return 0;
}
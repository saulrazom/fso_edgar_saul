#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>
#include <pthread.h>

#define TAMBUFFER 10

typedef struct {
    int in; // Posición del siguiente elemento por entrar al buffer
    int out; // Posición del siguiente elemento a retirar del buffer
    int buffer[TAMBUFFER];  // Buffer circular
} BUFFER;

// Semáforos para implementar el monitor
sem_t monitor_mutex;    // Controla el acceso al monitor
sem_t condition_sem;    // Para la condición de espera
int waiting_count = 0;  // Número de hilos esperando

void enter_monitor();
void leave_monitor();
void cwait();
void cnotify();

void buffer_init(BUFFER *bf);
void buffer_put(BUFFER *b,int data);
int buffer_get(BUFFER *b);

void *productor(void *args);
void *consumidor(void *args);;

BUFFER buffer;
int elementos=0;

int main()
{
    pthread_t tid[2];

    srand(getpid());

    sem_init(&monitor_mutex, 0, 1);
    sem_init(&condition_sem, 0, 0);

    buffer_init(&buffer);

    pthread_create(&tid[0],NULL,productor,NULL);
    pthread_create(&tid[1],NULL,consumidor,NULL);

    pthread_join(tid[0],NULL);
    pthread_join(tid[1],NULL);

    sem_destroy(&monitor_mutex);
    sem_destroy(&condition_sem);
}

void enter_monitor()
{
    sem_wait(&monitor_mutex);
}

void leave_monitor()
{
    sem_post(&monitor_mutex);
}

void cwait()
{
    waiting_count++;
    sem_post(&monitor_mutex);
    sem_wait(&condition_sem);
    sem_wait(&monitor_mutex);
}

void cnotify()
{
    if (waiting_count > 0) {
        waiting_count--;
        sem_post(&condition_sem);
    }
}

void buffer_init(BUFFER *bf) 
{
    bf->in = 0;
    bf->out = 0;
}

void buffer_put(BUFFER *bf, int e)
{
    enter_monitor();

    while(elementos==TAMBUFFER)
        cwait();

    bf->buffer[bf->in]=e;
    bf->in++;
    bf->in=bf->in%TAMBUFFER;

    elementos++;

    cnotify();

    leave_monitor();
}

int buffer_get(BUFFER *bf)
{
    int retval;

    enter_monitor();

    while(elementos==0)
        cwait();

    retval=bf->buffer[bf->out];
    bf->out++;
    bf->out=bf->out%TAMBUFFER;

    elementos-- ;

    cnotify();

    leave_monitor();

    return(retval);
}

void *productor(void *args)
{
    int e=0;
    int n=10;
    while(n--)
    {
        e=e+1;
        printf("Productor produce %d\n",e);
        buffer_put(&buffer,e);
        usleep(rand()%3000000);
    }
}

void *consumidor(void *args)
{
    int e;
    int n=10;
    while(n--)
    {
        e=buffer_get(&buffer);
        printf("\t\t\tConsumidor consume %d\n",e);
        usleep(rand()%3000000);
    }
}
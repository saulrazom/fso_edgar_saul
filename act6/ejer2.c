#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>

#define ITERS 1000000000 // Número total de iteraciones
#define NUM_THREADS 4    // Número de hilos a utilizar

// Estructura para pasar datos a los hilos
typedef struct {
    int start;          // Inicio del rango de iteraciones para este hilo
    int end;            // Fin del rango de iteraciones para este hilo
    double x;           // Valor de x
    double partial_sum; // Suma parcial calculada por este hilo
} ThreadData;

// Función que ejecuta cada hilo
void *calculate_partial_sum(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    double sum = 0.0;

    for (int n = data->start; n < data->end; n++) {
        sum += pow(-1, n + 1) * pow(data->x, n) / n;
    }

    data->partial_sum = sum;
    pthread_exit(NULL);
}

int main() {
    long long start_ts;
    long long stop_ts;
    long long elapsed_time;
    struct timeval ts;
    float x;
    double total_sum = 0.0;

    printf("Calcular el logaritmo natural de 1+x\n");
    printf("Dame el valor de x: ");
    scanf("%f", &x);

    // Crear hilos y datos para cada hilo
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    // Dividir el trabajo entre los hilos
    int iter_per_thread = ITERS / NUM_THREADS;

    // Iniciar el tiempo
    gettimeofday(&ts, NULL);
    start_ts = ts.tv_sec;

    // Crear y ejecutar los hilos
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].start = i * iter_per_thread + 1;
        thread_data[i].end = (i + 1) * iter_per_thread + 1;
        thread_data[i].x = x;
        thread_data[i].partial_sum = 0.0;

        if (i == NUM_THREADS - 1) {
            // Asegurarse de que el último hilo cubra todas las iteraciones restantes
            thread_data[i].end = ITERS;
        }

        pthread_create(&threads[i], NULL, calculate_partial_sum, &thread_data[i]);
    }

    // Esperar a que todos los hilos terminen
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        total_sum += thread_data[i].partial_sum;
    }

    // Detener el tiempo
    gettimeofday(&ts, NULL);
    stop_ts = ts.tv_sec;
    elapsed_time = stop_ts - start_ts;

    // Mostrar resultados
    printf("Resultado = %.4lf\n", total_sum);
    printf("------------------------------\n");
    printf("TIEMPO TOTAL, %lld segundos\n", elapsed_time);

    return 0;
}
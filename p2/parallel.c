// Saul Razo Magallanes
// Edgar Alonso Zaragoza Delgadillo

#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>


#define NUM_THREADS 4  

double f(double x) {
    return sqrt(1 - x * x);
}

// Estructura para pasar datos a cada hilo (El antifalse es para asegurarme de que no se alacenen juntos, lo puse despues de que me pasara :())
typedef struct {
    int start;       
    int end;         
    double sum;      
    double h;
    char antifalse[64];   
} ThreadData;


void* calculate_pi(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    data->sum = 0.0; 

    for (int i = data->start; i < data->end; i++) {
        double x = data->h * (i + 0.5); 
        data->sum += f(x);              
    }

    pthread_exit(NULL); 
}

double divide_work(int n) {
    pthread_t threads[NUM_THREADS];      
    ThreadData thread_data[NUM_THREADS]; 
    int size = n / NUM_THREADS;    
    double total_sum = 0.0;              

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].start = i * size;         
        if (i == NUM_THREADS - 1) {
            thread_data[i].end = n; 
        } else {
            thread_data[i].end = (i + 1) * size; 
        }
        thread_data[i].h = 1.0 / n;                    
        pthread_create(&threads[i], NULL, calculate_pi, &thread_data[i]); 
    }

    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL); 
        total_sum += thread_data[i].sum; 
    }

    return 4 * (1.0 / n) * total_sum; 
}


int main() {
    long n = 1000000000;  
    struct timeval start, end; 
    double duration;

    gettimeofday(&start, NULL);

    double pi = divide_work(n);

    gettimeofday(&end, NULL);

    duration = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;

    printf("PI (parallel): %.16f\n", pi);
    printf("Tiempo (parallel): %.6f segundos\n", duration);

    return 0;
}
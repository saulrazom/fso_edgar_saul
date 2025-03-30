#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

#define NTHREADS 3
#define ITERS 10

bool flag[3] = {false, false, false};
int turno = 0;

void *T(void *args) {
    int i = *((int *)args);
    int j = (i + 1) % 3;
    int k = (i + 2) % 3;
    int n = ITERS;

    while (n--) {
        // Entrada a la sección de entrada
        printf("[P%d] Entrando a sección de entrada. flag[%d] = true\n", i, i);
        flag[i] = true;
        printf("[P%d] turno = %d\n", i, j);
        turno = j;

        // Verificación de la condición de espera
        printf("[P%d] Verificando: (flag[%d] || flag[%d]) && turno == %d → (%d || %d) && (%d == %d)\n",
               i, j, k, j, flag[j], flag[k], turno, j);
        while ((flag[j] || flag[k]) && turno == j) { 
            // Espera activa
        }

        // Sección crítica
        printf("[P%d] **ENTRA** a la sección crítica\n", i);
        printf("[P%d] **SALE** de la sección crítica\n", i);

        // Liberar el acceso
        printf("[P%d] flag[%d] = false\n", i, i);
        flag[i] = false;

        // Sección restante (simulación)
        for (int x = 0; x < 1000000; x++); // Pequeña espera para forzar alternancia
    }
    return NULL;
}

int main() {
    pthread_t tid[NTHREADS];
    int args[NTHREADS];
    
    for (int i = 0; i < NTHREADS; i++) {
        args[i] = i;
        pthread_create(&tid[i], NULL, T, &args[i]);
    }

    for (int i = 0; i < NTHREADS; i++) {
        pthread_join(tid[i], NULL);
    }
    
    return 0;
}
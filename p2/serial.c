#include <stdio.h>
#include <math.h>
#include <sys/time.h>

double f(double x)
{
    return sqrt(1 - x * x);
}

double calculate_pi(int n){
    double h = 1.0 / n; // Ancho de cada intervalo
    double sum = 0.0;
    for (int i = 0; i < n; i++)
    {
        double x = h * (i + 0.5); // x a la mitad del rectángulo
        sum += f(x);
    }
    return 4 * h * sum; // 4 * área que hay debajo del cuarto del círculo

}

int main()
{
    long n = 1000000000;  
    struct timeval start, end;
    double duration;

    gettimeofday(&start, NULL);
    double pi = calculate_pi(n);
    gettimeofday(&end, NULL);

    duration = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;

    printf("PI (serial): %.16f\n", pi);
    printf("Tiempo (serial): %.6f segundos\n", duration);
}
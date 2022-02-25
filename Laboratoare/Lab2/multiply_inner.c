#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

int N;
int P;
int **a;
int **b;
int **c;

pthread_mutex_t lock;

#define MIN(x,y) ((x < y) ? x : y)

void *thread_function(void *arg)
{
	int thread_id = *(int *)arg;
	int i, j, k;

	int start = thread_id * ceil(N/P);
	int end = MIN ((thread_id + 1) * ceil(N/P), N);

	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			for (k = start; k < end; k++) {
				if (pthread_mutex_lock(&lock) != 0){
					printf("Eroare la realizarea lock-ului\n");
					exit(-1);
				}

				c[i][j] += a[i][k] * b[k][j];
				
				if (pthread_mutex_unlock(&lock) != 0){
					printf("Eroare la deschiderea lock-ului\n");
					exit(-1);
				}
			}
		}
	}
	
	pthread_exit(NULL);
}

void get_args(int argc, char **argv)
{
	if(argc < 3) {
		printf("Numar insuficient de parametri: ./program N P\n");
		exit(1);
	}

	N = atoi(argv[1]);
	P = atoi(argv[2]);
}

void init()
{
	a = malloc(sizeof(int *) * N);
	b = malloc(sizeof(int *) * N);
	c = malloc(sizeof(int *) * N);

	if (a == NULL || b == NULL || c == NULL) {
		printf("Eroare la malloc!");
		exit(1);
	}

	int i, j;
	for (i = 0; i < N; i++) {
		a[i] = malloc(sizeof(int) * N);
		b[i] = malloc(sizeof(int) * N);
		c[i] = malloc(sizeof(int) * N);

		if(a[i] == NULL || b[i] == NULL || c[i] == NULL) {
			printf("Eroare la malloc!");
			exit(1);
		}
	}

	for (i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			c[i][j] = 0;

			if(i <= j) {
				a[i][j] = 1;
				b[i][j] = 1;
			} else {
				a[i][j] = 0;
				b[i][j] = 0;
			}
		}
	}
}

void print(int **mat)
{
	int i, j;

	for (i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			printf("%i\t", mat[i][j]);
		}
		printf("\n");
	}
}

int main(int argc, char *argv[])
{
	int i;

	if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("Eroare la initializarea mutex-ului\n");
        return 1;
    }

	get_args(argc, argv);
	init();

	pthread_t tid[P];
	int thread_id[P];

	for (i = 0; i < P; i++) {
		thread_id[i] = i;
		pthread_create(&tid[i], NULL, thread_function, &thread_id[i]);
	}

	for (i = 0; i < P; i++) {
		pthread_join(tid[i], NULL);
	}

	if (pthread_mutex_destroy(&lock) != 0) {
		printf("Eroare la dezalocarea mutex-ului\n");
		exit(-1);
	}

	print(c);

	return 0;
}
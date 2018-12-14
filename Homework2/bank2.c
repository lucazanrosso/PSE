/* This example is rewritten based on the example from 
* <http://www.coe.uncc.edu/~abw/parallel/pthreads/pthreads.html 
* It illustrates the idea of shared memory programming 
* It simulates the access to balance of a bank account from multiple 
* threads. Demonstrate the importance of proper coordination. 
* oct-09-2000 
*
* REVISED: 5/23/04 Monica Reggiani
*
*/

#include <pthread.h>   
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX 20
#define NUMTHREAD 3      /* number of threads */
#define LIMIT 50         /* low balance limit */

pthread_mutex_t balance_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t i_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t belowLimit  = PTHREAD_COND_INITIALIZER;

int thread_id[NUMTHREAD]  = {0,1,2};
int i = 0, j = 0;

/* shared variable */
double balance = 0;

void * depositor(int *id) {
	double deposit;

	srand(time(NULL));

	while(j < MAX) {
		deposit = (double)(rand() % 100);/* generate a random amt of deposit?*/

		pthread_mutex_lock(&balance_mutex);
	  
		balance += deposit;
		printf("          deposited :%10.2f balance is %10.2f: by %d\n", deposit, balance, *id);
		fflush(stdout);
		j ++;
		if (balance >= LIMIT)
			pthread_cond_signal(&belowLimit);
		pthread_mutex_unlock(&balance_mutex);
      
		if (rand()%100 >= 50)
			sleep(rand()%2);
    }
	pthread_exit(NULL);
}

void * withdrawer(int *id) {

	double debt;
	srand(time(NULL));

	while (i < MAX) {
      
		debt = (double)(rand() % 60);  /* generate a random amt of withdraw */
		
		pthread_mutex_lock(&balance_mutex);
		
		if (balance < LIMIT) 
			pthread_cond_wait(&belowLimit, &balance_mutex);
		if (balance - debt < 0)
			printf("sorry can't withdraw\n");
		else {
			balance -= debt;
			printf("%d withdraw %10.2f : by  :%d: balance %10.2f\n",i, debt, *id, balance);
		}
		fflush(stdout);
		// pthread_mutex_lock(&i_mutex);
		i++;
		// pthread_mutex_unlock(&i_mutex);
		pthread_mutex_unlock(&balance_mutex);
		if (rand()%100 >= 50)
			sleep(rand()%2);
    }
  pthread_exit(NULL);
}


int main() {
	int i;
	int status;
	/* define the type to be pthread */
	pthread_t thread[NUMTHREAD];

	/* create 3 threads*/
	/* create one depositor and two withdrawer */
	status = pthread_create(&thread[1], NULL, (void *)withdrawer, &thread_id[1]);
	if (status != 0) {
		printf("Create withdrawer thread");
		exit(EXIT_FAILURE);
	}

	status = pthread_create(&thread[2], NULL, (void *)withdrawer, &thread_id[2]);
	if (status != 0) {
		printf("Create withdrawer thread");
		exit(EXIT_FAILURE);
	}

	status = pthread_create(&thread[0], NULL, (void *)depositor, &thread_id[0]);
	if (status != 0) {
		printf("Create depositer thread");
		exit(EXIT_FAILURE);
	}

	for(i=0; i< NUMTHREAD ; i++) {
		status = pthread_join(thread[i], NULL);
		if (status != 0) {
			printf("Join thread");
			exit(EXIT_FAILURE);
		}
	}

	pthread_exit(NULL);
}

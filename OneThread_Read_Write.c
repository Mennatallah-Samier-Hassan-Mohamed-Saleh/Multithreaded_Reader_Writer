#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


int x = 0;  			/* variable to write in */
int flag = 0;  			/* flag */

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;  	/* mutex lock for buffer */
pthread_cond_t c_read = PTHREAD_COND_INITIALIZER; /* reader waits on this cond var */
pthread_cond_t c_write = PTHREAD_COND_INITIALIZER; /* writer waits on this cond var */

void *read (void * param);
void *write (int input);

int main(int argc, char *argv[]) {

	pthread_t tid1, tid2;  /* thread identifiers */
	int i;

	/* create the threads; may be any number, in general */
	if(pthread_create(&tid1, NULL, write, 1) != 0) {
		fprintf(stderr, "Unable to create writer thread\n");
		exit(1);
	}

	if(pthread_create(&tid2, NULL, read, NULL) != 0) {
		fprintf(stderr, "Unable to create consumer thread\n");
		exit(1);
	}

	/* wait for created thread to exit */
	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	printf("Parent quiting\n");

	return 0;
}

/* Write value(s) */
void *write(int input) {

		/* Insert into x */
		pthread_mutex_lock (&m);	
	    x=input;
		pthread_mutex_unlock (&m);
		printf ("Writer: inserted %d\n", input);
		fflush (stdout);
	return 0;
}

/* Read values*/
void *read(void *reader) {
		pthread_mutex_lock (&m);
	    printf("x equals %d \n", x);
		pthread_mutex_unlock (&m);
	return 0;
}
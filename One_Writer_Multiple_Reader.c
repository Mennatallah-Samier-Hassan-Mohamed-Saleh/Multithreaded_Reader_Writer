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
	for(i=0;i<5;++i)
	{
	if(pthread_create(&tid2, NULL, read, NULL) != 0) {
		fprintf(stderr, "Unable to create consumer thread\n");
		exit(1);
	}
	}


	/* wait for created thread to exit */
	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	printf("Parent quiting\n");

	return 0;
}

/* Produce value(s) */
void *write(int input) {
		/* Insert into x */
		pthread_mutex_lock (&m);	
		while (flag !=0) {  /* chec if any thread is reading*/
			pthread_cond_wait (&c_write, &m);
		}
        flag=-1;
		pthread_mutex_unlock (&m);

		x=input;
		printf ("Writer: inserted %d\n", input);

        pthread_mutex_lock (&m);	
        flag=0;
		pthread_cond_broadcast (&c_read);
		pthread_cond_signal (&c_write);
        pthread_mutex_unlock (&m);
		
		fflush (stdout);
	return 0;
}

/* Consume value(s); Note the consumer never terminates */
void *read(void *reader) {
		pthread_mutex_lock (&m);
		while (flag ==-1) {  // A writer is writing
			pthread_cond_wait (&c_read, &m);
		}
        flag=flag+1;
		pthread_mutex_unlock (&m);

	    printf("x equals %d \n", x);
		printf("flag from reader in between equals %d \n", flag);

        pthread_mutex_lock (&m);
        flag=flag-1;
		if (flag==0)
		{
			pthread_cond_signal (&c_write);
		}
		pthread_mutex_unlock (&m);

		printf("flag from reader after equals %d \n", flag);
		

	return 0;
}
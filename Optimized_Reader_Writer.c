#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/* Constants*/
#define NUM_READERS 5
#define NUM_WRITERS 5
#define NUM_READ 5
#define NUM_WRITE 5

/* Global variable*/
unsigned int x = 0;  			/* variable to write in */
int flag = 0;  			/* flag */
int gwaitingReader=0;

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;  	/* mutex lock for buffer */
pthread_cond_t c_read = PTHREAD_COND_INITIALIZER; /* reader waits on this cond var */
pthread_cond_t c_write = PTHREAD_COND_INITIALIZER; /* writer waits on this cond var */

void *read (void * param);
void *write (void * param);

int main(int argc, char *argv[]) {
	int i;

	pthread_t twrite[NUM_WRITERS] ;  /* Writer thread identifiers */
	pthread_t tread[NUM_READERS] ;  /* Reader thread identifiers */

    int numwrite[NUM_WRITE]; /* writer temp variable*/
	int numread[NUM_READ]; /*Reader remp variable*/

	// Seed the random number generator
	srandom((unsigned int)time(NULL));

	/* create writer threads; may be any number, in general */
	for(i=0;i<NUM_WRITERS;++i){
	    numwrite[i]=i;
		if(pthread_create(&twrite[i], NULL, write,&numwrite[i]) != 0) {
			fprintf(stderr, "Unable to create writer thread\n");
			exit(1);
		}
    }

	/* create reader threads; may be any number, in general */
	for(i=0;i<NUM_READERS;++i)
	{
		numread[i]=i;
		if(pthread_create(&tread[i], NULL, read, &numread[i]) != 0) {
			fprintf(stderr, "Unable to create reader thread\n");
			exit(1);
		}
	}


	/* wait for writer threads to exit */
	for(i=0;i<NUM_WRITERS;++i){
		pthread_join(twrite[i], NULL);
    }

	for(i=0;i<NUM_READERS;++i)
	{
		pthread_join(tread[i], NULL);
	}

	printf("Parent quiting\n");
	printf("flag at end equals %d \n", flag);
	

	return 0;
}

/* Write value(s) */
void *write(void* param) {
	/* Insert into x */
	int i;
	int id=*((int*)param);
	for (int i=0;i<NUM_WRITE;++i)
	{
		// Wait so that reads and writes do not all happen at once
	    usleep(1000 * (random() % NUM_READERS + NUM_WRITERS));

		// Enter critical section
		pthread_mutex_lock (&m);	
		while (flag !=0) {  /* check if any thread is reading*/
			pthread_cond_wait (&c_write, &m);
		}
        flag=-1;
		pthread_mutex_unlock (&m);
        //sleep(10);
		printf ("thread %d Writing %d and Number of readers is  %d \n", id, ++x, flag);
		if(flag==-1)
		{	
			printf("One writer is writing \n");
		}
		else
		{
			printf("More than one writer, Error");
		}

	  	// Exit critical section
        pthread_mutex_lock (&m);	
        flag=0;
		if (gwaitingReader>0)
		{
			pthread_cond_broadcast (&c_read);

		}
		else
		{
			pthread_cond_signal (&c_write);
		}

        pthread_mutex_unlock (&m);

	}

	
  pthread_exit(0);
}

/* Read value(s) */
void *read(void *param) {
		int i;
		int id=*((int*)param);

		for (int i=0;i<NUM_READ;++i)
		{
			// Wait so that reads and writes do not all happen at once
	  		usleep(1000 * (random() % NUM_READERS + NUM_WRITERS));

			// Enter critical section
			pthread_mutex_lock (&m);
			gwaitingReader++;
			while (flag ==-1) {  // A writer is writing
				pthread_cond_wait (&c_read, &m);
			}
			gwaitingReader--;
        	flag=flag+1;
			pthread_mutex_unlock (&m);

			//sleep(10);
	    	printf("thread %d reading x equals %d and Number of readers is  %d\n",id, x,flag);

			// Exit critical section
        	pthread_mutex_lock (&m);
        	flag=flag-1;
			if (flag==0)
			{
				pthread_cond_signal (&c_write);
			}
		
			pthread_mutex_unlock (&m);

		}
		

	pthread_exit(0);
}
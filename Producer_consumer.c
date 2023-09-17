#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define BUF_SIZE 3		/* Size of shared buffer */

int buffer[BUF_SIZE];  	/* shared buffer */
int add = 0;  			/* place to add next element */
int rem = 0;  			/* place to remove next element */
int num = 0;  			/* number elements in buffer */

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;  	/* mutex lock for buffer */
pthread_cond_t c_cons = PTHREAD_COND_INITIALIZER; /* consumer waits on this cond var */
pthread_cond_t c_prod = PTHREAD_COND_INITIALIZER; /* producer waits on this cond var */

void *producer (void *param);
void *consumer (void *param);

int main(int argc, char *argv[]) {

	pthread_t tid1, tid2;  /* thread identifiers */
	int i;

	/* create the threads; may be any number, in general */
	if(pthread_create(&tid1, NULL, producer, NULL) != 0) {
		fprintf(stderr, "Unable to create producer thread\n");
		exit(1);
	}

	if(pthread_create(&tid2, NULL, consumer, NULL) != 0) {
		fprintf(stderr, "Unable to create consumer thread\n");
		exit(1);
	}

	/* wait for created thread to exit */
	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	printf("Parent quiting\n");

	return 0;
}

/* Produce value(s) */
void *producer(void *param) {

	int i;
	for (i=1; i<=20; i++) {
		
		/* Insert into buffer */
		pthread_mutex_lock (&m);	
			if (num > BUF_SIZE) {
				exit(1);  /* overflow */
			}

			while (num == BUF_SIZE) {  /* block if buffer is full */
				// waiting for a consumer to consume and take from the buffer
				pthread_cond_wait (&c_prod, &m);
			}
			
			/* if executing here, buffer not full so add element */
			buffer[add] = i;
			// Make sure add doesn't exceed buffer size
			add = (add+1) % BUF_SIZE;
			// increment number of elements in the buffer
			num++;
		pthread_mutex_unlock (&m);
        // Notify the comsumer that a new product was created.
		pthread_cond_signal (&c_cons);
		printf ("producer: inserted %d\n", i);
		//causes the system to empty the buffer that is associated with the specified output stream, if possible
		fflush (stdout);
	}

	printf("producer quiting\n");
	//causes the system to empty the buffer that is associated with the specified output stream, if possible 
	fflush(stdout);
	return 0;
}

/* Consume value(s); Note the consumer never terminates */
void *consumer(void *param) {

	int i;

	while(1) {
		//tries to remove an element from a buffer and update rem and num accordingly.
		pthread_mutex_lock (&m);
			if (num < 0) {
				exit(1);
			} /* underflow */

			while (num == 0) {  /* block if buffer empty */
			    /*
				it waits for a new product to be created using the c_cons condition 
				variable which is signaled by the producer as we saw
				*/
				pthread_cond_wait (&c_cons, &m);
			}

			/* if executing here, buffer not empty so remove element */
			i = buffer[rem];
			// for the end of buffer operations
			rem = (rem+1) % BUF_SIZE;
			// Decrement the total number of elements in the array.
			num--;
		pthread_mutex_unlock (&m);
        // once the consumer consumed one element of the buffer and is no longer full, we need to notify the producer
		// using c_prod condition variable that there is room in the buffer to produce more elements
		// only one producer can insert one lement in the buffer so it is signal
		pthread_cond_signal (&c_prod);
		printf ("Consume value %d\n", i);  fflush(stdout);

	}
	return 0;
}
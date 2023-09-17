#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 5

void * print( void* args){
    int num=*((int*)args);
    printf("Hello thread %d \n", num);
    return 0;
}

int main(void){
    int i;
    pthread_t tid[NUM_THREADS];
    int num[NUM_THREADS];
    for (i=0; i<NUM_THREADS ;i++)
    {
        num[i]=i;
        pthread_create(&tid[i],NULL,print,&num[i]);
    }
    for (i=0; i<NUM_THREADS ;i++)
    {
        num[i]=i;
        pthread_join(tid[i],NULL);
    }
    
    return 0;



    return 0;
}
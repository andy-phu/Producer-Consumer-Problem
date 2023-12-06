#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 10
pthread_cond_t notempty = PTHREAD_COND_INITIALIZER;
pthread_cond_t notfull = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;



struct Producer{
    int id;
    int itemCount;
    int delay;
};

struct Conumer{
    int id;
    int delay;
};

int itemCount = 0;

void *producer(void *arg) {
    //uses the argument given and creates a struct out of it to access delay 
    struct Producer *producer = (struct Producer *)arg;

    while (1) {
        pthread_mutex_lock(&mutex);
        //while the buffer is full continue waiting til the notFull signal is given from the consumer 
        while (itemCount == MAX_BUFFER_SIZE) {
            pthread_cond_wait(&notfull, &mutex);
        }
        //Produces an item and adds it to the buffer 
        
        //Increment itemCount
        itemCount++;
        //Signal notempty
        pthread_cond_signal(&notempty);
        pthread_mutex_unlock(&mutex);

        //delay requested by user 
        usleep(threadDelay->delay);
    }
    return NULL;
}

void *consumer(void *arg) {
    //uses the argument given and creates a struct out of it to access delay 
    struct Consumer *consumer = (struct Consumer *)arg;
    while (1) {
        pthread_mutex_lock(&mutex);
        //while there isn't anything in the buffer wait while on the notEmpty condition
        while (itemCount == 0) {
            pthread_cond_wait(&notempty, &mutex);
        }
        //Produces an item and adds it to the buffer 

        //Decrement itemCount
        itemCount--;

        //Signal notFull after a consumption
        pthread_cond_signal(&notfull);
        pthread_mutex_unlock(&mutex);
        //delay requested by user 
        usleep(threadDelay->delay);
    }
    return NULL;
}

int main(int argc, char *argv[]){
    //grabs the 5 numbers that represent [p c i b d]
    int p = atoi(argv[1]);
    int c = atoi(argv[2]);
    int i = atoi(argv[3]);
    int b = atoi(argv[4]);
    int d = atoi(argv[5]);


    if (c < (p * i)){
        perror("c should be less than (p * i)");
        exit(1);
    }
    else if (b > MAX_BUFFER_SIZE){
        perror("buffer has to be less than 10");
        exit(1);
    }

    //initializes the struct Threads to hold the delay for the thread functions to use 
    struct Producer producer;
    struct Consumer consumer;

    producer.delay = d; 
    consumer.delay = d;

    pthread_t producer_thread, consumer_thread;
    pthread_create(&producer_thread, NULL, producer, (void *)&producer);
    pthread_create(&consumer_thread, NULL, consumer, (void *)&consumer);

    //allocates the buffer to the size asked by the user 
    int *buffer = (int *)malloc(b * sizeof(int)); 

    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    free(buffer);
    return 0;
}
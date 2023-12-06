#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 10
pthread_cond_t notempty = PTHREAD_COND_INITIALIZER;
pthread_cond_t notfull = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct Threads{
    int delay;
};

int itemCount = 0;

void *producer(void *arg) {
    //uses the argument given and creates a struct out of it to access delay 
    struct Threads *threadDelay = (struct ThreadArgs *)arg;

    while (1) {
        pthread_mutex_lock(&mutex);
        while (itemCount == MAX_BUFFER_SIZE) {
            pthread_cond_wait(&notfull, &mutex);
        }
        //Produces an item and adds it to the buffer 

        //Increment itemCount

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
    struct Threads *threadDelay = (struct ThreadArgs *)arg;
    while (1) {
        pthread_mutex_lock(&mutex);
        while (itemCount == 0) {
            pthread_cond_wait(&notempty, &mutex);
        }
        //Produces an item and adds it to the buffer 

        //Increment itemCount

        //Signal notempty
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
    //initializes the struct Threads to hold the delay for the thread functions to use 
    struct Threads args;
    args.delay = d; 
    pthread_t producer_thread, consumer_thread;
    pthread_create(&producer_thread, NULL, producer, (void *)&args);
    pthread_create(&consumer_thread, NULL, consumer, (void *)&args);

    if (c < (p * i)){
        perror("c should be less than (p * i)");
        exit(1);
    }
    else if (b > MAX_BUFFER_SIZE){
        perror("buffer has to be less than 10");
        exit(1);
    }

    //allocates the buffer to the size asked by the user 
    int *buffer = (int *)malloc(b * sizeof(int)); 

    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    free(buffer);
    return 0;
}
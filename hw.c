#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 10
pthread_cond_t notempty = PTHREAD_COND_INITIALIZER;
pthread_cond_t notfull = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int itemCount = 0;

void *producer(void *arg) {
    int *delay = (int *)arg;
    
    //while the buffer is full continue waiting til the notFull signal is given from the consumer 
    while (1) {
        pthread_mutex_lock(&mutex);
        while (itemCount == MAX_BUFFER_SIZE) {
            pthread_cond_wait(&notfull, &mutex);
        }
        //Produces an item and adds it to the buffer

        //Signal notempty
        itemCount++;
        pthread_cond_signal(&notempty);
        pthread_mutex_unlock(&mutex);

        usleep(*delay);
    }
    return NULL;
}

void *consumer(void *arg) {
    int *delay = (int *)arg;

    while (1) {
        pthread_mutex_lock(&mutex);
        //while there isn't anything in the buffer wait while on the notEmpty condition
        while (itemCount == 0) {
            pthread_cond_wait(&notempty, &mutex);
        }
        
        itemCount--;
        //Signal notFull after a consumption
        pthread_cond_signal(&notfull);
        pthread_mutex_unlock(&mutex);

        usleep(*delay);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    // Grabs the 5 numbers that represent [p c i b d]
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

    pthread_t producer_threads[p];
    pthread_t consumer_threads[c];

    int producer_delay = d;
    int consumer_delay = d;

    for (int i = 0; i < p; i++) {
        // Create producer threads
        pthread_create(&producer_threads[i], NULL, producer, (void *)&producer_delay);
    }

    for (int i = 0; i < c; i++) {
        // Create consumer threads
        pthread_create(&consumer_threads[i], NULL, consumer, (void *)&consumer_delay);
    }

    for (int i = 0; i < p; i++) {
        // Join producer threads
        pthread_join(producer_threads[i], NULL);
    }

    for (int i = 0; i < c; i++) {
        // Join consumer threads
        pthread_join(consumer_threads[i], NULL);
    }

    return 0;
}

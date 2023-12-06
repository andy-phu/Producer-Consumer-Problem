#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 10
#define MAX_THREADS 16

pthread_cond_t notempty = PTHREAD_COND_INITIALIZER;
pthread_cond_t notfull = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int itemsProduced = 0; //to keep track of how many items the multipe producers produced 
int currentItemCount = 0;

struct ProducerAttributes {
    int id;
    int* buffer;
    int bufferSize;
    int itemLimit;
    int currCount;
};

struct ConsumerAttributes{
    int* buffer;
    int bufferSize;
    int itemLimit;
    int currCount;
    int pAmount;
    int cAmount;
};

void *producer(void *arg) {
    struct ProducerAttributes *producerElement = (struct ProducerAttributes *)arg;
    int producerID = producerElement->id;
    int b = producerElement->bufferSize;
    int itemLimit = producerElement->itemLimit;
    //while the buffer is full continue waiting til the notFull signal is given from the consumer 
    while (1) {
        pthread_mutex_lock(&mutex);
        while (currentItemCount == b) {
            pthread_cond_wait(&notfull, &mutex);
        }

        //iterate throughout to find an empty spot
        for(int i = 0; i < b;i++){
            if (producerElement->buffer[i] == -1){
                producerElement->buffer[i] = itemsProduced;
                //Produces an item and adds it to the buffer
                (producerElement->currCount)++;
                itemsProduced++;
                pthread_cond_signal(&notempty);
                // Break the loop after placing the item in the buffer
                break; 
            }
        }

        pthread_mutex_unlock(&mutex);
        if (producerElement->currCount == producerElement->itemLimit){
            break;
        }
    }
    return NULL;
}

void *consumer(void *arg) {
    struct ConsumerAttributes *consumerElement = (struct ConsumerAttributes *)arg;
    int b = consumerElement->bufferSize;
    int p = consumerElement->pAmount;
    int c = consumerElement->cAmount;

    while (1) {
        pthread_mutex_lock(&mutex);
        //while there isn't anything in the buffer wait while on the notEmpty condition
        while (currentItemCount == 0) {
            pthread_cond_wait(&notempty, &mutex);
        }

        //iterate throughout to find a filled spot
        for(int i = 0; i < b;i++){
            if (consumerElement->buffer[i] != -1){
                consumerElement->buffer[i] = -1; //initialize the consumed spot back to -1
                //decrement the current item count
                currentItemCount--;
                //Signal notFull after a consumption
                pthread_cond_signal(&notfull);
            }
        }


        if (c == ((p*itemsProduced)/c)){
            break;
        }
        pthread_mutex_unlock(&mutex);


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
    else if(p > 16 || c > 16){
        perror("the max thread limit for producers or consumers is 16");
        exit(1);
    }

    //initialize buffer if the element is -1 then it's not filled 
    int *buffer = malloc(b * sizeof(int));
    for(int i = 0; i < b;i++){
        buffer[i] = -1; 
    }
    pthread_t producer_threads[p];
    pthread_t consumer_threads[c];
    struct ProducerAttributes *producerArray = malloc(p * sizeof(struct ProducerAttributes));
    struct ConsumerAttributes *consumerArray = malloc(p * sizeof(struct ConsumerAttributes));

    //initialize the producer attributes
    for (int i = 0; i < p; ++i) {
        producerArray[i].id = i;
        producerArray[i].buffer = buffer; // Assign the buffer to each producer so that both functions can use it 
        producerArray[i].bufferSize = b;
        producerArray[i].itemLimit = i;
        producerArray[i].currCount = 0;
    }

    for (int i = 0; i < c; ++i) {
        consumerArray[i].buffer = buffer; // Assign the buffer to each producer so that both functions can use it 
        consumerArray[i].bufferSize = b;
        consumerArray[i].itemLimit = i;
        consumerArray[i].currCount = 0;
        consumerArray[i].pAmount = p;
        consumerArray[i].cAmount = c;
    }


    int delay = 500000; 

    for (int i = 0; i < p; i++) {
        // Create producer threads
        pthread_create(&producer_threads[i], NULL, producer, (void *)&producerArray[i]);
        if (d == 1){
            usleep(delay);
        }
    }

    for (int i = 0; i < c; i++) {
        // Create consumer threads
        pthread_create(&consumer_threads[i], NULL, consumer, (void *)&consumerArray[i]);
        if (d == 0){
            usleep(delay);
        }
    }

    for (int i = 0; i < p; i++) {
        // Join producer threads
        pthread_join(producer_threads[i], NULL);
    }

    for (int i = 0; i < c; i++) {
        // Join consumer threads
        pthread_join(consumer_threads[i], NULL);
    }

    free(buffer);
    free(producerArray);
    free(consumerArray);

    return 0;
}

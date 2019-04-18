#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

typedef int buffer_item;
#define BUFFER_SIZE 5

buffer_item buffer[BUFFER_SIZE];
pthread_mutex_t mutex;
sem_t empty;
sem_t full;

int counter = 0, add = 0, rem = 0;

int insert_item(buffer_item);
int remove_item(buffer_item*);
void *producer(void*);
void *consumer(void*);

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Must pass 3 parameters with the function call\n");
        exit(1);
    }
    int uTime = atoi(argv[1]);
    int numProducers = atoi(argv[2]);
    int numConsumers = atoi(argv[3]);
    srand(time(0));
    pthread_mutex_init(&mutex,NULL);
    sem_init(&empty,0,BUFFER_SIZE);
    sem_init(&full,0,0);
    pthread_t producers[numProducers];
    pthread_t consumers[numConsumers];
    for(int i = 0; i < numProducers; i++)
        pthread_create(&producers[i], NULL, producer, NULL);
    for(int i = 0; i < numConsumers; i++)
        pthread_create(&consumers[i], NULL, consumer, NULL);
    sleep(uTime);
    printf("Exiting the program\n");
    return 0;
}

int insert_item (buffer_item item) {
    int success = -1;
    if (sem_wait(&empty) == 0)
        printf("Waiting for EMPTY semaphore\n");
    pthread_mutex_lock(&mutex);
    if(counter < BUFFER_SIZE) {
        buffer[add] = item;
        add = (add+1) % BUFFER_SIZE;
        counter++;
        success = 0;
    }
    pthread_mutex_unlock(&mutex);
    if (sem_post(&full) == 0)
        printf("Signaling the FULL semaphore\n");
    return success;
}

int remove_item(buffer_item *item) {
    int success = -1;
    if (sem_wait(&full) == 0)
        printf("Waiting for FULL semaphore\n");
    pthread_mutex_lock(&mutex);
    if(counter > 0) {
        *item = buffer[rem];
        buffer[rem] = 0;
        rem = (rem+1) % BUFFER_SIZE;
        counter--;
        success = 0;
    }
    pthread_mutex_unlock(&mutex);
    if(sem_post(&empty) == 0)
        printf("Signaling the EMPTY semaphore\n");
    return success;
}

void *producer(void *p) {
    (void)p;
    buffer_item item;
    while(1) {
        sleep((rand() % BUFFER_SIZE) );
        item = rand();
        if (insert_item(item))
            fprintf(stderr, "Error occurred\n");
        else {
            pthread_t self_id;
            self_id = pthread_self();
            printf("ThreadID %zu Producing item %d. Items in buffer: %d\n", self_id, item, counter);
        }
    }
}

void *consumer(void *c){
    (void)c;
    buffer_item item;
    while (1) {
        sleep((rand() % BUFFER_SIZE));
        if (remove_item(&item))
            fprintf(stderr,"Error occured\n");
        else {
            pthread_t self_id;
            self_id=pthread_self();
            printf("ThreadID %zu Consuming item %d. Items in buffer: %d\n", self_id, item, counter);
        }
    }
}
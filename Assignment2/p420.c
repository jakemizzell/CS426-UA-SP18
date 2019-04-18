/**
 * 4.20.c
 * Written by Jake Mizzell for CS426
 * This solves problem 4.20 described in Operating Systems Concepts textbook
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#define MIN_PID 300
#define MAX_PID 5000
#define TRUE 1
#define FALSE 0
#define RANGE (MAX_PID - MIN_PID + 1)
#define THREAD_COUNT 100
sem_t SEM;

struct PidTable {
    int pid;
    int isAvailable;
}*PID;

int allocate_map(void);
int allocate_pid(void);
void release_pid(int pid);
void *createThread(void *);

int main(void) {
    allocate_map();
    srand(time(NULL));
    sem_init(&SEM,0,1);
    void *status;
    int pid = 0;
    pthread_t thread[100];
    //loop to create the threads
    for (int i=0; i < THREAD_COUNT; i++) {
        pid = pthread_create(&thread[i],NULL,createThread,NULL);
        //error checking
        if (pid) {
            printf("Error creating thread\n");
            exit(1);
        }
    }
    //loop to join the threads
    for (int i=0; i < THREAD_COUNT; i++) {
        pid = pthread_join(thread[i],&status);
        //error checking
        if(pid) {
            printf("Error; return code from pthread_join() is %d\n", pid);
            exit(1);
        }
    }
    return 0;
}

int allocate_map(void) {
    PID = (struct PidTable *)calloc((RANGE),sizeof(struct PidTable));
    //if it is unsuccessful return -1;
    if (PID == NULL)
        return -1;
    //loop to set initialize the PIDs
    for (int i=0; i<RANGE; i++) {
        //base case
        if (i == 0) {
            PID[0].pid = MIN_PID;
            PID[0].isAvailable = TRUE;
        }
        //to allocated all but index 0
        else {
            //set the next one to the previous PID plus 1
            PID[i].pid = PID[i - 1].pid + 1;
            PID[i].isAvailable = TRUE;
        }
    }
    return 1;
}

int allocate_pid(void) {
    //loop to allocate, if that spot is available fill it and change flag
    for (int i=0; i<RANGE; i++) {
        if (PID[i].isAvailable == TRUE) {
            PID[i].isAvailable = FALSE;
            return PID[i].pid;
        }
    }
    return -1;
}

void release_pid(int pid) {
    PID[pid-MIN_PID].isAvailable = TRUE;
}

void *createThread (void *v) {
    (void)v;
    int time;
    int pid = allocate_pid();
    sem_wait(&SEM);
    sem_post(&SEM);
    if (pid != -1) {
        printf("New Process Allocated PID = %d\n",pid);
        time = rand()%10;
        sleep(time);
        printf("Process %d releasing PID\n",pid);
        release_pid(pid);
    }
    pthread_exit(NULL);
}
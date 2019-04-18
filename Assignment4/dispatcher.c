#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

#define UNINITIALIZED 0
#define INITIALIZED 1
#define READY 2
#define RUNNING 3
#define SUSPENDED 4
#define TERMINATED 5

struct queue {
    pid_t pid;
    char *args[3];
    int arrivalTime;
    int priority;
    int processorTime;
    int status;
    struct queue *next;
};

typedef struct queue Queue;
typedef Queue *QueuePtr;

QueuePtr startProcess(QueuePtr);
QueuePtr suspendProcess(QueuePtr);
QueuePtr terminateProcess(QueuePtr);
int checkQueues(QueuePtr *);
QueuePtr newQueue();
QueuePtr enqueue(QueuePtr, QueuePtr);
QueuePtr dequeue(QueuePtr *);


int main(int argc, char **argv) {
    if (argc < 2) {
        printf("must include text file with command call\n");
        exit(1);
    }
    QueuePtr inputQueue = NULL;
    QueuePtr userQueue[4];
    int i;
    //initalize an array of Queues for userQueue
    for(i = 0; i < 4; userQueue[i++] = NULL);
    QueuePtr currentProcess = NULL;
    QueuePtr process = NULL;
    int timer = 0;
    FILE *fp;
    fp = fopen(argv[1], "r");
    //error message
    if (fp == NULL) {
        printf("Error in opening file.\n");
        exit(1);
    }
    //read the file and put them into the inputQueue
    while (!feof(fp)) {
        process = newQueue();
        if (fscanf(fp, "%d, %d, %d", &(process->arrivalTime), &(process->priority), &(process->processorTime)) != 3) {
            free(process);
            continue;
        }
        process->status = INITIALIZED;
        //printf("Just read: %d %d %d\n",process->arrivalTime, process->priority, process->processorTime);
        inputQueue = enqueue(inputQueue, process);
    }
    fclose(fp);
    //while there's things in the queues or if a process is running
    while (inputQueue || checkQueues(userQueue) >= 0 ||currentProcess) {
        //while there's things in the inputQueue and it's arrival and timer match
        while (inputQueue && inputQueue->arrivalTime <= timer) {
            process = dequeue(&inputQueue);
            process->status = READY;
            process->priority = 0;
            userQueue[process->priority] = enqueue(userQueue[process->priority], process);
        }
        //if a process is running
        if (currentProcess && currentProcess->status == RUNNING) {
            //decrement the time
            currentProcess->processorTime--;
            //if you run out of time
            if (currentProcess->processorTime <= 0) {
                //terminate it
                currentProcess = terminateProcess(currentProcess);
                free(currentProcess);
                currentProcess = NULL;
            }
            //if there is anything in the userQueue waiting
            else if (checkQueues(userQueue) >= 0) {
                //suspend the running process
                suspendProcess(currentProcess);
                //reduce the priority
                if(++(currentProcess->priority) >= 4)
                    currentProcess->priority = 4-1;
                //enqueue it into the right place now
                userQueue[currentProcess->priority] = enqueue(userQueue[currentProcess->priority],currentProcess);
                currentProcess = NULL;
            }
        }
        //if nothing is running and the userQueue isn't empty
        if(!currentProcess && (i = checkQueues(userQueue)) >= 0) {
            //dequeue form the userQueue
            currentProcess = dequeue(&userQueue[i]);
            //run it or restart it if it was suspended
            startProcess(currentProcess);
        }
        sleep(1);
        timer += 1;
    }
    exit(0);
}

//Looks at queues and sees which is the highest non empty one, returning it
int checkQueues(QueuePtr *q) {
    int i;
    for(i = 0; i < 4; i++)
        if(q[i])
            return i;
    return -1;
}

//Function to both start and restart it
QueuePtr startProcess (QueuePtr q) {
    //if the process isn't suspended the start it
    if (q->pid == 0) {
        switch (q->pid = fork()) {
            case -1:
                perror("startProcess");
                exit(1);
            case 0:
                q->pid = getpid();
                q->status = RUNNING;
                execvp (q->args[0], q->args);
                perror (q->args[0]);
                exit (2);
        }
    }
    //if it was suspended the restart it
    else
        kill (q->pid, SIGCONT);
    q->status = RUNNING;
    return q;

}

//suspends the process
QueuePtr suspendProcess(QueuePtr q) {
    int status;
    kill(q->pid, SIGTSTP);
    waitpid(q->pid, &status, WUNTRACED);
    q->status = SUSPENDED;
    return q;
}

//terminates the process
QueuePtr terminateProcess (QueuePtr q) {
    int status;
    kill(q->pid, SIGINT);
    waitpid(q->pid, &status, WUNTRACED);
    q->status = TERMINATED;
    return q;
}

//sets up a new queue
QueuePtr newQueue() {
    QueuePtr new;
    if ((new = (QueuePtr) malloc (sizeof(Queue)))) {
        new->pid = 0;
        new->args[0] = "./process";
        new->args[1] = NULL;
        new->arrivalTime = 0;
        new->priority = 0;
        new->processorTime = 0;
        new->status = UNINITIALIZED;
        new->next = NULL;
        return new;
    }
    perror("Allocating memory for new process");
    return NULL;
}

//enqueues at the end
QueuePtr enqueue(QueuePtr a, QueuePtr b) {
    QueuePtr n = a;
    b->next = NULL;
    if (a) {
        while (a->next)
            a = a->next;
        a->next = b;
        return n;
    }
    return b;
}

//removes from the front
QueuePtr dequeue(QueuePtr *a) {
    QueuePtr c;
    if(a && (c = *a)) {
        *a = c->next;
        return c;
    }
    return NULL;
}

/**
 * 4.21.c
 * Written by Jake Mizzell. Linked list code is recycled from CS201 projects but modified to insert in order.
 * This solves problem 4.21 described in Operating Systems Concepts textbook
 * It takes user input and calculates min, max, and avg values using threads
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct sllnode
{
    int value;
    struct sllnode *next;
} sllnode;

typedef struct sll
{
    sllnode *head;
    sllnode *tail;
    int size;
} sll;

int avg = 0;
int min = 0;
int max = 0;

void getInput(sll*);
void average (sll*);
void minimum (sll*);
void maximum (sll*);
sll *newSLL();
void insertSLL(sll*,int);
void displaySLL(sll*);

int main (void) {
    //initialize an singly linked list to hold all the arguments
    sll *items = newSLL();
    //get the users input
    getInput(items);
    /*printf("\n");
    displaySLL(items);
    printf("\n");*/
    //create all the threads
    pthread_t thread1,thread2,thread3;
    int t1 = pthread_create(&thread1, NULL, (void *)average, (void *)items);
    //error checking
    if (t1) {
        printf("Error in pthread_create(), t1 = %d\n", t1);
        exit(1);
    }
    int t2 = pthread_create(&thread2, NULL, (void *)minimum, (void *)items);
    //error checking
    if (t2) {
        printf("Error in pthread_create(), t2 = %d\n", t2);
        exit(1);
    }
    int t3 = pthread_create(&thread3, NULL, (void *)maximum, (void *)items);
    //error checking
    if (t3) {
        printf("Error in pthread_create(), t3 = %d\n", t3);
        exit(1);
    }
    /*printf("pthread_create() for Thread 1 returns: %d\n",t1);
    printf("pthread_create() for Thread 2 returns: %d\n",t2);
    printf("pthread_create() for Thread 3 returns: %d\n\n",t3);*/
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    printf("\nThe average value is %d\n", avg);
    printf("The minimum value is %d\n", min);
    printf("The maximum value is %d\n", max);
    return 0;
}

void getInput (sll *items) {
    printf("Enter the numbers you want to calculate the average, minimum, and maximum value for\n");
    //variable to determine if you've already done getChar() that loop
    int flag = 0;
    int num = 0;
    //Read everything the user enters until a newline
    int c = getchar();
    while (c != '\n') {
        //exit if non letter/whitespace is entered
        if ((c < 48 || c > 57) && (c != ' ' && c != '\t')) {
            printf("Exiting: you must enter only numbers\n");
            exit(1);
        }
        flag = 0;
        //loop to read up space
        while(c == ' ' || c == '\t') {
            flag = 1;
            c = getchar();
        }
        //if it's a number
        while(c>47 && c<58 && c != '\n') {
            flag = 2;
            //different math if it's the first number vs trailing numbers
            if (num == 0)
                num += c - 48;
            else
                num = num * 10 + (c - 48);
            c = getchar();

        }
        //this makes sure you only insert numbers
        if (flag == 2)
            insertSLL(items, num);
        num = 0;
        //so you don't get the next char if you already did earlier in the loop
        if (flag == 0)
            c = getchar();
    }

}

//function to figure out the average
void average (sll *items) {
    if (items == 0) {
        printf("Nothing was inputted\n");
        return;
    }
    sllnode *curr = malloc(sizeof(sll));
    curr = items->head;
    int num = 0;
    //read through the list and add up the numbers
    for(int i = 0; curr != NULL; i++) {
        num += curr->value;
        curr = curr->next;
    }
    //do the sum of the numbers divided by how many their are
    avg = num / items->size;
}

//function to calculate the minimum value
void minimum (sll *items) {
    //return the head since it's a sorted list
    min = items->head->value;
}

//funciton to calculate the minimum value
void maximum (sll *items) {
    //return the tail since it's a sorted list
    max = items->tail->value;
}

sll *newSLL() {
    sll *items = malloc(sizeof(sll));
    if (items == 0) {
        fprintf(stderr,"out of memory");
        exit(-1);
    }
    items->head = 0;
    items->tail = 0;
    items->size = 0;
    return items;
}

void insertSLL(sll *items,int value) {
    sllnode *newNode = malloc(sizeof(sll));
    newNode->value = value;
    newNode->next = NULL;
    //insert at head if it's the first or the value needs to be the head
    if (items->size == 0 || value < items->head->value) {
        newNode->next = items->head;
        items->head = newNode;
        if (items->head->next == NULL) {
            items->tail = newNode;
        }
    }
    //insert in order
    else {
        int index = 0;
        sllnode *curr = items->head;
        sllnode *prev = NULL;
        //loop to find where a value needs to be put
        while (curr != NULL && curr->value <= value) {
            prev = curr;
            curr = curr->next;
            index++;
        }
        //relink things
        newNode->next = curr;
        prev->next = newNode;
        //to update tail pointer if needed
        if (index == items->size) {
            items->tail->next = newNode;
            items->tail = newNode;
        }
    }
    //update the size of the SLL
    items->size++;
}

//This function is just for error checking in this project
void displaySLL(sll *items) {
    if (items == 0) {
        printf("Nothing was inputted\n");
        return;
    }
    sllnode *curr = malloc(sizeof(sll));
    curr = items->head;
    for(int i = 0; curr != NULL; i++) {
        //print a space if it's not the first one
        if (i > 0)
            printf(" ");
        printf("%d",curr->value);
        curr = curr->next;
    }
}
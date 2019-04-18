/**
 * 3.20.c
 * Written by Jake Mizzell for CS426
 * Creates an API to solve 3.20 described in Operating Systems Concepts textbook
 */

#include <stdio.h>
#include <stdlib.h>

#define MIN_PID 300
#define MAX_PID 5000
#define TRUE 1
#define FALSE 0
#define RANGE (MAX_PID - MIN_PID + 1)

struct PidTable {
    int pid;
    int isAvailable;
}*PID;

int allocate_map(void);
int allocate_pid(void);
void release_pid(int pid);

int allocate_map(void) {
    PID = (struct PidTable *)calloc((RANGE),sizeof(struct PidTable));
    //if it is unsucessful return -1;
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